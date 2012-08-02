#ifndef HUMANPLAYER_HPP
#define HUMANPLAYER_HPP

#include <boost/shared_ptr.hpp>
#include "player.hpp"
#include "animatedentity.hpp"
#include "controllable.hpp"

class HumanPlayer;
typedef boost::shared_ptr<HumanPlayer> HumanPlayerPtr;

class HumanPlayer : public Player, public Controllable
{
public:
	HumanPlayer(const AnimatedEntityPtr& entity = AnimatedEntityPtr());
	void timepass();

	void setShooting(bool val)				{_isShooting                     = val;}
	//void setReloading(bool val)				{_isReloading			         = val;}
	void setChangeToPreviousWeapon(bool val){_isChangeToPreviousWeapon       = val;}
	void setChangeToNextWeapon(bool val)    {_isChangeToNextWeapon           = val;}
    void setThrowingGrenade(bool val)       {_isThrowingGrenade              = val;}
    void setWeapon(int weaponNum)           {if(_weaponNum == -1) _weaponNum = weaponNum;}
	void changeToPreviousWeapon();
	void changeToNextWeapon();

    void yaw(float angle);
    void pitch(float angle);
	void reload();

    void updateMovement();

private:
	bool _isShooting;
	bool _isReloading;
	bool _isChangeToPreviousWeapon;
	bool _isChangeToNextWeapon;
    bool _isThrowingGrenade; 
    int _weaponNum;

public:
	virtual ~HumanPlayer(void);
};

#endif
