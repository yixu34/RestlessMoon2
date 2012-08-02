#include <boost/optional.hpp>

#include "audio.hpp"
#include "sound.hpp"
#include "music.hpp"
#include "game.hpp"
#include "camera.hpp"
#include "entity.hpp"
#include <string>

AudioPtr audio;

Audio::Audio()
{
	_system			= 0;
	_musicFile		= MusicPtr();
	interuptedMusic = false;
	_musicVolume    = 0.25f;
}

Audio::~Audio()
{
}

bool Audio::initialize()
{
	if (FMOD::System_Create(&_system) != FMOD_OK)
		return false;

	if (_system->init(100, FMOD_INIT_NORMAL, 0) != FMOD_OK)
		return false;

	_system->set3DSettings(1.0f, 1450.0f, 1.0f);

	return true;
}


void Audio::shutdown()
{
	_system->release();
	_system = 0;
}

void Audio::setMasterVolume(float volume)
{
	FMOD::ChannelGroup *channelGroup;
	_system->getMasterChannelGroup(&channelGroup);
	channelGroup->setVolume(volume);
}

void Audio::setMusicVolume(float volume)
{
	_musicFile->setVolume(volume);
	_musicVolume = volume;
}

bool Audio::playSound(const Event& soundToPlay)
{
	SoundToPlay* soundToPlayData = new SoundToPlay();
	std::string sound = "";
	if(soundToPlay.getType().getString() == Ev_Play_Sound::eventName)
	{
		EvPlaySound_data *eventData = soundToPlay.getRawData<EvPlaySound_data>();
		sound = eventData->_soundToPlay;
		soundToPlayData->position = eventData->_position;
		soundToPlayData->headRelative = eventData->_headRelative;

	}
	if(soundToPlay.getType().getString() == Ev_Entity_Die::eventName)
	{
		EvEntityDie_data *eventData = soundToPlay.getRawData<EvEntityDie_data>();
		sound = eventData->_soundToPlay;
		soundToPlayData->position = eventData->_deathPosition;
		soundToPlayData->headRelative = false;
	}
	
	soundToPlayData->sound = soundCache.get(sound);
	if(soundToPlayData->sound)
    {
		_soundsToPlay.push_back(soundToPlayData);
        soundToPlayData->sound->setLooping(0);
    }

	return false;
}
bool Audio::playMusic(const Event& musicFile)
{
	EvPlayMusic_data *eventData = musicFile.getRawData<EvPlayMusic_data>();
	std::string music = eventData->_musicFile;
	bool interupted = eventData->_interupt;
	bool looping = true;
	MusicPtr loadedMusic = musicCache.get(music);

	if(interupted)
	{
		// FIXME:  There is a null pointer somewhere in this block sometimes.
		// I think this is fixed, tell me if there is a still a problem -Diego
		looping = false;
		_interptedMusicFile = _musicFile;
		_musicFile = MusicPtr();
		interuptedMusic = true;
		if(_interptedMusicFile)
			_interptedMusicFile->setPaused(true);
	}

	if(loadedMusic && _musicFile != loadedMusic)
	{
		if(_musicFile)
			_musicFile->stop();

		_musicFile = loadedMusic;
		_musicFile->play();
		_musicFile->setLooping(looping);
       setMusicVolume(_musicVolume);
	}
	return false;
}

void Audio::registerHandlers()
{
	registerHandler(
		EventType(Ev_Play_Sound::eventName), 
		EventHandlerFunctorPtr(new BaseEventHandlerFunctor<Audio>(this, &Audio::playSound)));

	registerHandler(
		EventType(Ev_Play_Music::eventName), 
		EventHandlerFunctorPtr(new BaseEventHandlerFunctor<Audio>(this, &Audio::playMusic)));

	registerHandler(
		EventType(Ev_Entity_Die::eventName), 
		EventHandlerFunctorPtr(new BaseEventHandlerFunctor<Audio>(this, &Audio::playSound)));
}

void Audio::timepass()
{
	updateListener();

	_system->update();


	for(std::vector<SoundToPlay*>::iterator i = _soundsToPlay.begin(); i != _soundsToPlay.end(); i++)
	{
		//if we have something that's at (0,0,0) we assume that it's not trying to play positionally, 
		//or if the game is not in progress. otherwise we are going to play 3d sound
		//if((*i)->position.x == 0 && (*i)->position.y == 0 && (*i)->position.z == 0 || !game->isGameInProgress())
		if (!game->isGameInProgress() || !((*i)->position))
			(*i)->sound->play();
		//else if(game->isGameInProgress())
		else
		{
			Vector3 sourcePos;
			sourcePos.x = (*i)->position->x;
			sourcePos.y = (*i)->position->y;
			sourcePos.z = (*i)->position->z;

			(*i)->sound->play3D(sourcePos, (*i)->headRelative);
		}

		delete (*i);
	}

	if(interuptedMusic && !_musicFile->isPlaying())
	{
		interuptedMusic = false;
		_musicFile = _interptedMusicFile;
		_interptedMusicFile = MusicPtr();
		if(_musicFile)
		{
			_musicFile->setPaused(false);
			_musicFile->setLooping(true);
			_musicFile->setVolume(0.5f);
		}
	}
	_soundsToPlay.clear();
}

void Audio::updateListener()
{
	if (!game->isGameInProgress())
		return;

	EntityPtr playerEntity   = game->player->getEntity();
	D3DXVECTOR3 listenerPos1 = playerEntity->getPos();
	float offsetY, offsetZ;
	playerEntity->getCameraOffsets(offsetY, offsetZ);
	listenerPos1.y += offsetY;

	Vector3 listenerPos2;
	listenerPos2.x = listenerPos1.x;
	listenerPos2.y = listenerPos1.y;
	listenerPos2.z = listenerPos1.z;

	D3DXVECTOR3 upVector1 = camera->getUpVector();
	D3DXVec3Normalize(&upVector1, &upVector1);
	Vector3 upVector2;
	upVector2.x = upVector1.x;
	upVector2.y = upVector1.y;
	upVector2.z = upVector1.z;

	D3DXVECTOR3 lookVector1 = camera->getLookVector();
	D3DXVec3Normalize(&lookVector1, &lookVector1);
	Vector3 lookVector2;
	lookVector2.x = lookVector1.x;
	lookVector2.y = lookVector1.y;
	lookVector2.z = lookVector1.z;

	Vector3 velocity = {0.0f, 0.0f, 0.0f};
	audio->getDevice()->set3DListenerAttributes(
		0, 
		&listenerPos2, 
		&velocity,
		&lookVector2, 
		&upVector2);
}