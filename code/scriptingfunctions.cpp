//
// All functions that get exposed to script files get defined here.
// Include whatever headers you need to define them.
//
#include <d3dx9.h>

#include "scripting.hpp"
#include "log.hpp"
#include "framemanager.hpp"
#include "frame.hpp"
#include "widget.hpp"
#include "gameapp.hpp"
#include "process.hpp"
#include "processmanager.hpp"
#include "packet.hpp"
#include "common.hpp"
#include "game.hpp"
#include "camera.hpp"
#include "animatedentity.hpp"
#include "staticentity.hpp"
#include "game.hpp"
#include "sound.hpp"
#include "powerup.hpp"
#include "trigger.hpp"

#include "kdtree.hpp"
#include "speech.hpp"
#include "event.hpp"
#include "eventmanager.hpp"
#include <d3dx9.h>
#include <iostream>
#include <fstream>
#include <boost/optional.hpp>

// Scripting function prototypes
void registerCoreScriptingFunctions();
void registerKeyboardScriptingFunctions();
void registerEffectScriptingFunctions();
void registerUIScriptingFunctions();
void registerAIScriptingFunctions();

//////////////////////////////////////////////////////////////////////////
// Process functions.
//////////////////////////////////////////////////////////////////////////

SCRIPTING_FUNCTION(createProcess)
{
	PacketPtr packet = scripting->writeToPacket(scripting->getNumArgs());
	int processId    = processManager->createDisabledProcess(packet);

    scripting->pushArgument(processId);
	return 1;
}

SCRIPTING_FUNCTION(attachProcess)
{
	PacketPtr packet = scripting->writeToPacket(scripting->getNumArgs());
	int processId    = processManager->attachProcess(packet);

	scripting->pushArgument(processId);
	return 1;
}

SCRIPTING_FUNCTION(setNextProcess)
{
	int sourceId = scripting->getArgument<int>(1);
	int targetId = scripting->getArgument<int>(2);

	int result = processManager->connectProcesses(sourceId, targetId);
	scripting->pushArgument(result);

	return 1;
}

//////////////////////////////////////////////////////////////////////////

SCRIPTING_FUNCTION(addEntity)
{
	NewEntityStruct newEntityParams;

	std::string fileName = scripting->getArgument<std::string>(1);
	newEntityParams.position.x = scripting->getArgument<float>(2);
	newEntityParams.position.y = scripting->getArgument<float>(3);
	newEntityParams.position.z = scripting->getArgument<float>(4);

	scripting->executeFile(fileName);
	newEntityParams.name				 = scripting->getGlobal<std::string>("name");
	newEntityParams.mesh				 = scripting->getGlobal<std::string>("mesh");
	newEntityParams.deathSound			 = scripting->getGlobal<std::string>("deathSound");
    newEntityParams.hitSound			 = scripting->getGlobal<std::string>("hitSound");
	newEntityParams.footStepSound		 = scripting->getGlobal<std::string>("footStepSound");
    newEntityParams.coeffOfRestitution   = scripting->getGlobal<float>("coeffOfRestitution");
    newEntityParams.weight               = scripting->getGlobal<float>("weight");
	newEntityParams.boundBox.max.x		 = scripting->getGlobal<float>("boundBoxMaxX");
	newEntityParams.boundBox.max.y		 = scripting->getGlobal<float>("boundBoxMaxY");
	newEntityParams.boundBox.max.z		 = scripting->getGlobal<float>("boundBoxMaxZ");
	newEntityParams.boundBox.min.x		 = scripting->getGlobal<float>("boundBoxMinX");
	newEntityParams.boundBox.min.y		 = scripting->getGlobal<float>("boundBoxMinY");
	newEntityParams.boundBox.min.z       = scripting->getGlobal<float>("boundBoxMinZ");
    newEntityParams.speed			     = scripting->getGlobal<float>("speed");
	newEntityParams.hitPoints		     = scripting->getGlobal<float>("hitPoints");
	newEntityParams.collisionDamage      = scripting->getGlobal<float>("collisionDamage");
	newEntityParams.radius				 = scripting->getGlobal<float>("radius");
	newEntityParams.isStatic			 = scripting->getGlobal<int>("staticEntity");
	newEntityParams.diesAfterTime		 = scripting->getGlobal<bool>("diesAfterTime");
	newEntityParams.diesOnImpact		 = scripting->getGlobal<bool>("diesOnImpact");
	newEntityParams.dropsItemsAtDeath	 = scripting->getGlobal<bool>("dropsItemsAtDeath");
	newEntityParams.deathMode            = static_cast<Entity::death_mode>(scripting->getGlobal<int>("deathMode"));
	newEntityParams.onHitMode            = static_cast<Entity::on_hit_mode>(scripting->getGlobal<int>("onHitMode"));
	newEntityParams.gravitySensitive	 = true;
	newEntityParams.yawAngle			 = 0;
	newEntityParams.pitchAngle			 = 0;
	newEntityParams.radialDamage		 = 0;
    newEntityParams.isProjectile         = false;

	if(newEntityParams.dropsItemsAtDeath)
	{
		int numFields = scripting->setActiveTable("dropTable");
		ASSERTMSG(numFields == 6, "We must have 6 values in the drop table");

		for(int x = 0; x < 6; x++)
			newEntityParams.dropTable[x] = scripting->getField<float>(x+1);
	}
	
	if(newEntityParams.isStatic)
	{
		StaticEntityPtr newEntity = StaticEntityPtr(new StaticEntity(&newEntityParams));
		game->addEntity(newEntity);

		scripting->pushArgument(newEntity->getIDNum());
	}
	else
	{
        bool isHuman = scripting->getGlobal<int>("isHuman");
	    bool isAI    = scripting->getGlobal<int>("isAI");

		newEntityParams.cameraOffsetY = scripting->getGlobal<float>("cameraOffsetY");
		newEntityParams.cameraOffsetZ = scripting->getGlobal<float>("cameraOffsetZ");

		//
		// Extract animation sequence information if we haven't seen 
		// this entity type before.
		//
		if (!AnimatedEntity::animationsExist(newEntityParams.name))
		{
			// We want exactly four animation sets:
			//   Idle
			//   Run/shoot
			//   Reload
			//   Death
			// each of which has a [start, end, speed] triple.
			int numFields = scripting->setActiveTable("animations");
			ASSERTMSG(
				numFields == 12, 
				"Need four animation sets in [start, end, speed] triples!");

			AnimatedEntity::AnimationSequence animationSequence;
			animationSequence.reserve(numFields / 3);
			for (int i = 1; i <= numFields; i += 3)
			{
				float currStartTime = scripting->getField<float>(i);
				float currEndTime   = scripting->getField<float>(i + 1);
				float currSpeed     = scripting->getField<float>(i + 2);

				animationSequence.push_back(AnimatedEntity::AnimationInterval(
												currStartTime, 
												currEndTime, 
												currSpeed));
			}

			// Add this animation sequence to our table of known animations.
			AnimatedEntity::addAnimationSequence(newEntityParams.name, animationSequence);
		}

		AnimatedEntityPtr newEntity = AnimatedEntityPtr(new AnimatedEntity(&newEntityParams));

        int totWeapons = 1;

        if(isHuman)
        {
            newEntity->addGrenade(game->getFromWeaponCache("Grenade"));
	        totWeapons = 0;//4;
        }

        for(int x = 0; x < totWeapons; x++)
            newEntity->addWeapon(game->getFromWeaponCache(scripting->getGlobal<std::string>("weapon" + util::toString(x + 1))));

		game->addEntity(newEntity);

		if (isHuman)
			game->player->setEntity(newEntity);

		else if (isAI)
			game->addAIPlayer(newEntity);

		scripting->pushArgument(newEntity->getIDNum());
	}

	return 1;
}

SCRIPTING_FUNCTION(editAddEntity)
{
	std::string filename = scripting->getArgument<std::string>(1);

	// Figure out where the camera is and where it's facing...
	const D3DXVECTOR3 &cameraPosition = camera->getPosition();
	const D3DXVECTOR3 &cameraHeading  = camera->getLookVector();
	
	// ... and add the entity a few units along the direction 
	// that the camera is facing.
	D3DXVECTOR3 entityPosition = cameraPosition + (cameraHeading * 100);

	// Call addEntity with the relevant parameters.
	std::string execString = "addEntity(\"" + filename + "\", " + 
	                          util::toString(entityPosition.x) + ", " + 
	                          util::toString(entityPosition.y) + ", " + 
	                          util::toString(entityPosition.z) + ")";
	scripting->executeString(execString);

	std::fstream myfile(game->getCurrentLevelFile().c_str());
	myfile.seekp(0,std::ios_base::end);
	std::string stringToWrite = "\n" + execString;
	myfile<<stringToWrite.c_str();
	myfile.close();
	return 0;
}
//////////////////////////////////////////////////////////////////////////
SCRIPTING_FUNCTION(playMusicOnce)
{
	eventManager->raiseEvent(EventPtr(new Ev_Play_Music(scripting->getArgument<std::string>(1), true)));
	return 0;
}

//////////////////////////////////////////////////////////////////////////
SCRIPTING_FUNCTION(playMusic)
{
	eventManager->raiseEvent(EventPtr(new Ev_Play_Music(scripting->getArgument<std::string>(1), false)));
	return 0;
}

//////////////////////////////////////////////////////////////////////////
SCRIPTING_FUNCTION(kill)
{
    game->reloadCurrentLevel();
    return 0;
}

//////////////////////////////////////////////////////////////////////////
SCRIPTING_FUNCTION(addWeaponToCache)
{
    //get weapon info
    WeaponInfo * weapon = new WeaponInfo;

    weapon->projectileSpeed		         = scripting->getArgument<int>(14);         //("projectileSpeed");
    weapon->projectileLife			     = scripting->getArgument<float>(15);         //("projectileLife");
    weapon->weaponCoolDown		     	 = scripting->getArgument<float>(4);        //("weaponCoolDown");
    weapon->weaponClipSize		         = scripting->getArgument<int>(6);          //("weaponClipSize");
    weapon->weaponReloadDelay     	     = scripting->getArgument<float>(5);        //("weaponReloadDelay");
    weapon->projectileDamage	  	     = scripting->getArgument<float>(16);       //("projectileDamage");
    weapon->weaponCurrAmmo	             = scripting->getArgument<int>(8);          //("weaponStartAmmo");
    weapon->weaponMaxAmmo                = scripting->getArgument<int>(7);          //("weaponMaxAmmo");
    weapon->projectileWeight             = scripting->getArgument<float>(18);       //("projectileWeight");
    weapon->projectileCoeffOfRestitution = scripting->getArgument<float>(17);       //("projectileCoeffOfRestitution");
    weapon->weaponChangeDelay            = scripting->getArgument<float>(9);        //("weaponChangeDelay");
    weapon->minInaccuracy                = scripting->getArgument<float>(11);       //("weaponMinInaccuracy");
    weapon->maxInaccuracy                = scripting->getArgument<float>(12);       //("weaponMaxInaccuracy");
    weapon->inaccuracy                   = scripting->getArgument<float>(11);       //("weaponMinInaccuracy");
    weapon->weaponName                   = scripting->getArgument<std::string>(1);  //("weaponName");
    weapon->reloadSound           	     = scripting->getArgument<std::string>(2);  //("reloadSound");
    weapon->projectileMesh               = scripting->getArgument<std::string>(13); //("projectileMesh");
    weapon->projectileLaunchSound	     = scripting->getArgument<std::string>(23); //("projectileLaunchSound");
    weapon->projectileHitSound		     = scripting->getArgument<std::string>(22); //("projectileHitSound");
    weapon->projectileIntermedSound      = scripting->getArgument<std::string>(24); //("projectileIntermedSound");
    weapon->weaponEquipSound             = scripting->getArgument<std::string>(10); //("weaponEquipSound");
    weapon->weaponEmptySound             = scripting->getArgument<std::string>(3);  //("weaponEmptySound");
    weapon->projectileDiesAfterTime		 = scripting->getArgument<bool>(20);        //("projectileDiesAfterTime");
    weapon->projectileDiesOnImpact		 = scripting->getArgument<bool>(19);        //("projectileDiesOnImpact");
    weapon->projectileGravitySensitive	 = scripting->getArgument<bool>(21);        //("projectileGravitySensitive");
    weapon->projectileRadialDamage		 = scripting->getArgument<float>(25);       //("projectileRadialDamage");
	int numOffsets						 = scripting->getArgument<float>(26);
    for(int x = 0; x < numOffsets; x++)
    {
        weapon->bulletOffset.push_back(D3DXVECTOR3(0,0,0));
	    weapon->bulletOffset[x].x = scripting->getArgument<int>(27 + x*3);         //("bulletXOffset");
        weapon->bulletOffset[x].y = scripting->getArgument<int>(28 + x*3);         //("bulletYOffset");
        weapon->bulletOffset[x].z = scripting->getArgument<int>(29 + x*3);         //("bulletZOffset");
    }
    weapon->numProjectiles = scripting->getArgument<int>(27 + numOffsets*3);

    weapon->bulletBoundBox.max.x = 0;
    weapon->bulletBoundBox.max.y = 0;
    weapon->bulletBoundBox.max.z = 0;
    weapon->bulletBoundBox.min.x = 0;
    weapon->bulletBoundBox.min.y = 0;
    weapon->bulletBoundBox.min.z = 0;

    //sets up the amount of ammo in the clip, and weapon overall
    if(weapon->weaponCurrAmmo >= weapon->weaponClipSize)
        weapon->weaponAmmoInClip = weapon->weaponClipSize;
    else
        weapon->weaponAmmoInClip = weapon->weaponCurrAmmo;

    weapon->weaponCurrAmmo -= weapon->weaponAmmoInClip;

    //load the sounds into the sound cache
    soundCache.get(weapon->projectileLaunchSound);
    soundCache.get(weapon->projectileHitSound); 
    soundCache.get(weapon->projectileIntermedSound);
    soundCache.get(weapon->reloadSound);

    //load the mesh into the mesh cache
    staticMeshCache.get(weapon->projectileMesh);

    assert(weapon->projectileLife  >= 0);
    assert(weapon->weaponClipSize  >= 0);
    assert(weapon->weaponCurrAmmo  >= 0);
    assert(weapon->weaponMaxAmmo   >= 0);

    game->addToWeaponCache(weapon);
    return 0;
}
//////////////////////////////////////////////////////////////////////////
SCRIPTING_FUNCTION(tableTest)
{
	int numFields = scripting->setActiveTable("weapons");
	std::string r = scripting->getField<std::string>(1);
	std::string g = scripting->getField<std::string>(2);
	std::string b = scripting->getField<std::string>(3);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
SCRIPTING_FUNCTION(addToPowerUpCache)
{
	PowerUpInfo* newPowerUp = new PowerUpInfo();

	newPowerUp->name					= scripting->getArgument<std::string>(1);
	newPowerUp->mesh					= scripting->getArgument<std::string>(2);
	newPowerUp->boundBox.min.x			= scripting->getArgument<float>(3);
	newPowerUp->boundBox.min.y			= scripting->getArgument<float>(4);
	newPowerUp->boundBox.min.z			= scripting->getArgument<float>(5);
	newPowerUp->boundBox.max.x			= scripting->getArgument<float>(6);
	newPowerUp->boundBox.max.y			= scripting->getArgument<float>(7);
	newPowerUp->boundBox.max.z			= scripting->getArgument<float>(8);
	newPowerUp->soundToPlay				= scripting->getArgument<std::string>(9);
	newPowerUp->radius					= scripting->getArgument<float>(10);
	newPowerUp->weaponName				= scripting->getArgument<std::string>(11);

	soundCache.get(newPowerUp->soundToPlay);
	game->addToPowerUpCache(newPowerUp);

	return 0;
}

//////////////////////////////////////////////////////////////////////////
SCRIPTING_FUNCTION(addTrigger)
{
	D3DXVECTOR2 minXZ;
	D3DXVECTOR2 maxXZ;
	float delay;
	std::string entityName;

	int type = scripting->getArgument<int>(1);
	std::string triggerCommand = scripting->getArgument<std::string>(2);

	switch (type)
	{
		case 0:
			minXZ.x = scripting->getArgument<float>(3);
			minXZ.y = scripting->getArgument<float>(4);
			maxXZ.x = scripting->getArgument<float>(5);
			maxXZ.y = scripting->getArgument<float>(6);

			// Make sure that the max point really is the max!
			if (maxXZ.x < minXZ.x) 
				std::swap(maxXZ.x, minXZ.x);
			if (maxXZ.y < minXZ.y) 
				std::swap(maxXZ.y, minXZ.y);

			game->addToTriggerList(PlayerLocationTriggerPtr(new PlayerLocationTrigger(triggerCommand, minXZ, maxXZ)));
			break;

		case 1:
			delay = scripting->getArgument<float>(3); 
			game->addToTriggerList(TimeTriggerPtr(new TimeTrigger(triggerCommand, delay)));
			break;

		case 2:
			entityName = scripting->getArgument<std::string>(3); 
			game->addToTriggerList(EntityDeathTriggerPtr(new EntityDeathTrigger(triggerCommand, entityName)));
			break;
	
		default:
			assert(false);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
SCRIPTING_FUNCTION(addPowerUp)
{
	D3DXVECTOR3 pos;
	std::string powerUp     = scripting->getArgument<std::string>(1);
	pos.x                   = scripting->getArgument<float>(2);
	pos.y                   = scripting->getArgument<float>(3);
	pos.z                   = scripting->getArgument<float>(4);
	PowerUpInfo* newPowerUp = game->getFromPowerUpCache(powerUp);

	if(powerUp == "Health")
		game->addToPowerUpVector(HealthPowerUpPtr(new HealthPowerUp(pos, newPowerUp)));

	else if(powerUp == "GrenadeAmmo")
		game->addToPowerUpVector(GrenadePowerUpPtr(new GrenadePowerUp(pos, newPowerUp)));

	else
		game->addToPowerUpVector(WeaponPowerUpPtr(new WeaponPowerUp(pos, newPowerUp)));

	return 0;
}
//////////////////////////////////////////////////////////////////////////
SCRIPTING_FUNCTION(editAddPowerUp)
{
	std::string powerUpName = scripting->getArgument<std::string>(1);

	// Figure out where the camera is and where it's facing...
	const D3DXVECTOR3 &cameraPosition = camera->getPosition();
	const D3DXVECTOR3 &cameraHeading  = camera->getLookVector();
	
	// ... and add the entity a few units along the direction 
	// that the camera is facing.
	D3DXVECTOR3 entityPosition = cameraPosition + (cameraHeading * 100);

	// Call addEntity with the relevant parameters.
	std::string execString = "addPowerUp(\"" + powerUpName + "\", " + 
	                          util::toString(entityPosition.x) + ", " + 
	                          util::toString(entityPosition.y) + ", " + 
	                          util::toString(entityPosition.z) + ")";
	scripting->executeString(execString);

	std::fstream myfile(game->getCurrentLevelFile().c_str());
	myfile.seekp(0,std::ios_base::end);
	std::string stringToWrite = "\n" + execString;
	myfile<<stringToWrite.c_str();
	myfile.close();
	return 0;
}
//////////////////////////////////////////////////////////////////////////

SCRIPTING_FUNCTION(playSound)
{
	eventManager->raiseEvent(EventPtr(new Ev_Play_Sound(
		scripting->getArgument<std::string>(1), 
		boost::optional<D3DXVECTOR3>(), true)));
	return 0;
}

SCRIPTING_FUNCTION(playSpeech)
{
	std::string soundName = scripting->getArgument<std::string>(1);
	currSpeechSound = soundCache.get(soundName);
	MYASSERT(currSpeechSound);
	currSpeechSound->play();
	return 0;
}

SCRIPTING_FUNCTION(freezeEntities)
{
	float freezeTimeSeconds = scripting->getArgument<float>(1);
	if (game->isGameInProgress())
		game->freezeEntities(freezeTimeSeconds);
	return 0;
}
//////////////////////////////////////////////////////////////////////////

SCRIPTING_FUNCTION(startSpeechSequence)
{
	int numArgs = scripting->getNumArgs();
	ASSERTMSG(numArgs % 5 == 0, "The number of arguments in a speech sequence \
							  must be a multiple of 5!\
							  (talker, text, duration, relX, relY)");

	SpeechSequence newSequence;
	newSequence.reserve(numArgs / 5);
	for (int i = 1; i <= numArgs; i += 5)
	{
		std::string talker = scripting->getArgument<std::string>(i);
		std::string text   = scripting->getArgument<std::string>(i + 1);
		float duration   = scripting->getArgument<float>(i + 2);
		float relX       = scripting->getArgument<float>(i + 3);
		float relY       = scripting->getArgument<float>(i + 4);
		newSequence.push_back(Speech(talker, text, duration, relX, relY));
	}

	if (!newSequence.empty())
		game->setTalker(newSequence[0].talker);

	startSpeechSequence(newSequence);
	return 0;
}

SCRIPTING_FUNCTION(shakeScreen)
{
	float shakeStrength = scripting->getArgument<float>(1); // 0.1f is a decent shake
	float shakeTime     = scripting->getArgument<float>(2);
	eventManager->raiseEvent(EventPtr(new Ev_Shake_Screen(shakeStrength, shakeTime)));
	return 0;
}

SCRIPTING_FUNCTION(setOnSpeechFinished)
{
	std::string text = scripting->getArgument<std::string>(1);
	onSpeechFinished = text;
	return 0;
}

//////////////////////////////////////////////////////////////////////////

void registerScriptingFunctions()
{
	registerCoreScriptingFunctions();
	registerKeyboardScriptingFunctions();
	registerUIScriptingFunctions();
	registerEffectScriptingFunctions();
	registerAIScriptingFunctions();

	// Process functions.
    REGISTER_SCRIPTING_FUNCTION(createProcess);
    REGISTER_SCRIPTING_FUNCTION(attachProcess);
    REGISTER_SCRIPTING_FUNCTION(setNextProcess);

	//adding an entity to the game
	REGISTER_SCRIPTING_FUNCTION(editAddEntity);
    REGISTER_SCRIPTING_FUNCTION(addEntity);
	REGISTER_SCRIPTING_FUNCTION(playMusic);
	REGISTER_SCRIPTING_FUNCTION(playMusicOnce);
	REGISTER_SCRIPTING_FUNCTION(playSound);
    REGISTER_SCRIPTING_FUNCTION(addWeaponToCache);
	REGISTER_SCRIPTING_FUNCTION(addToPowerUpCache);
	REGISTER_SCRIPTING_FUNCTION(addPowerUp);
	REGISTER_SCRIPTING_FUNCTION(editAddPowerUp);

	REGISTER_SCRIPTING_FUNCTION(addTrigger);

    REGISTER_SCRIPTING_FUNCTION(kill);
	REGISTER_SCRIPTING_FUNCTION(playSpeech);
	REGISTER_SCRIPTING_FUNCTION(freezeEntities);

	REGISTER_SCRIPTING_FUNCTION(tableTest);
	REGISTER_SCRIPTING_FUNCTION(startSpeechSequence);
	REGISTER_SCRIPTING_FUNCTION(shakeScreen);
	REGISTER_SCRIPTING_FUNCTION(setOnSpeechFinished);
}
