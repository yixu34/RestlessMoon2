#include <string>
#include <boost/optional.hpp>

#include "game.hpp"
#include "bindings.hpp"
#include "camera.hpp"
#include "entity.hpp"
#include "scripting.hpp"
#include "renderer.hpp"
#include "humanplayer.hpp"
#include "aiplayer.hpp"
#include "animatedentity.hpp"
#include "framemanager.hpp"
#include "eventmanager.hpp"
#include "input.hpp"
#include "cvar.hpp"
#include "kdtree.hpp"
#include "staticentity.hpp"
#include "trigger.hpp"
#include "speech.hpp"

CVar<bool> aiEnabled("aiEnabled", false);
CVar<bool> dieAnotherDay("dieAnotherDay", false);
CVar<float> deadEntityTime("deadEntityTime", 4.0f);
GamePtr game;

Game::Game()
	: Frame("game", 2)
{
	_levelLoaded     = false;
	player           = HumanPlayerPtr(new HumanPlayer());
    currControllable = player;
    _currentLevel    = ""; 
	_freezeTimer     = 0.0f;
	_whoIsTalking    = "Nobody";
	_renderScriptFunction = "doRenderScript()";
	_renderStrings["talker"] = _whoIsTalking;
}
Game::~Game()
{
    for(WeaponList::iterator it = _weaponsList.begin(); it != _weaponsList.end(); it++)
        delete (*it).second;

    _weaponsList.clear();

	for(PowerUpCache::iterator it = _powerUpCache.begin(); it != _powerUpCache.end(); it++)
        delete (*it).second;

	for(int i = 0; i != prevWeapons.size(); i++)
		delete prevWeapons[i];

	prevWeapons.clear();
    _powerUpCache.clear();
}

bool Game::initialize()
{
	frameManager->addFrame(shared_from_this());
	updateRenderVars();
	return true;
}

void Game::onEnable()
{
	input->setMouseExclusive(true);
	bindings.reset();
}

void Game::onDisable()
{
	input->setMouseExclusive(false);
}

void Game::registerHandlers()
{
	registerHandler(
		EventType(Ev_Start_Game::eventName), 
		EventHandlerFunctorPtr(new BaseEventHandlerFunctor<Game>(this, &Game::onStartGame)));
}

bool Game::onStartGame(const Event &event)
{
	onSpeechFinished = "";

    EvMapLoad_data *eventData = event.getRawData<EvMapLoad_data>();
	int type = eventData->_type;
	AnimatedEntityPtr tempPlayer;
	float tempHealth;
	int prevWeaponNumTemp;
	int prevNumGrenadesTemp;
	std::vector<WeaponInfo *> tempPrevWeapons;
	
	if(type == 3)
	{
		tempPlayer = player->getEntity();
		tempHealth = player->getEntity()->getHealth();
		prevWeaponNumTemp = player->getEntity()->getWeaponNum();
		std::vector<WeaponInfo *>* tempVec = player->getEntity()->getWeaponsVec();
		prevNumGrenadesTemp = player->getEntity()->getNumGrenades();
		for(int i = 0; i != tempVec->size(); i++)
			tempPrevWeapons.push_back(new WeaponInfo(*tempVec->at(i)));
	}

	loadLevel(eventData->_mapName);
	switch (type)
	{
		//New Game
		case 0:
			scripting->executeString("addEntity('units/selena.lua', 0, 0, 0)");
			player->getEntity()->addWeapon(game->getFromWeaponCache("Battle Rifle"));
			break;

		//Load Game
		case 1:
			scripting->executeString("addEntity('units/selena.lua', 0, 0, 0)");
			player->getEntity()->addWeapon(game->getFromWeaponCache("Battle Rifle"));
			player->getEntity()->addWeapon(game->getFromWeaponCache("Shotgun"));
			player->getEntity()->addWeapon(game->getFromWeaponCache("Rocket Launcher"));
			player->getEntity()->addWeapon(game->getFromWeaponCache("Flamethrower"));
			break;

		//restart
		case 2:
			scripting->executeString("addEntity('units/selena.lua', 0, 0, 0)");
			player->getEntity()->setHealth(prevHealth);
			for(int i = 0; i != prevWeapons.size(); i++)
			{
				player->getEntity()->addWeapon(new WeaponInfo(*prevWeapons[i]));
			}
			player->getEntity()->setWeaponNum(prevWeaponNum);
			player->getEntity()->setNumGrenades(prevNumGrenades);
			break;

		//next level
		case 3:
			scripting->executeString("addEntity('units/selena.lua', 0, 0, 0)");
			player->getEntity()->setHealth(tempHealth);
			for(int i = 0; i != tempPrevWeapons.size(); i++)
			{
				player->getEntity()->addWeapon(new WeaponInfo(*tempPrevWeapons[i]));
			}
			player->getEntity()->setWeaponNum(prevWeaponNumTemp);
			player->getEntity()->setNumGrenades(prevNumGrenadesTemp);
			break;

		default:
			assert(false);
	}

	for(int i = 0; i != prevWeapons.size(); i++)
		delete prevWeapons[i];

	prevWeapons.clear();

	std::vector<WeaponInfo *>* tempVec = player->getEntity()->getWeaponsVec();
	for(int i = 0; i != tempVec->size(); i++)
		prevWeapons.push_back(new WeaponInfo(*tempVec->at(i)));

	prevHealth = player->getEntity()->getHealth();
	prevWeaponNum = player->getEntity()->getWeaponNum();
	prevNumGrenades = player->getEntity()->getNumGrenades();

	camera->attachToEntity(player->getEntity());
	
	setEnabled(true);
	return false;
}

void Game::addAIPlayer(const AnimatedEntityPtr &aiEntity)
{
	_aiPlayers.push_back(AIPlayerPtr(new AIPlayer(aiEntity)));
}

void Game::loadLevel(const std::string& levelName)
{
	if(_levelLoaded)
		unloadLevel();

	kdTree.clear();
	_whoIsTalking = "Nobody";
    _currentLevel = levelName;
	scripting->executeFile(levelName);
	std::string renderScript   = scripting->getGlobal<std::string>("renderScript");
	std::string skyboxMeshName = scripting->getGlobal<std::string>("skybox");

	renderer->setRenderScript(renderScript);
	
	StaticMeshPtr skybox = staticMeshCache.get(skyboxMeshName);
	MYASSERT(skybox);

	renderer->addRenderable(skybox, "skybox", 0);
	_levelLoaded = true;

	// Construct the kd-tree for the collision geometry.
	kdTree.build();

    _frame = 0;
}

void Game::unloadLevel()
{
	_entities.clear();
	_deadEntities.clear();
	_aiPlayers.clear();
    _projectiles.clear();
	_powerUps.clear();
	player->setEntity(AnimatedEntityPtr());
	camera->attachToEntity(EntityPtr());
	renderer->clearAllObjects();
	for(int i = 0; i != _triggers.size(); i++)
		_triggers[i]->cleanUp();
	_triggers.clear();
	_levelLoaded = false;
	_freezeTimer = 0.0f;
}

void Game::reloadCurrentLevel()
{
    if(!_levelLoaded)
        return;

    eventManager->raiseEvent(EventPtr(new Ev_Start_Game(_currentLevel, 2)));
}

void Game::timepass()
{
    assert(player->getEntity());

    _frame++;

    //if the current frame is 1 or 2, then the dt is really high 
    //due to the level load time we should ignore the first 2 frames, 
    //since they don't really give us "valid" values
    if(_frame < 3)
        return;

	_freezeTimer -= getDtSeconds();
	Frame::timepass();

    for(int i = 0; i != _entities.size(); i++)        
        _entities[i]->resetFiring();

	//update the main player & ai players to get their velocity vectors
	currControllable->updateMovement();
	player->timepass();

	// Update the AI players.
	if (aiEnabled)
	{
		for(int i = 0; i != _aiPlayers.size(); i++)
			_aiPlayers[i]->timepass();
	}

	//we get the displacement vector & reset the floor flag
	for(int i = 0; i != _entities.size(); i++)
	{
		_entities[i]->setOnTheFloor(false);
		_entities[i]->normalizeVelocity();
	}

	for(PowerUpList::iterator it = _powerUps.begin(); it != _powerUps.end();)
	{
		if((*it)->isDead())
		{
			//if the played needed the powerup, then erase and move along
			it= _powerUps.erase(it);
			continue;
		}
		it++;
	}
	checkCollisionWithPowerUp();

    //we get the displacement vector & reset the floor flag
    for(int i = 0; i != _projectiles.size(); i++)
    {
        _projectiles[i]->setOnTheFloor(false);
        _projectiles[i]->normalizeVelocity();
    }

	//we get the displacement vector & reset the floor flag
    for(int i = 0; i != _powerUps.size(); i++)
    {
		if(!_powerUps[i]->onTheFloor())
			_powerUps[i]->normalizeVelocity();
    }

	//do collision detection after they have their displacement vectors, so that we can modify them so they don't collide
    for(int i = 0; i != _powerUps.size(); i++)
    {
		if(_powerUps[i]->onTheFloor())
			continue;
        kdTree.collideWithEntity(_powerUps[i]);
		_powerUps[i]->timepass();
    }

	//do collision detection after they have their displacement vectors, so that we can modify them so they don't collide
	for(int i = 0; i != _entities.size(); i++)
	{
		checkCollision(_entities[i]);
		kdTree.collideWithEntity(_entities[i]);
	}

    //do collision detection after they have their displacement vectors, so that we can modify them so they don't collide
    for(int i = 0; i != _projectiles.size(); i++)
    {
        checkCollisionWithProjectiles(_projectiles[i]);
        kdTree.collideWithEntity(_projectiles[i]);
	}
	
	//now we can go through and move the objects, or remove them from the list if dead
	for(EntityList::iterator it = _entities.begin(); it != _entities.end();)
	{
		const EntityPtr &currEntity = *it;
        
        //we need to remove the entity since it's dead
		if(currEntity->isDead())
		{
            //if it's the main player, reload the level
            if(currEntity == player->getEntity())
			{
				// The dead player still needs to timepass() to play animations.
				currEntity->timepass(); 
				it++;
				continue;
			}

            //remove it's controller if it has one.
			// If the entity leaves a corpse, then we move it to the dead list.
			// Otherwise, we can get rid of it completely.
			if (currEntity->getDeathMode() == Entity::death_leaves_corpse)
				_deadEntities.push_back(currEntity);

			removeEntityController((*it)->getIDNum());
			it = _entities.erase(it);

            continue;
		}

        //the entity is dead, should schedule itself for deletion
		else if(currEntity->getHealth() <= 0) 
			currEntity->killEntity();

        //now we know it's not dead or dying, we can update it
		else 
		    currEntity->timepass();

        it++;
    }

	for(TriggerList::iterator it = _triggers.begin(); it != _triggers.end();)
	{
		bool done = (*it)->timepass();
		if(done)
			it = _triggers.erase(it);
		else
			it++;
	}

    //now we can go through and move the _projectiles, or remove them from the list if dead
    for(EntityList::iterator it = _projectiles.begin(); it != _projectiles.end();)
    {
        const EntityPtr &currProj = *it;

        //we need to remove the entity since it's dead
        if(currProj->isDead())
        {
            it = _projectiles.erase(it);
            continue;
        }

        //the entity is dead, should schedule itself for deletion
        else if(currProj->getHealth() <= 0) 
            currProj->killEntity();

        //now we know it's not dead or dying, we can update it
        else 
            currProj->timepass();

        it++;
    }

	// Update the dead entities' animations.
	for (EntityList::iterator it = _deadEntities.begin();
		 it != _deadEntities.end(); it++)
	{
		const EntityPtr &currEntity = *it;
		if (getTimeSeconds() - currEntity->getTimeOfDeath() < deadEntityTime)
		{
			// We can't remove the entity, because the renderer needs to 
			// keep it around.
			currEntity->setOnTheFloor(false);
			currEntity->normalizeVelocity();
			kdTree.collideWithEntity(currEntity);
			currEntity->timepass();
		}
	}

	// Get new values for the rendering variables.
	updateRenderVars();
}

void Game::addEntity(const EntityPtr& newEntity)
{
	_entities.push_back(newEntity);
}

void Game::addProjectile(
	WeaponInfo* newBullet, 
	const D3DXVECTOR3& pos, 
	float yawAngle, 
	float pitchAngle, 
	float cameraOffsetY, 
	float cameraOffsetZ)
{
    for(int x = 0; x < newBullet->bulletOffset.size(); x++)
    {
        D3DXVECTOR3 offset = newBullet->bulletOffset[x];
        D3DXVECTOR3 offsetXZ(offset.x, 0.0f, offset.z);
        D3DXVECTOR3 offsetY(0.0f, offset.y, 0.0f);

        // After we do some rotations to figure out the orientation of the bullet,
        // we have to offset it by the -camera- y-coordinate, not the shoulder's
        // y-coordinate.
        D3DXMATRIX cameraTrans;
        D3DXMatrixTranslation(&cameraTrans, 0.0f, cameraOffsetY - offset.y, 0.0f);
        D3DXVec3TransformCoord(&offsetY, &offsetY, &cameraTrans);

        D3DXVec3Normalize(&offsetXZ, &offsetXZ);

        // Rotate by the pitch.
        D3DXMATRIX pitchMatrix;
        D3DXMatrixIdentity(&pitchMatrix);
        D3DXMatrixRotationX(&pitchMatrix, -pitchAngle);
        D3DXVec3TransformCoord(&offsetXZ, &offsetXZ, &pitchMatrix);

        // Keep the bullet at the appropriate distance from yourself.
        offsetXZ *= std::abs(offset.z);

        // Place the bullet where your eyes are.
        D3DXVECTOR3 pitchPos = offsetY + offsetXZ;

        // Rotate by the yaw.
        D3DXMATRIX yawMatrix;
        D3DXMatrixIdentity(&yawMatrix);
        D3DXMatrixRotationY(&yawMatrix, yawAngle);
        D3DXVec3TransformCoord(&pitchPos, &pitchPos, &yawMatrix);

        // Now the bullet should be coming out of your eyes.
        // We have to shift the bullet down so that it comes out
        // of your gun again.
        D3DXVECTOR3 shoulderOffset(0.0f, -1.0f, 0.0f);
        D3DXVec3TransformCoord(&shoulderOffset, &shoulderOffset, &pitchMatrix);
        D3DXVec3TransformCoord(&shoulderOffset, &shoulderOffset, &yawMatrix);
        D3DXVec3Normalize(&shoulderOffset, &shoulderOffset);
        shoulderOffset *= cameraOffsetY - offset.y;

        NewEntityStruct newEntityParams;
        newEntityParams.position		    = pos + pitchPos + shoulderOffset;
        newEntityParams.name			    = newBullet->weaponName;
        newEntityParams.mesh			    = newBullet->projectileMesh;
        newEntityParams.deathSound		    = newBullet->projectileHitSound;
        newEntityParams.hitPoints		    = newBullet->projectileLife;
        newEntityParams.footStepSound	    = newBullet->projectileIntermedSound;
        newEntityParams.speed			    = newBullet->projectileSpeed;
        newEntityParams.collisionDamage     = newBullet->projectileDamage;
        newEntityParams.boundBox		    = newBullet->bulletBoundBox;
        newEntityParams.radius		        = 0; // for faster collisions against the kd-tree.
        newEntityParams.coeffOfRestitution  = newBullet->projectileCoeffOfRestitution;
        newEntityParams.weight              = newBullet->projectileWeight;
        newEntityParams.diesAfterTime		= newBullet->projectileDiesAfterTime;
        newEntityParams.diesOnImpact		= newBullet->projectileDiesOnImpact;
        newEntityParams.gravitySensitive	= newBullet->projectileGravitySensitive;
        newEntityParams.isStatic            = true;
        newEntityParams.isProjectile        = true;
        newEntityParams.dropsItemsAtDeath	= false;
        newEntityParams.radialDamage		= newBullet->projectileRadialDamage;
        newEntityParams.yawAngle            = yawAngle   + util::randFloat(-newBullet->inaccuracy, newBullet->inaccuracy);    
        newEntityParams.pitchAngle          = pitchAngle + util::randFloat(-newBullet->inaccuracy, newBullet->inaccuracy);

        StaticEntityPtr newEntity = StaticEntityPtr(new StaticEntity(&newEntityParams));
        _projectiles.push_back(newEntity);

		// HACK:  Rather than come up with a new field for leaving a rocket trail,
		// we'll just assume that the projectile is a rocket if:
		// 1) it's not gravity sensitive
		// 2) it deals radial damage.
		if (!newEntityParams.gravitySensitive && newEntityParams.radialDamage > 0)
			renderer->getParticleEmitter("missiles")->addEmitterEntity(newEntity, 6);
    }
}

void Game::checkCollisionWithProjectiles(EntityPtr projectile)
{
    if(projectile->isDead())
        return;

    D3DXVECTOR3 futurePos = projectile->getPos() + projectile->getVelocity();

    for(int it = 0; it != _entities.size(); it++)
    {
        if(_entities[it]->isDead() || (_entities[it] == player->getEntity() && dieAnotherDay))
            continue;

        D3DXVECTOR3 otherEntityVel = _entities[it]->getVelocity();
        BoundingBox collideEntityBoundBox = _entities[it]->getBoundingBox();
        collideEntityBoundBox.max += otherEntityVel;
        collideEntityBoundBox.min += otherEntityVel;

		if(collideEntityBoundBox.containsPoint(futurePos))
        {
            //deal their collision damages
            projectile->takeDamage(_entities[it]->getCollideDamage());
            _entities[it]->takeDamage(projectile->getCollideDamage());

			// Raise an event signaling that the entity got hit.
			eventManager->raiseEvent(EventPtr(new Ev_Entity_Hit(
			                                          projectile->getPos(), 
													  projectile->getVelocity(), 
													  _entities[it]->getOnHitMode())));
        }
    }

}
void Game::checkCollision(EntityPtr entityCollide)
{
	if(entityCollide->isDead())
		return;

	D3DXVECTOR3 entityvel = entityCollide->getVelocity();
	BoundingBox entityBoundBox = entityCollide->getBoundingBox();
	entityBoundBox.max += entityvel;
	entityBoundBox.min += entityvel;

	D3DXVECTOR3 leftBottomNear  = D3DXVECTOR3(entityBoundBox.min.x,entityBoundBox.min.y,entityBoundBox.min.z);
	D3DXVECTOR3 leftBottomFar   = D3DXVECTOR3(entityBoundBox.min.x,entityBoundBox.min.y,entityBoundBox.max.z);
	D3DXVECTOR3 leftTopNear     = D3DXVECTOR3(entityBoundBox.min.x,entityBoundBox.max.y,entityBoundBox.min.z);
	D3DXVECTOR3 leftTopFar      = D3DXVECTOR3(entityBoundBox.min.x,entityBoundBox.max.y,entityBoundBox.max.z);
	D3DXVECTOR3 rightBottomNear = D3DXVECTOR3(entityBoundBox.max.x,entityBoundBox.min.y,entityBoundBox.min.z);
	D3DXVECTOR3 rightBottomFar  = D3DXVECTOR3(entityBoundBox.max.x,entityBoundBox.min.y,entityBoundBox.max.z);
	D3DXVECTOR3 rightTopNear    = D3DXVECTOR3(entityBoundBox.max.x,entityBoundBox.max.y,entityBoundBox.min.z);
	D3DXVECTOR3 rightTopFar     = D3DXVECTOR3(entityBoundBox.max.x,entityBoundBox.max.y,entityBoundBox.max.z);

	for(int it = 0; it != _entities.size(); it++)
	{
        if(_entities[it] == entityCollide || _entities[it]->isDead())
            continue;
	
		D3DXVECTOR3 otherEntityVel = _entities[it]->getVelocity();
		BoundingBox collideEntityBoundBox = _entities[it]->getBoundingBox();
		collideEntityBoundBox.max += otherEntityVel;
		collideEntityBoundBox.min += otherEntityVel;
		
		if(collideEntityBoundBox.containsPoint(leftBottomNear)  || collideEntityBoundBox.containsPoint(leftBottomFar) || 
		   collideEntityBoundBox.containsPoint(leftTopNear)     || collideEntityBoundBox.containsPoint(leftTopFar)    ||
		   collideEntityBoundBox.containsPoint(rightBottomNear) || collideEntityBoundBox.containsPoint(rightBottomFar)||
		   collideEntityBoundBox.containsPoint(rightTopNear)    || collideEntityBoundBox.containsPoint(rightTopFar))
		{
			// FIXME:  This don't work!
			// If this entity is standing right on top of another one, then 
			// we should mark it as 'on the floor'.
			if (std::abs(entityBoundBox.min.y - collideEntityBoundBox.max.y) <=
				util::EPSILON)
			{
				entityCollide->setOnTheFloor(true);
			}

			//TODO: work our proper velocity vectors
			_entities[it]->setVelocity(D3DXVECTOR3(0,0,0));
			entityCollide->setVelocity(D3DXVECTOR3(0,0,0));

			//deal their collision damages
			entityCollide->takeDamage(_entities[it]->getCollideDamage());
			_entities[it]->takeDamage(entityCollide->getCollideDamage());
		}
	}	
}

void Game::checkCollisionWithPowerUp()
{
	EntityPtr mainPlayer = player->getEntity();
	if(mainPlayer->isDead())
		return;

	D3DXVECTOR3 entityvel = mainPlayer->getVelocity();
	BoundingBox mainPlayerBoundBox = mainPlayer->getBoundingBox();
	mainPlayerBoundBox.max += entityvel;
	mainPlayerBoundBox.min += entityvel;	

	for(PowerUpList::iterator it = _powerUps.begin(); it != _powerUps.end(); it++)
	{
		BoundingBox powerUpBoundBox = (*it)->getBoundingBox();

        D3DXVECTOR3 leftBottomNear  = D3DXVECTOR3(powerUpBoundBox.min.x,powerUpBoundBox.min.y,powerUpBoundBox.min.z);
        D3DXVECTOR3 leftBottomFar   = D3DXVECTOR3(powerUpBoundBox.min.x,powerUpBoundBox.min.y,powerUpBoundBox.max.z);
        D3DXVECTOR3 leftTopNear     = D3DXVECTOR3(powerUpBoundBox.min.x,powerUpBoundBox.max.y,powerUpBoundBox.min.z);
        D3DXVECTOR3 leftTopFar      = D3DXVECTOR3(powerUpBoundBox.min.x,powerUpBoundBox.max.y,powerUpBoundBox.max.z);
        D3DXVECTOR3 rightBottomNear = D3DXVECTOR3(powerUpBoundBox.max.x,powerUpBoundBox.min.y,powerUpBoundBox.min.z);
        D3DXVECTOR3 rightBottomFar  = D3DXVECTOR3(powerUpBoundBox.max.x,powerUpBoundBox.min.y,powerUpBoundBox.max.z);
        D3DXVECTOR3 rightTopNear    = D3DXVECTOR3(powerUpBoundBox.max.x,powerUpBoundBox.max.y,powerUpBoundBox.min.z);
        D3DXVECTOR3 rightTopFar     = D3DXVECTOR3(powerUpBoundBox.max.x,powerUpBoundBox.max.y,powerUpBoundBox.max.z);
		
		if(mainPlayerBoundBox.containsPoint(leftBottomNear)  || mainPlayerBoundBox.containsPoint(leftBottomFar) || 
		   mainPlayerBoundBox.containsPoint(leftTopNear)     || mainPlayerBoundBox.containsPoint(leftTopFar)    ||
		   mainPlayerBoundBox.containsPoint(rightBottomNear) || mainPlayerBoundBox.containsPoint(rightBottomFar)||
		   mainPlayerBoundBox.containsPoint(rightTopNear)    || mainPlayerBoundBox.containsPoint(rightTopFar))
		{
			//if the played needed the powerup, then erase and move along
			(*it)->collidedWithPlayer();
		}
	}	
}

void Game::removeEntity(int idNum)
{
	for(EntityList::iterator it = _entities.begin(); it != _entities.end(); it++)
	{
		if((*it)->getIDNum() == idNum)
		{
            (*it)->killEntity();
			_entities.erase(it);
			return;
		}
	}
}

AIPlayerPtr Game::getAIPlayer(int idNum)
{
	for(PlayerList::iterator it = _aiPlayers.begin(); it != _aiPlayers.end(); it++)
		if((*it)->getIDNum() == idNum)
			return *it;
	return AIPlayerPtr();
}

void Game::removeEntityController(int idNum)
{
	//we loop the all the _aiPlayers in order to remove it's controler
	for(PlayerList::iterator it = _aiPlayers.begin(); it != _aiPlayers.end(); it++)
	{
		if((*it)->getEntity()->getIDNum() == idNum)
		{
			_aiPlayers.erase(it);
			return;
		}
	}
}

CVar<float> maxShakeDistance("maxShakeDistance", 2500.0f);

void Game::dealRadialDamage(const D3DXVECTOR3& pos, float damage)
{
	//if it deals no damage we don't want to loop through everything
	if(damage == 0)
		return;

	// Shake the screen if the explosion goes off close enough
	// to the player.
	float distToPlayer = D3DXVec3Length(&(pos - player->getEntity()->getPos()));
	if (distToPlayer < maxShakeDistance)
	{
		float shakeStrength = 0.1f * (1 - distToPlayer / maxShakeDistance);

		D3DXVECTOR3 direction = player->getEntity()->getPos() - pos;
		Ray lineOfSight(pos, direction);
		bool viewObstructed = kdTree.doesRayIntersect(lineOfSight);

		// Reduce the amount by which the camera shakes if the player is
		// not in the explosion's line of sight.
		if (viewObstructed)
			shakeStrength /= 10;

		eventManager->raiseEvent(EventPtr(new Ev_Shake_Screen(shakeStrength, 1.0f)));
	}

	//to deal the radial damage we have to go through all the _entities
	for(int i = 0; i != _entities.size(); i++)
	{
		//we assume that the entity dealing the damage is already dead,
		//so we can just move onto the next one, in addition,
		//we'd get divide by 0 error anyways. Also, if it's a projectile
		//we don't want it to die in the explosion
		if(_entities[i]->getPos() == pos)
			continue;

		D3DXVECTOR3 screwedEntityRadialDist = _entities[i]->getPos() - pos;
		float dist                          = D3DXVec3Length(&screwedEntityRadialDist);
		float damageToDeal                  = damage * 50000.0f/(dist*dist);

		//if the damage is too low we don't want to bother the entity
		if(damageToDeal > 5.0f && !(_entities[i]->isPlayerEntity() && dieAnotherDay))
			_entities[i]->takeDamage(damageToDeal);
	}
}
void Game::addToWeaponCache(WeaponInfo *newWeapon)
{
    WeaponList::iterator serachResults = _weaponsList.find(newWeapon->weaponName);
	if(serachResults != _weaponsList.end())
		delete (*serachResults).second;
	_weaponsList[newWeapon->weaponName] = newWeapon;
}
WeaponInfo* Game::getFromWeaponCache(std::string weaponName)
{
    WeaponList::iterator serachResults = _weaponsList.find(weaponName);

    //if we didn't find the weapon that's T_T, and we kill the game
    //otherwise return a copy of the weapon we found
	assert(serachResults != _weaponsList.end());
    return new WeaponInfo(*(*serachResults).second);
}
void Game::addToPowerUpCache(PowerUpInfo* newPowerUp)
{
    _powerUpCache[newPowerUp->name] = newPowerUp;
}

PowerUpInfo* Game::getFromPowerUpCache(std::string powerUpName)
{
    PowerUpCache::iterator serachResults = _powerUpCache.find(powerUpName);
	assert(serachResults != _powerUpCache.end());
    return (*serachResults).second;
}

void Game::updateRenderVars()
{
	if (!player || !player->getEntity())
		return;

	EntityPtr playerEntity = player->getEntity();
	_renderNumbers["health"]       = playerEntity->getHealth();
	_renderNumbers["isShooting"]   = playerEntity->isFiring();
	_renderNumbers["lastHitTime"]  = playerEntity->getLastHitTime();
	_renderNumbers["dashTimer"]    = playerEntity->getDashTimer();
	_renderNumbers["dashCooldown"] = playerEntity->getDashCooldown();
	_renderNumbers["isPlayerDead"] = playerEntity->isDead();

	_renderStrings["weapon"] = playerEntity->getWeapon()->weaponName;
	_renderStrings["talker"] = _whoIsTalking;
}

bool Game::areEntitiesFrozen() const
{
	return _freezeTimer > 0.0f;
}

void Game::freezeEntities(float timeSeconds)
{
	_freezeTimer = timeSeconds;
}

boost::optional<std::string> Game::getRenderString(const std::string &varName) const
{
	StringMap::const_iterator it = _renderStrings.find(varName);
	if (it != _renderStrings.end())
		return boost::optional<std::string>(it->second);
	else
		return boost::optional<std::string>();
}

boost::optional<float> Game::getRenderNumber(const std::string &varName) const
{
	NumberMap::const_iterator it = _renderNumbers.find(varName);
	if (it != _renderNumbers.end())
		return boost::optional<float>(it->second);
	else
		return boost::optional<float>();
}