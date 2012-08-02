#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/optional/optional_fwd.hpp>

#include "entity.hpp"
#include "player.hpp"
#include "humanplayer.hpp"
#include "aiplayer.hpp"
#include "animatedentity.hpp"
#include "staticmesh.hpp"
#include "controllable.hpp"
#include "eventlistener.hpp"
#include "frame.hpp"
#include "powerup.hpp"
#include "util.hpp"

class Trigger;
typedef boost::shared_ptr<Trigger> TriggerPtr;

class Game;
typedef boost::shared_ptr<Game> GamePtr;

extern GamePtr game;
class Game : public Frame, public EventListener
{
	public:
		Game();
        ~Game();
		bool initialize();
		void onEnable();
		void onDisable();
		void registerHandlers();

        void reloadCurrentLevel();

		void dealRadialDamage(const D3DXVECTOR3& pos, float damage);
        bool isGameInProgress() const {return _levelLoaded;}
        AIPlayerPtr getAIPlayer(int idNum);
        WeaponInfo*  getFromWeaponCache(std::string weaponName);
		PowerUpInfo* getFromPowerUpCache(std::string powerUpName);
		std::string getCurrentLevelFile() const {if(_levelLoaded) return _currentLevel; else return "";} 

		void addEntity(const EntityPtr& newEntity);
        void addAIPlayer(const AnimatedEntityPtr &aiEntity);
        void addToWeaponCache(WeaponInfo* newWeapon);
		void addToPowerUpCache(PowerUpInfo* newPowerUp);
		void addToPowerUpVector(PowerUpPtr newPowerUp)	{_powerUps.push_back(newPowerUp);}
		void addToTriggerList(TriggerPtr trigger)		{_triggers.push_back(trigger);}
		void addProjectile(
			WeaponInfo* newBullet, 
			const D3DXVECTOR3& pos, 
			float yawAngle, 
			float pitchAngle, 
			float cameraOffsetY, 
			float cameraOffsetZ);

		void removeEntity(int idNum);
		void removeEntityController(int idNum);

		void timepass();
		bool areEntitiesFrozen() const;
		void freezeEntities(float timeSeconds);
		void setTalker(std::string talker) {_whoIsTalking = talker;}

		boost::optional<std::string> getRenderString(const std::string &varName) const;
		boost::optional<float> getRenderNumber(const std::string &varName) const;

		HumanPlayerPtr player;
        ControllablePtr currControllable;

	private:
		bool onStartGame(const Event &event);

		void loadLevel(const std::string& levelName);
        void checkCollision(EntityPtr entityCollide);
        void checkCollisionWithProjectiles(EntityPtr entityCollide);
		void checkCollisionWithPowerUp();
        void unloadLevel();

		void updateRenderVars();

		typedef std::map<std::string, std::string, util::CaseInsensitiveLess> StringMap;
		StringMap _renderStrings;

		typedef std::map<std::string, float, util::CaseInsensitiveLess> NumberMap;
		NumberMap _renderNumbers;

		std::vector<WeaponInfo *> prevWeapons;
		float prevHealth;
		int prevWeaponNum;
		int prevNumGrenades;

		typedef std::vector<EntityPtr> EntityList;
		typedef std::vector<AIPlayerPtr> PlayerList;
		typedef std::vector<PowerUpPtr> PowerUpList;
		typedef std::vector<TriggerPtr> TriggerList;
        typedef std::map<std::string, WeaponInfo *> WeaponList;
		typedef std::map<std::string, PowerUpInfo *> PowerUpCache;
		EntityList _entities;
		EntityList _deadEntities;
        EntityList _projectiles;
		PlayerList _aiPlayers;
		PowerUpList _powerUps;
        WeaponList _weaponsList;
		PowerUpCache _powerUpCache;
		TriggerList _triggers;
		float _freezeTimer;
		std::string _whoIsTalking;
		
		bool _levelLoaded;
        std::string _currentLevel;

        unsigned int _frame;
};

#endif
