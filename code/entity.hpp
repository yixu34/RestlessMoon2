#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <boost/shared_ptr.hpp>
#include <d3dx9.h>
#include <string>

#include "boundingvolume.hpp"
#include "renderable.hpp"

class StaticMesh;
class SkinnedMesh;

class Entity;
typedef boost::shared_ptr<Entity> EntityPtr;

struct NewEntityStruct;
struct WeaponInfo;

class Entity
{
	public:
		Entity();
		Entity(NewEntityStruct* newEntityParams);
		virtual ~Entity();

		enum death_mode
		{
			death_leaves_corpse = 0,
			death_explodes,
			death_disappear,
		};

		enum on_hit_mode
		{
			on_hit_nothing = 0,
			on_hit_blood_red = 1,
			on_hit_blood_green = 2,
			on_hit_sparks = 3,
		};
	
		void takeDamage(float damage);	      //deals damage to the entity
		bool increaseHealth(float incHealth); //increases a unit's health
		bool isPlayerEntity() const;
		virtual void timepass();		      //moves the entity, and lowers delay counters
		virtual void killEntity();            //tells the entity to die
		void normalizeVelocity();		      //gets the displacement vector
		virtual void renderInstance(int subset) {}
		death_mode getDeathMode() const;
		on_hit_mode getOnHitMode() const;

		//getter functions
		int getIDNum()									 const {return _idNum;}
		int getNumBullets()								 const; 
		int getTotalBullets()						     const; 
        int getNumGrenades()                             const; 
		int getWeaponNum()								 const {return _weaponNum;}
        bool isStatic()									 const {return _isStatic;}
		bool isDead()									 const {return _isDead;}
		bool onTheFloor()								 const {return _isOnFloor;}
        bool isFiring()                                  const {return _isFiring;}
		void getWorldMatrix(D3DXMATRIX &outWorld)		 const;
		const BoundingBox &getBoundingBox()				 const {return _boundBox;}
		float getRadius()								 const {return _radius;}
		float getCollideDamage()						 const {return _collisionDamage;}
		float getRadialDamage()                          const {return _radialDamage;}
		float getCoeffRestitution()						 const {return _coeffOfRestitution;}
		float getSpeed()								 const {return _speed;}
		float getYawAngle()								 const {return _yawAngle;}
        float getPitchAngle()							 const {return _pitchAngle;}
		float getHealth()								 const {return _health;}
		float getMaxHealth()							 const {return _maxHealth;}
		float getLastHitTime()                           const {return _lastHitTime;}
		float getDashTimer()                             const {return _dashTimer;}
		float getDashCooldown()                          const {return _canDashTimer;}
		float getTimeOfDeath()                           const {return _timeOfDeath;}
		std::string getName()							 const {return _name;}
		const WeaponInfo *getWeapon()                    const {assert(!_weapons.empty()); return _weapons[_weaponNum];}
		std::vector<WeaponInfo *>* getWeaponsVec()             {return &_weapons;}
		D3DXVECTOR3 getVelocity()						 const {return _velocity;}
		D3DXVECTOR3 getPos()							 const {return _pos;}
		D3DXVECTOR3 getOldPos()                          const {return _oldPos;}

		virtual void getCameraOffsets(float &offsetY, float &offsetZ) const = 0;

		//setter functions
		void setPos(const D3DXVECTOR3& pos)			{_pos       = pos;}
		void setOnTheFloor(bool isOnTheFloor)		{_isOnFloor = isOnTheFloor;}
		void setVelocity(const D3DXVECTOR3 &vel);
		void setHealth(float health)				{_health    = health;}
		bool addGrenade(WeaponInfo* grenade);
        bool addWeapon(WeaponInfo* newWeapon);
        void resetFiring()                          {_isFiring  = false;}
		void setWeaponNum(int weaponNum)			{_weaponNum = weaponNum;}
		void setNumGrenades(int numGrenades);
       
        void yaw(float angle);
        void pitch(float angle);
		
	protected:
		friend Renderable;
		
		void updateBoundingBox();

		BoundingBox _boundBox;
		BoundingBox _originBoundBox;

		std::string _name; 
		float _health;
		float _maxHealth;
		float _lastHitTime;
		int _idNum;
		int _weaponNum;
		bool _isStatic;
		bool _isDead;
		bool _isOnFloor;
		bool _diesOnImpact;
		bool _diesAfterTime;
		bool _gravitySensitive;
        bool _isProjectile;
        bool _isFiring;
        bool _isJumping;
		bool _dropsItemsAtDeath;
		death_mode _deathMode;
		on_hit_mode _onHitMode;
		float _collisionDamage;
		float _yawAngle;
		float _pitchAngle;
        float _maxSpeed;
		float _speed;
        float _dashTimer;
        float _canDashTimer;
        float _weight;
        float _coeffOfRestitution;
		float _oldY;
		float _radialDamage;
		float _radius;
		float _timeOfDeath;
		float _dropTable[6];
		D3DXVECTOR3 _pos;
		D3DXVECTOR3 _oldPos;
		D3DXVECTOR3 _velocity;
		D3DXVECTOR3 _tempVel;
		std::vector<WeaponInfo *> _weapons;
        WeaponInfo* _grenade;
		
		//sound information
		std::string _hitSound;
		std::string _deathSound;
		std::string _footStepSound;

		//counters for the current weapon
		float _weaponCoolDownCounter;
		float _weaponReloadDelayCounter;
		float _weaponChangeDelayCounter;
		float _weaponChangeDelayCounterMin;
		float _reloadSoundDelay;
		float _walkSoundDelay;

		static int nextId;
};

struct WeaponInfo
{
	int projectileSpeed; 
	int weaponClipSize;
	int weaponMaxAmmo;
	int weaponAmmoInClip;
	int weaponCurrAmmo;
	int numProjectiles;
	bool projectileDiesOnImpact;
	bool projectileDiesAfterTime;
	bool projectileGravitySensitive; 
	float inaccuracy;
	float projectileDamage;
	float projectileRadialDamage;
	float maxInaccuracy;
	float minInaccuracy;
	float weaponChangeDelay;
	float weaponReloadDelay;
	float weaponCoolDown;
	float projectileLife;
	float projectileCoeffOfRestitution;
	float projectileWeight;
	std::string weaponName;
	std::string projectileLaunchSound;
	std::string projectileHitSound; 
	std::string projectileIntermedSound;
	std::string reloadSound;
	std::string projectileMesh;
	std::string weaponEquipSound;
	std::string weaponEmptySound;
	std::vector<D3DXVECTOR3> bulletOffset;
	BoundingBox bulletBoundBox;
};
struct NewEntityStruct
{
	D3DXVECTOR3 position;
	std::string name;
	std::string mesh;
	std::string hitSound;
	std::string footStepSound;
	std::string deathSound; 
	float hitPoints; 
	float collisionDamage; 
	bool isStatic;
	bool diesOnImpact;
	bool diesAfterTime;
	bool gravitySensitive;
	bool isProjectile;
	bool dropsItemsAtDeath;
	Entity::death_mode deathMode;
	Entity::on_hit_mode onHitMode;
	float coeffOfRestitution;
	float weight;
	float yawAngle;
	float pitchAngle;
	float speed;
	float cameraOffsetY;
	float cameraOffsetZ;
	float radialDamage;
	float radius;
	float dropTable[6];
	BoundingBox boundBox;
	std::vector<WeaponInfo *> weapons;
};


#endif
