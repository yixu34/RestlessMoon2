#include <d3dx9.h>
#include <algorithm>

#include "animatedentity.hpp"
#include "staticentity.hpp"
#include "renderer.hpp"
#include "boundingvolume.hpp"
#include "eventmanager.hpp"
#include "event.hpp"
#include "game.hpp"
#include "common.hpp"
#include "log.hpp"

AnimatedEntity::AnimationPool AnimatedEntity::animations;

AnimatedEntity::AnimatedEntity(NewEntityStruct* newEntityParams) : Entity(newEntityParams)
{
	_mesh = skinnedMeshCache.get(newEntityParams->mesh); 
	assert(_mesh);
	renderer->addRenderable(_mesh, _name, this);

	_cameraOffsetY = newEntityParams->cameraOffsetY;
	_cameraOffsetZ = newEntityParams->cameraOffsetZ;

	_currAnimationTime = 0.0;
	_futureAnimationsStopped = false;

	initializeAnimations();
	addAnimation(animation_idle);
}

AnimatedEntity::~AnimatedEntity()
{
	util::release(_animCtrl);
}

void AnimatedEntity::killEntity()
{
	Entity::killEntity();

	addAnimation(animation_death);
}

void AnimatedEntity::walkForward()
{
	addAnimation(animation_run_shoot);
	_velocity += D3DXVECTOR3(0,0,-1);
}

void AnimatedEntity::walkBackward()
{
	addAnimation(animation_run_shoot);
	_velocity += D3DXVECTOR3(0,0,1);
}

void AnimatedEntity::strafeLeft()
{
	_velocity += D3DXVECTOR3(1,0,0);
	addAnimation(animation_run_shoot);
}

void AnimatedEntity::strafeRight()
{
	_velocity += D3DXVECTOR3(-1,0,0);
	addAnimation(animation_run_shoot);
}

void AnimatedEntity::dash()
{
    if(_canDashTimer > 0)
        return;

    _speed = _maxSpeed * 6.0f;
    _dashTimer = 0.2f;
    _canDashTimer = 2.0f;
	//eventManager->raiseEvent(EventPtr(new Ev_Play_Sound(
	//		"media/sounds/soundFXCD/dash.wav", 
	//		boost::optional<D3DXVECTOR3>(_pos), 
	//		isPlayerEntity())));
}

void AnimatedEntity::turnRight()
{
	_yawAngle = util::wrapAngle(_yawAngle + (6.0f * getDtSeconds()), 0, 2.0f*D3DX_PI);
}
void AnimatedEntity::turnLeft()
{
	_yawAngle = util::wrapAngle(_yawAngle - (6.0f * getDtSeconds()), 0, 2.0f*D3DX_PI);
}
void AnimatedEntity::turnTo(float angle)
{
	_yawAngle = angle;
}

bool AnimatedEntity::turnTo(D3DXVECTOR3 target)
{
	D3DXVECTOR3 diff = target - _pos;
	
	float angle = atan2(diff.x,diff.z)+D3DX_PI;
	_pitchAngle = -atan(diff.y/sqrt(diff.x*diff.x+diff.z*diff.z));

	//Putting this here kills reaction time somewhat, but makes more fluid when going straight
	if(angle - _yawAngle < 0.1 && angle - _yawAngle > -0.1)
		return true;

	//WTF, I turn right when I should turn left.  WHAT DID YOU DO!!
	//Just reversed them..someone used some messed up angles
	if(_yawAngle < angle )
	{
		if ( angle - _yawAngle > D3DX_PI)
			turnLeft();
		else 	
			turnRight();

		// Stop the jittering goddamn it!
		if (_yawAngle > angle) _yawAngle = angle;
	}
	else if(_yawAngle > angle)
	{
		if (_yawAngle - angle > D3DX_PI)
			turnRight();
		else 
			turnLeft();

		// Stop the jittering goddamn it!
		if (_yawAngle < angle) _yawAngle = angle;
	}

	return false ;
}

void AnimatedEntity::jump()
{
	addAnimation(animation_run_shoot);
    _isJumping = true;
}

void AnimatedEntity::shoot()
{
    assert(_weaponNum < _weapons.size());

    if(_weapons[_weaponNum]->weaponAmmoInClip == 0)
    {
		if(_reloadSoundDelay > 0)
			return;

        eventManager->raiseEvent(EventPtr(new Ev_Play_Sound(
			_weapons[_weaponNum]->weaponEmptySound, 
			boost::optional<D3DXVECTOR3>(_pos), 
			isPlayerEntity())));

		_reloadSoundDelay = 0.4f;
		return;
    }

	if(_weaponCoolDownCounter > 0 || _weaponReloadDelayCounter > 0 || _weaponChangeDelayCounter > 0)
		return;

    //we set the animation then play the sound
    addAnimation(animation_run_shoot);
	eventManager->raiseEvent(EventPtr(new Ev_Play_Sound(
		_weapons[_weaponNum]->projectileLaunchSound, 
		boost::optional<D3DXVECTOR3>(_pos), 
		isPlayerEntity())));

    //add in the projectiles
    for(int x = 0; x < _weapons[_weaponNum]->numProjectiles; x++)
	{
	    game->addProjectile(
			_weapons[_weaponNum], 
			_pos + _velocity, 
			_yawAngle, _pitchAngle, 
			_cameraOffsetY, 
			_cameraOffsetZ);
	}

    //reset cooldown counter, and decrement ammo in clip
    _weaponCoolDownCounter = _weapons[_weaponNum]->weaponCoolDown;
	_weapons[_weaponNum]->weaponAmmoInClip--;
    _isFiring = true;
}

void AnimatedEntity::reload()
{
	if (isReloading())
		return;
	
	_currAnimationTime = 0.0;

	assert(_weaponNum < _weapons.size());
	_weaponReloadDelayCounter = _weapons[_weaponNum]->weaponReloadDelay;
	_weapons[_weaponNum]->weaponCurrAmmo += _weapons[_weaponNum]->weaponAmmoInClip;
	if(_weapons[_weaponNum]->weaponCurrAmmo < _weapons[_weaponNum]->weaponClipSize)
	{
		_weapons[_weaponNum]->weaponAmmoInClip = _weapons[_weaponNum]->weaponCurrAmmo;
		_weapons[_weaponNum]->weaponCurrAmmo   = 0;
	}
	else
	{
		_weapons[_weaponNum]->weaponAmmoInClip = _weapons[_weaponNum]->weaponClipSize;
		_weapons[_weaponNum]->weaponCurrAmmo  -= _weapons[_weaponNum]->weaponClipSize;
	}
	eventManager->raiseEvent(EventPtr(new Ev_Play_Sound(
		_weapons[_weaponNum]->reloadSound, 
		boost::optional<D3DXVECTOR3>(_pos), 
		isPlayerEntity())));
}

bool AnimatedEntity::isReloading() const
{
	return _weaponReloadDelayCounter > 0;
}

bool AnimatedEntity::canReload() const
{
	// You're allowed to reload if:
	// 1) You have some ammo (not in your current clip) remaining.
	// 2) Your current clip is not full.
	return _weapons[_weaponNum]->weaponCurrAmmo > 0 &&
		   _weapons[_weaponNum]->weaponAmmoInClip < _weapons[_weaponNum]->weaponClipSize;
}

void AnimatedEntity::nextWeapon()
{
	if(_weaponChangeDelayCounterMin > 0)
		return; 

	int tempWeapon = _weaponNum;
	_weaponNum++;
	if(_weaponNum == _weapons.size())
		_weaponNum = 0;

	if(tempWeapon == _weaponNum)
		return;

	_weaponChangeDelayCounter = _weapons[_weaponNum]->weaponChangeDelay;
	_weaponChangeDelayCounterMin = 0.20f;
	eventManager->raiseEvent(EventPtr(new Ev_Play_Sound(
		_weapons[_weaponNum]->weaponEquipSound, 
		boost::optional<D3DXVECTOR3>(_pos), 
		isPlayerEntity())));
}
void AnimatedEntity::previousWeapon()
{
	if(_weaponChangeDelayCounterMin > 0)
		return; 
	 
	int tempWeapon = _weaponNum;
	_weaponNum--;
	if(_weaponNum == -1)
		_weaponNum = _weapons.size() - 1;

	if(tempWeapon == _weaponNum)
		return;

	_weaponChangeDelayCounter = _weapons[_weaponNum]->weaponChangeDelay;
	_weaponChangeDelayCounterMin = 0.20f;
	eventManager->raiseEvent(EventPtr(new Ev_Play_Sound(
		_weapons[_weaponNum]->weaponEquipSound, 
		boost::optional<D3DXVECTOR3>(_pos), 
		isPlayerEntity())));
}
void AnimatedEntity::setWeapon(int weaponNum)
{
    assert(weaponNum >= 0);

    if(_weaponChangeDelayCounterMin > 0)
        return; 

    if(weaponNum >= _weapons.size() || _weaponNum == weaponNum)
        return;

    _weaponNum = weaponNum;
    _weaponChangeDelayCounter = _weapons[_weaponNum]->weaponChangeDelay;
	_weaponChangeDelayCounterMin = 0.20f;
    eventManager->raiseEvent(EventPtr(new Ev_Play_Sound(
		_weapons[_weaponNum]->weaponEquipSound, 
		boost::optional<D3DXVECTOR3>(_pos), 
		isPlayerEntity())));
}
void AnimatedEntity::throwGrenade()
{
    assert(_grenade);

    if(_weaponCoolDownCounter > 0 || _weaponReloadDelayCounter > 0 || _weaponChangeDelayCounter > 0 || _grenade->weaponAmmoInClip == 0)
        return;

    _weaponCoolDownCounter = _grenade->weaponCoolDown;
    addAnimation(animation_run_shoot);
    eventManager->raiseEvent(EventPtr(new Ev_Play_Sound(
		_grenade->projectileLaunchSound, 
		boost::optional<D3DXVECTOR3>(_pos), 
		isPlayerEntity())));

    game->addProjectile(
		_grenade, 
		_pos, 
		_yawAngle, 
		_pitchAngle, 
		_cameraOffsetY, 
		_cameraOffsetZ);

    _grenade->weaponAmmoInClip--;
}
void AnimatedEntity::stop()
{
	_velocity = D3DXVECTOR3(0,_velocity.y,0);
	addAnimation(animation_idle);
}

void AnimatedEntity::getCameraOffsets(float &offsetY, float &offsetZ) const
{
	offsetY = _cameraOffsetY;
	offsetZ = _cameraOffsetZ;
}

void AnimatedEntity::addAnimationSequence(const std::string &name, const AnimationSequence &sequence)
{
	// Make sure that we are in fact adding this animation sequence 
	// for the first and only time.
	AnimationPool::iterator it = animations.find(name);
	if (it != animations.end())
	{
		ERRORMSG("Animation sequences already exist");
		return;
	}

	animations[name] = sequence;
}

AnimatedEntity::AnimationInterval AnimatedEntity::getInterval(
	const std::string &name,
	AnimatedEntity::animation_set index)
{
	unsigned vecIndex = AnimatedEntity::animationSetToIndex(index);

	// Make sure that the animation sequence associated with [name] exists!
	AnimationPool::iterator it = animations.find(name);
	if (it != animations.end())
	{
		// At least one animation interval should exist!
		MYASSERT(!it->second.empty());

		// Trying to access an invalid animation interval should just return
		// the first one.  (i.e. the idle animation)
		if (vecIndex >= it->second.size())
			return it->second[0];
		else
			return it->second[vecIndex];
	}
	else
	{
		ERRORMSG("No animation sequence was found!");
		return AnimationInterval();
	}
}

bool AnimatedEntity::animationsExist(const std::string &name)
{
	return animations.find(name) != animations.end();
}

unsigned AnimatedEntity::animationSetToIndex(AnimatedEntity::animation_set anim)
{
	return static_cast<unsigned>(anim) - 1;
}

int AnimatedEntity::getAnimationSetPriority(animation_set anim)
{
	//switch (anim)
	//{
	//case animation_idle:   return 3;
	////case animation_run:    return 4;
	////case animation_strafe: return 5;
	////case animation_shoot:  return 3;
	//case animation_death:  return 1;
	////case animation_jump:   return 11;
	//case animation_reload: return 2;
	//default:               return 99;
	//};

	switch (anim)
	{
	case animation_idle:      return 4;
	case animation_run_shoot: return 3;
	case animation_death:     return 1;
	case animation_reload:    return 2;
	default:                  return 99;
	};
}

void AnimatedEntity::stopFutureAnimations()
{
	_futureAnimationsStopped = true;
}

bool AnimatedEntity::hasStoppedAnimations() const
{
	return _futureAnimationsStopped;
}

void AnimatedEntity::timepass()
{
	if (hasStoppedAnimations())
	{
		/*addAnimation(animation_death);
		selectBestAnimationSet();*/
		return;
	}

	Entity::timepass();

	if (isReloading() && !isDead())
	{
		addAnimation(animation_reload);
		// Adjust the reload animation so that it ends right 
		// when you finish reloading.
		AnimationInterval reloadInterval = AnimatedEntity::getInterval(
			_name, 
			animation_reload);

		float animDuration   = reloadInterval._endTime - reloadInterval._startTime;
		float reloadDuration = _weapons[_weaponNum]->weaponReloadDelay;
		_animationInterval._speed = animDuration / reloadDuration;
	}

	selectBestAnimationSet();

	double deltaAnimationTime = getDtSeconds() * _animationInterval._speed;
	_animCtrl->AdvanceTime(deltaAnimationTime, 0);
	_currAnimationTime += deltaAnimationTime;

	if (_currAnimationTime + _animationInterval._startTime >= 
		_animationInterval._endTime)
	{
		if (isDead())
		{
			_currAnimationTime = _animationInterval._endTime - _animationInterval._startTime;
			stopFutureAnimations();
		}
		else
			_currAnimationTime = 0.0;
	}

	_pendingAnimations.clear();
}

void AnimatedEntity::initializeAnimations()
{
	// Make sure that this entity has at least one animation cycle, and activate it.
	AnimationPool::iterator it = animations.find(_name);
	if (it == animations.end() || it->second.empty())
	{
		ERRORMSG("Entity does not have any animation cycles!");
	}

	_animationInterval = getInterval(_name, AnimatedEntity::animation_idle);
	_animCtrl = _mesh->getNewController();

	// Set up the one and only animation track.
	DWORD numTracks = _animCtrl->GetMaxNumTracks();
	MYASSERT(numTracks >= 1);

	_animCtrl->SetTrackEnable(0, true);
}

void AnimatedEntity::addAnimation(animation_set anim)
{
	// Add this animation set to the list of pending animations
	// for this frame if we haven't already done so.
	for (unsigned i = 0; i < _pendingAnimations.size(); i++)
	{
		if (_pendingAnimations[i] == anim)
			return;
	}

	_pendingAnimations.push_back(anim);
}

void AnimatedEntity::selectBestAnimationSet()
{
	std::sort(
		_pendingAnimations.begin(), 
		_pendingAnimations.end(), 
		AnimationSetLess());

	if (_pendingAnimations.empty() ||
		_currAnimationSet == _pendingAnimations[0])
		return;

	_currAnimationSet = _pendingAnimations[0];
	float oldStart    = _animationInterval._startTime;
	float oldEnd      = _animationInterval._endTime;
	_animationInterval = AnimatedEntity::getInterval(
							_name, 
							_pendingAnimations[0]);

	// Reset the animation time if we're changing animation cycles.
	if (oldStart != _animationInterval._startTime ||
		oldEnd != _animationInterval._endTime)
	{
		_currAnimationTime = 0.0;
	}
}

void AnimatedEntity::renderInstance(int subset)
{
	_animCtrl->SetTrackPosition(
		0, 
		_animationInterval._startTime + _currAnimationTime);
	_animCtrl->AdvanceTime(0, 0);

	_mesh->renderFrames(subset);
}