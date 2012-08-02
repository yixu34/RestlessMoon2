#include "eventdata.hpp"

EventArgumentList EventData::getArguments() const
{
	return EventArgumentList();
}

/**/////////////////////////////////////////////////////////////////////////

EvData_Test::EvData_Test(float x, float y)
{
	_x = x;
	_y = y;
}

EventArgumentList EvData_Test::getArguments() const
{
	EventArgumentList args;
	args.push_back(convertArgument(_x));
	args.push_back(convertArgument(_y));

	return args;
}

//////////////////////////////////////////////////////////////////////////

EvPlaySound_data::EvPlaySound_data(
	const std::string &soundToPlay, 
	const boost::optional<D3DXVECTOR3> &position, 
	bool headRelative)
{
	_soundToPlay  = soundToPlay;
	_position     = position;
	_headRelative = headRelative;
}

//////////////////////////////////////////////////////////////////////////

EvPlayMusic_data::EvPlayMusic_data(const std::string &musicFile, bool interupt)
{
	_musicFile = musicFile;
	_interupt = interupt;
}
//////////////////////////////////////////////////////////////////////////

EvEntityDie_data::EvEntityDie_data(
	const std::string &deathSound, 
	Entity* entityToDie, 
	const std::string &entityName, 
	const D3DXVECTOR3 &deathPosition, 
	Entity::death_mode deathMode)
{
	_soundToPlay   = deathSound;
	_entityName    = entityName;
	_entityToDie   = entityToDie; 
	_deathPosition = deathPosition;
	_deathMode     = deathMode;
}

//////////////////////////////////////////////////////////////////////////

EvEntityHit_data::EvEntityHit_data(
	const D3DXVECTOR3 &projectilePosition, 
	const D3DXVECTOR3 &projectileVelocity, 
	Entity::on_hit_mode onHitMode)
{
	_projectilePosition = projectilePosition;
	_projectileVelocity = projectileVelocity;
	_onHitMode          = onHitMode;
}

//////////////////////////////////////////////////////////////////////////

EvMapLoad_data::EvMapLoad_data(const std::string &mapName, int type)
{
    _mapName = mapName;
	_type = type;
}

//////////////////////////////////////////////////////////////////////////

EvShakeScreen_data::EvShakeScreen_data(float shakeStrength, float shakeTime)
{
	_shakeStrength = shakeStrength;
	_shakeTime     = shakeTime;
}