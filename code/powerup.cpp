#include "powerup.hpp"
#include "renderer.hpp"
#include "game.hpp"
#include "eventmanager.hpp"
#include "event.hpp"

PowerUp::PowerUp(const D3DXVECTOR3 &pos, PowerUpInfo* newPowerUpInfo)
{
	_pos              = pos;
	_oldPos           = pos;     
	_originBoundBox   = newPowerUpInfo->boundBox;
	_radius           = newPowerUpInfo->radius;
	_soundToPlay      = newPowerUpInfo->soundToPlay;
	_idNum            = nextId++;
	_mesh             = staticMeshCache.get(newPowerUpInfo->mesh);
	_name             = newPowerUpInfo->name;
	_gravitySensitive = true; 

    updateBoundingBox();
	assert(_mesh);
	renderer->addRenderable(_mesh, _name, this);
}

PowerUp::~PowerUp(){}

void PowerUp::timepass()
{
	Entity::timepass();
	if(_isOnFloor)
	{
		_gravitySensitive = false;
		_velocity = D3DXVECTOR3(0,0,0);
	}

}

void PowerUp::getCameraOffsets(float &offsetX, float &offsetY) const
{
	offsetY = (_boundBox.max.y + _boundBox.min.y) / 2;
	offsetX = (_boundBox.max.x + _boundBox.min.x) / 2;
}

//********************************************************************************************************************
HealthPowerUp::HealthPowerUp(D3DXVECTOR3 pos, PowerUpInfo* newPowerUpInfo): PowerUp(pos, newPowerUpInfo)
{
	incHealth = atof(newPowerUpInfo->weaponName.c_str());
}

void HealthPowerUp::collidedWithPlayer()
{
	_isDead = game->player->getEntity()->increaseHealth(incHealth);
	if(_isDead)
		eventManager->raiseEvent(EventPtr(new Ev_Entity_Die(_soundToPlay, this, _name, _pos, getDeathMode())));
}

//********************************************************************************************************************
WeaponPowerUp::WeaponPowerUp(D3DXVECTOR3 pos, PowerUpInfo* newPowerUpInfo): PowerUp(pos, newPowerUpInfo)
{
	_weaponName = newPowerUpInfo->weaponName;
}

void WeaponPowerUp::collidedWithPlayer()
{
	_isDead = game->player->getEntity()->addWeapon(game->getFromWeaponCache(_weaponName));
    if(_isDead)
	    eventManager->raiseEvent(EventPtr(new Ev_Entity_Die(_soundToPlay, this, _name, _pos, getDeathMode())));
}

//********************************************************************************************************************
GrenadePowerUp::GrenadePowerUp(D3DXVECTOR3 pos, PowerUpInfo* newPowerUpInfo): PowerUp(pos, newPowerUpInfo)
{
	_grenadeName = newPowerUpInfo->weaponName;
}

void GrenadePowerUp::collidedWithPlayer()
{
	_isDead = game->player->getEntity()->addGrenade(game->getFromWeaponCache(_grenadeName));
    if(_isDead)
	    eventManager->raiseEvent(EventPtr(new Ev_Entity_Die(_soundToPlay, this, _name, _pos, getDeathMode())));
}