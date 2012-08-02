#include "humanplayer.hpp"
#include "game.hpp"

HumanPlayer::HumanPlayer(const AnimatedEntityPtr& entity): Player(entity)
{
	_isShooting               = false;
    _isReloading              = false;
	_isChangeToPreviousWeapon = false;
	_isChangeToNextWeapon     = false;
    _isThrowingGrenade        = false;
    _weaponNum                = -1;
}

HumanPlayer::~HumanPlayer(void)
{

}

void HumanPlayer::updateMovement()
{
    trackMouse();

	// We need to freeze the entity's velocity and set its idle animation
	// separately.  Otherwise, we would keep telling the entity to draw
	// an idle animation, even if it were moving.
	_entity->setVelocity(D3DXVECTOR3(0,_entity->getVelocity().y,0));

	if (_entity->isDead())
		return;

	if (game->areEntitiesFrozen())
	{
		_entity->stop();
		return;
	}

	if(_entity->onTheFloor() && _flyingUp)
		_entity->jump();

	// If you're not doing anything related to an animation, then
	// fall back to the idle animation.
	if (!(_walkingForward || _walkingBackward ||
		  _strafingLeft || _strafingRight ||
		  _dashing))
	{
		_entity->stop();
	}
	else
	{
		// Otherwise, change animations and update your velocity.
		if(_walkingForward)
			_entity->walkForward();

		if(_walkingBackward)
			_entity->walkBackward();

		if(_dashing)
			_entity->dash();

		if(_strafingLeft)
			_entity->strafeLeft();

		if(_strafingRight)
			_entity->strafeRight();
	}
	
	if(_yawingRight)
		_entity->turnRight();

	if(_yawingLeft)
		_entity->turnLeft();
}	

void HumanPlayer::timepass(void)
{
	_isReloading = _entity->isReloading();

	if (!_entity || _entity->isDead() || game->areEntitiesFrozen())
		return;

	if(_isShooting)
		_entity->shoot();

	if(_isThrowingGrenade)
		_entity->throwGrenade();

	if (!_isReloading)
	{
		if(_isChangeToPreviousWeapon)
			changeToPreviousWeapon();

		if(_isChangeToNextWeapon)
			changeToNextWeapon();

		if(_weaponNum >= 0)
			_entity->setWeapon(_weaponNum);
	}

    _weaponNum = -1;
}

void HumanPlayer::changeToPreviousWeapon()
{
	_entity->previousWeapon();
}

void HumanPlayer::changeToNextWeapon()
{
	_entity->nextWeapon();
}

void HumanPlayer::yaw(float angle)
{
    _entity->yaw(angle);
}

void HumanPlayer::pitch(float angle)
{
    _entity->pitch(angle);
}

void HumanPlayer::reload()
{
	 if (!_isReloading && _entity->canReload())
	 {
		 _isReloading = true;
		 _entity->reload();
	 }
}