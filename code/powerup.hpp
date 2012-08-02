#ifndef POWERUP_HPP
#define POWERUP_HPP

#include <boost/shared_ptr.hpp>
#include <d3dx9.h>
#include "boundingvolume.hpp"
#include "staticmesh.hpp"
#include "entity.hpp"

struct PowerUpInfo
{
	std::string name;
	std::string mesh;
	BoundingBox boundBox;
	float radius;
	std::string soundToPlay;
	std::string weaponName;
};

class PowerUp;
class HealthPowerUp;
class WeaponPowerUp;
class GrenadePowerUp;

typedef boost::shared_ptr<PowerUp> PowerUpPtr;
typedef boost::shared_ptr<HealthPowerUp> HealthPowerUpPtr;
typedef boost::shared_ptr<WeaponPowerUp> WeaponPowerUpPtr;
typedef boost::shared_ptr<GrenadePowerUp> GrenadePowerUpPtr;

class PowerUp: public Entity
{
	public:
		PowerUp(const D3DXVECTOR3 &pos, PowerUpInfo* newPowerUpInfo);
		virtual ~PowerUp();
		virtual void timepass();
		virtual void collidedWithPlayer() = 0; //returns a true if the player needed the powerup
		void getCameraOffsets(float &offsetX, float &offsetY) const;

	protected:
		StaticMeshPtr _mesh;
		std::string _soundToPlay;
		std::string _name;
};

class HealthPowerUp: public PowerUp
{
	public:
		HealthPowerUp(D3DXVECTOR3 pos, PowerUpInfo* newPowerUpInfo);
		~HealthPowerUp(){;}
		void collidedWithPlayer();

	private:
		float incHealth;
};

class WeaponPowerUp: public PowerUp
{
	public:
		WeaponPowerUp(D3DXVECTOR3 pos, PowerUpInfo* newPowerUpInfo);
		~WeaponPowerUp(){;}
		void collidedWithPlayer();

	private:
		std::string _weaponName;
};

class GrenadePowerUp: public PowerUp
{
	public:
		GrenadePowerUp(D3DXVECTOR3 pos, PowerUpInfo* newPowerUpInfo);
		~GrenadePowerUp(){;}
		void collidedWithPlayer();

	private:
		std::string _grenadeName;
};

#endif
