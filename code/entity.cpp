#include <boost/optional.hpp>

#include "entity.hpp"
#include "timer.hpp"
#include "graphics.hpp"
#include "util.hpp"
#include "event.hpp"
#include "eventmanager.hpp"
#include "sound.hpp"
#include "game.hpp"
#include "cvar.hpp"
#include "staticmesh.hpp"
#include "skinnedmesh.hpp"
#include "renderer.hpp"
#include "scripting.hpp"
#include "powerup.hpp"

int Entity::nextId = 0;

CVar<float> gravity("gravity", 1200.0f);
CVar<float> maxFallSpeed("maxFallSpeed", 20.0f);
Entity::Entity()
{
    _name               = ""; 
    _health             = 0;
    _maxHealth          = 0;
    _lastHitTime        = 0;
    _weaponNum          = 0;
    _idNum              = nextId++;
    _isStatic           = false;
    _isDead             = false;
    _isOnFloor          = false;
    _diesOnImpact       = false;
    _diesAfterTime      = false;
    _gravitySensitive   = false;
    _isProjectile       = false;
    _isFiring           = false;
    _isJumping          = false;
    _dropsItemsAtDeath  = false;
	_deathMode          = death_disappear;
	_onHitMode          = on_hit_nothing;
    _collisionDamage    = 0;
    _yawAngle           = 0;
    _pitchAngle         = 0;
    _maxSpeed           = 0;
    _speed              = 0;
    _dashTimer          = 0;
    _canDashTimer       = 0;
    _weight             = 0;
    _coeffOfRestitution = 0;
    _oldY               = 0;
    _radialDamage       = 0;
    _radius             = 0;
    _grenade            = NULL;
	_timeOfDeath        = 0;
	_walkSoundDelay     = 0;

    //sound information
    _hitSound       = "";
    _deathSound     = "";
    _footStepSound  = "";

    //counters for the current weapon
    _weaponCoolDownCounter = 0;
    _weaponReloadDelayCounter = 0;
    _weaponChangeDelayCounter = 0;
	_weaponChangeDelayCounterMin = 0;
	_reloadSoundDelay = 0;
}

Entity::Entity(NewEntityStruct* newEntityParams)
{
	//pull out all the values to form the new entity
	_name					= newEntityParams->name; 
	_radius					= newEntityParams->radius;
	_hitSound				= newEntityParams->hitSound;
	_footStepSound			= newEntityParams->footStepSound;
	_deathSound				= newEntityParams->deathSound;
	_maxSpeed			    = newEntityParams->speed;
	_collisionDamage		= newEntityParams->collisionDamage;
	_health					= newEntityParams->hitPoints;
	_maxHealth				= newEntityParams->hitPoints;
	_pos					= newEntityParams->position;
	_oldPos                 = _pos;
	_yawAngle				= newEntityParams->yawAngle;
	_pitchAngle				= newEntityParams->pitchAngle;
	_originBoundBox			= newEntityParams->boundBox;
	_weapons				= newEntityParams->weapons;
	_isStatic				= newEntityParams->isStatic;
    _weight                 = newEntityParams->weight;
    _coeffOfRestitution     = newEntityParams->coeffOfRestitution;
	_diesOnImpact			= newEntityParams->diesOnImpact;
	_diesAfterTime			= newEntityParams->diesAfterTime;
	_gravitySensitive       = newEntityParams->gravitySensitive;
	_radialDamage			= newEntityParams->radialDamage;
    _isProjectile           = newEntityParams->isProjectile;
	_dropsItemsAtDeath		= newEntityParams->dropsItemsAtDeath;
	_deathMode              = newEntityParams->deathMode;
	_onHitMode              = newEntityParams->onHitMode;
	for(int x = 0; x < 6; x++)
		_dropTable[x] = newEntityParams->dropTable[x];

	//set initial variables for values not in script file
	_velocity				= D3DXVECTOR3(0,0,0);
	_idNum					= nextId++;
	_isDead					= false;
	_weaponNum				= 0;
    _grenade                = NULL;
    _speed                  = _maxSpeed;
    _dashTimer              = 0;
    _canDashTimer           = 0;
//	_lastHitTime            = -util::INFINITY;
	// You were never hit (hit an infinite amount of time ago).  You newbie.
	_lastHitTime            = util::INFINITY;
    _isJumping              = false;
	_walkSoundDelay         = 0;

	//this just insures that the sounds are in the sound cache
	soundCache.get(_hitSound);
	soundCache.get(_footStepSound);
	soundCache.get(_deathSound);
	
	//set the weapon variables
	_weaponCoolDownCounter    = 0;
	_weaponReloadDelayCounter = 0;
	_weaponChangeDelayCounter = 0;
	_weaponChangeDelayCounterMin = 0;
	_reloadSoundDelay = 0;

	updateBoundingBox();

	if(_diesAfterTime)
	{
		D3DXMATRIX pitchMatrix, yawMatrix;
		D3DXMatrixRotationX(&pitchMatrix, _pitchAngle + (D3DX_PI / 2));
		D3DXMatrixRotationY(&yawMatrix,   _yawAngle);

		_tempVel = D3DXVECTOR3(0, 1, 0);

		D3DXVec3TransformCoord(&_tempVel, &_tempVel, &pitchMatrix);
		D3DXVec3TransformCoord(&_tempVel, &_tempVel, &yawMatrix);

		D3DXVec3Normalize(&_tempVel, &_tempVel);

		_tempVel.x *= -1;
		_tempVel.z *= -1;

		_velocity = _tempVel * _speed * getDtSeconds();
		_oldY = _velocity.y;
	}
}
Entity::~Entity()
{
    for(int i = 0; i != _weapons.size(); i++)
    {
        delete _weapons[i];
    }
    delete _grenade;
}

int Entity::getNumBullets()	const
{
	assert(!_weapons.empty()); 
	return _weapons[_weaponNum]->weaponAmmoInClip;
}

int Entity::getTotalBullets() const
{
	assert(!_weapons.empty()); 
	return _weapons[_weaponNum]->weaponCurrAmmo;
}

int Entity::getNumGrenades()const
{
	assert(_grenade); 
	return _grenade->weaponAmmoInClip;
}

void Entity::getWorldMatrix(D3DXMATRIX &outWorld) const
{
	if (_isProjectile)
	{
		// Projectiles need to store their pitch rotations too.
		D3DXMATRIX yawMatrix;
		D3DXMatrixRotationY(&yawMatrix, _yawAngle);

		D3DXMATRIX pitchMatrix;
		D3DXMatrixRotationX(&pitchMatrix, -_pitchAngle);

		D3DXMATRIX transWorld;
		D3DXMatrixTranslation(&transWorld, _pos.x, _pos.y, _pos.z);

		D3DXMatrixIdentity(&outWorld);
		D3DXMatrixMultiply(&outWorld, &outWorld, &pitchMatrix);
		D3DXMatrixMultiply(&outWorld, &outWorld, &yawMatrix);
		D3DXMatrixMultiply(&outWorld, &outWorld, &transWorld);
	}
	else
	{
		// Other entities do NOT store their pitch rotations, because
		// it's not their entire bounding boxes that move.  Presumably, 
		// an entity would only look up/down by bending at its torso.
		D3DXMATRIX yawMatrix;
		D3DXMatrixRotationY(&yawMatrix, _yawAngle);
		
		D3DXMATRIX transWorld;
		D3DXMatrixTranslation(&transWorld, _pos.x, _pos.y, _pos.z);

		D3DXMatrixIdentity(&outWorld);
		D3DXMatrixMultiply(&outWorld, &outWorld, &yawMatrix);
		D3DXMatrixMultiply(&outWorld, &outWorld, &transWorld);
	}
}
void Entity::updateBoundingBox()
{
	_boundBox.max = _originBoundBox.max + _pos;
	_boundBox.min = _originBoundBox.min + _pos;
}

CVar<float> jumpBoost("jumpBoost", 1.0f);

void Entity::normalizeVelocity()
{
	float tempY = _velocity.y;
	// If it's a projectile make it go up or down, depending on the pitch angle.
	if(_isProjectile)
	{
		_velocity = _tempVel * _speed * getDtSeconds();
		_velocity.y += (tempY - _oldY);
		_oldY = _tempVel.y * _speed * getDtSeconds();
	}

	else
	{
		//save the y vel, and normalize the x and z directions
		_velocity.y = 0.0f;
		D3DXVec3Normalize(&_velocity, &_velocity);
		D3DXMATRIX rotMatrix;
		D3DXMatrixIdentity(&rotMatrix);
		D3DXMatrixRotationY(&rotMatrix, _yawAngle);
		D3DXVec3TransformCoord(&_velocity, &_velocity, &rotMatrix);

		_velocity *= _speed * getDtSeconds();
		_velocity.y = tempY;
	}

	if(_gravitySensitive)
	{
		//add in gravity
		float dt = getDtSeconds();
		float oldDt = getOldDtSeconds();

		if (oldDt != 0)
		{
			// Time corrected verlet integration ftw!
			_velocity.y = (((_pos.y - _oldPos.y) * (dt / oldDt)) + 
			              (-gravity * dt * dt));
		}
	}

	if(_isJumping)
	{
		// Scale the amount that you can jump by the dt value.
		_velocity.y += jumpBoost * getDtMilliseconds();
		_isJumping = false;
	}
}

void Entity::timepass()
{
    //lower all the cooldown counters
	if(_weaponCoolDownCounter > 0)
		_weaponCoolDownCounter -= getDtSeconds();

	if(_weaponReloadDelayCounter > 0)
		_weaponReloadDelayCounter -= getDtSeconds();

	if(_weaponChangeDelayCounter > 0)
		_weaponChangeDelayCounter -= getDtSeconds();

	if(_weaponChangeDelayCounterMin > 0)
		_weaponChangeDelayCounterMin -= getDtSeconds();

	if(_reloadSoundDelay > 0)
		_reloadSoundDelay -= getDtSeconds();

	if(_walkSoundDelay > 0)
		_walkSoundDelay -= getDtSeconds();

    if(_dashTimer > 0)
        _dashTimer -= getDtSeconds();
    else
        _speed = _maxSpeed;

    if(_canDashTimer > 0)
        _canDashTimer -= getDtSeconds();

	//if it's a projectile we need to decrement it's health
	if(_diesAfterTime)
		_health -= getDtSeconds();

	//we move the entity and store its old position.
	_oldPos = _pos;
	_pos += _velocity;

	if((std::abs(_velocity.x) > 5.0f || std::abs(_velocity.z) > 5.0f) && _walkSoundDelay <= 0 && _isOnFloor)
	{
		eventManager->raiseEvent(EventPtr(new Ev_Play_Sound(_footStepSound, 
		  boost::optional<D3DXVECTOR3>(_pos), 
			isPlayerEntity())));

		_walkSoundDelay = 0.5f;
	}

	if(isPlayerEntity() && _pos.y < -10000.0f)
		game->reloadCurrentLevel();

	_velocity = D3DXVECTOR3(0, _velocity.y ,0);

	// you've advanced without being hit!
	_lastHitTime += getDtSeconds();

	//we update the boxes for collision detection
	updateBoundingBox();

	
}
void Entity::killEntity()
{
    //we play it's death sound, and schedule it for deletion
	_timeOfDeath = getTimeSeconds();

	eventManager->raiseEvent(EventPtr(new Ev_Entity_Die(_deathSound, this, _name, _pos, getDeathMode())));
	game->dealRadialDamage(_pos, _radialDamage);
	if(_dropsItemsAtDeath)
	{
        float randomVar = util::randFloat(0, 100.0f);
		float lowerBound = 0;
		float upperBound = _dropTable[0];
		int powerUpNum;
		for(powerUpNum = 0; powerUpNum < 6; powerUpNum++)
		{
			if(randomVar >= lowerBound && randomVar <= upperBound)
				break;

			else if(powerUpNum == 5)
			{
				powerUpNum = 6;
				break;
			}

			lowerBound = upperBound;
			upperBound += _dropTable[powerUpNum+1];
		}

		switch (powerUpNum)
		{
			case 0:
				game->addToPowerUpVector(HealthPowerUpPtr(new HealthPowerUp(_pos, 
									game->getFromPowerUpCache("Health"))));
				break;
			
			case 1:
				game->addToPowerUpVector(WeaponPowerUpPtr(new WeaponPowerUp(_pos, 
									game->getFromPowerUpCache("BattleRifleAmmo"))));
				break;

			case 2:
				game->addToPowerUpVector(WeaponPowerUpPtr(new WeaponPowerUp(_pos, 
									game->getFromPowerUpCache("RocketLauncherAmmo"))));
				break;

			case 3:
				game->addToPowerUpVector(WeaponPowerUpPtr(new WeaponPowerUp(_pos, 
									game->getFromPowerUpCache("ShotGunAmmo"))));
				break;

			case 4:
				game->addToPowerUpVector(GrenadePowerUpPtr(new GrenadePowerUp(_pos, 
									game->getFromPowerUpCache("GrenadeAmmo"))));
				break;

			case 5:
				game->addToPowerUpVector(WeaponPowerUpPtr(new WeaponPowerUp(_pos, 
									game->getFromPowerUpCache("FlamethrowerAmmo"))));
				break;

			default:
				break;
		}
	}
	_isDead = true;
}
void Entity::takeDamage(float damage)
{
    //we should never send it negative damage
    assert(damage >= 0);

    //if a projectile takes damage, it should die!!!! 
    //The sound is played when it's removed, it plays_deathSound
    if(_diesOnImpact)
        _health = 0;

    //it's not a projectile, so play it's hit sound, and decrease the damage appropriately
    else if(damage != 0)
    {
        eventManager->raiseEvent(EventPtr(new Ev_Play_Sound(
			_hitSound, 
			boost::optional<D3DXVECTOR3>(_pos), 
			isPlayerEntity())));

        _health -= damage;
		//_lastHitTime = getTimeSeconds();
		// This is time since you were hit
		_lastHitTime = 0;
        //if the health is less than 0, make it 0, otherwise the Hud looks messed up
        if(_health < 0)
            _health = 0;
    }
}

void Entity::yaw(float angle)
{
    _yawAngle += angle;
}

void Entity::pitch(float angle)
{
    _pitchAngle += angle;
	_pitchAngle = util::clamp(
		_pitchAngle, 
		-(D3DX_PI / 2) + util::EPSILON, 
		(D3DX_PI / 2) - util::EPSILON);
}
void Entity::setVelocity(const D3DXVECTOR3 &vel)
{
	float force;
	if(getDtSeconds() != 0)
		force = std::abs((_velocity.y - vel.y))/getDtSeconds() * _weight;
	else
		force = 0;

	_velocity = vel;
	//FIXME: damage is not consistantly dealt
	/*force *= 0.02;
	if(force > 5.0f)
		takeDamage(force);*/
}
bool Entity::increaseHealth(float incHealth)
{
	//you shouldn't tell it to increase it's health with negative!!!!!
	assert(incHealth >= 0);

	if(_health == _maxHealth)
		return false;

	_health += incHealth;
	if(_health >= _maxHealth)
		_health = _maxHealth;

	return true;
}
bool Entity::addWeapon(WeaponInfo *newWeapon)
{
	//first we check to see if we have the weapon
	for(int i = 0; i != _weapons.size(); i++)
	{
		//if we have a match add the bullets, and make sure that you didn't add too much
		if(_weapons[i]->weaponName == newWeapon->weaponName)
		{
            if(_weapons[i]->weaponCurrAmmo == _weapons[i]->weaponMaxAmmo)
                return false;
			_weapons[i]->weaponCurrAmmo += newWeapon->weaponCurrAmmo + newWeapon->weaponAmmoInClip;
			if(_weapons[i]->weaponCurrAmmo > _weapons[i]->weaponMaxAmmo)
				_weapons[i]->weaponCurrAmmo = _weapons[i]->weaponMaxAmmo;
			delete newWeapon;
			return true;
		}
	}

	//if we don't have the weapon then just give it to the player
	_weapons.push_back(newWeapon);
    return true;
}

bool Entity::addGrenade(WeaponInfo* grenade)
{
	if(_grenade == NULL)
	{
		_grenade = grenade;
		return true;
	}
    if(_grenade->weaponAmmoInClip == _grenade->weaponMaxAmmo)
        return false;

	_grenade->weaponAmmoInClip += grenade->weaponAmmoInClip;
	if(_grenade->weaponAmmoInClip > _grenade->weaponMaxAmmo)
		_grenade->weaponAmmoInClip = _grenade->weaponMaxAmmo;

    return true;
}

Entity::death_mode Entity::getDeathMode() const
{
	return _deathMode;
}

Entity::on_hit_mode Entity::getOnHitMode() const
{
	return _onHitMode;
}

bool Entity::isPlayerEntity() const
{
	return game->player->getEntity().get() == this;
}
void Entity::setNumGrenades(int numGrenades)
{
	assert(_grenade);
	_grenade->weaponAmmoInClip = numGrenades;
}