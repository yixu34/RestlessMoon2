#include "process.hpp"
#include "timer.hpp"
#include "packet.hpp"
#include "sound.hpp"
#include "music.hpp"

#include "gameapp.hpp"

int Process::nextId = 0;

//
// Static id strings go here!
//
const char *const WaitProcess::processName  = "proc_wait";
const char *const QuitProcess::processName  = "proc_quit";
const char *const SoundProcess::processName = "proc_sound";
const char *const MusicProcess::processName = "proc_music";
//////////////////////////////////////////////////////////////////////////

Process::Process()
{
	_removalStatus   = removal_status_keep;
	_id              = nextId++;
	_attachedToList  = false;
	_sourceReachable = &_attachedToList;
}
	
Process::~Process()
{
}

void Process::setNext(ProcessPtr nextProcess)
{
	bool amIReachable = isReachable();

	_nextProcess = nextProcess;

	if (nextProcess)
		nextProcess->_sourceReachable = this->_sourceReachable;
}

void Process::remove()
{
	_removalStatus = removal_status_removeself;
}

void Process::removeAll()
{
	_removalStatus = removal_status_removeall;
}

bool Process::isReachable() const
{
	return _attachedToList || (_sourceReachable != 0 && *_sourceReachable);
}

void Process::setAttached(bool attached)
{
	_attachedToList = attached;
}

//////////////////////////////////////////////////////////////////////////

WaitProcess::WaitProcess(float waitMilliseconds /* = 0 */)
{
	_started          = false;
	_startTime        = 0;
	_waitMilliseconds = waitMilliseconds;
}

WaitProcess::~WaitProcess()
{
}

bool WaitProcess::readFromPacket(PacketPtr packet)
{
	_waitMilliseconds = (float)packet->getInt();

	return true;
}

void WaitProcess::timepass()
{
	if (!_started)
	{
		_startTime = getTimeMilliseconds();
		_started   = true;
	}

	float currTime = getTimeMilliseconds();
	if (currTime >= _startTime + _waitMilliseconds)
		remove();
}

//////////////////////////////////////////////////////////////////////////

QuitProcess::QuitProcess()
{
}

QuitProcess::~QuitProcess()
{
}

bool QuitProcess::readFromPacket(PacketPtr packet)
{
	return true;
}

void QuitProcess::timepass()
{
	gameApp->quit();
	remove();
}

//////////////////////////////////////////////////////////////////////////

SoundProcess::SoundProcess()
{
	_sound.reset(new Sound());
	_startedPlaying = false;
}

SoundProcess::~SoundProcess()
{
}

bool SoundProcess::readFromPacket(PacketPtr packet)
{
	std::string filename = packet->getString();
	return _sound->load(filename);
}

void SoundProcess::timepass()
{
	if (!_startedPlaying)
	{
		_sound->play();
		_startedPlaying = true;
	}
	else if (!_sound->isPlaying())
		remove();
}

//////////////////////////////////////////////////////////////////////////

MusicProcess::MusicProcess()
{
	_music.reset(new Music());
	_startedPlaying = false;
}

MusicProcess::~MusicProcess()
{
}

bool MusicProcess::readFromPacket(PacketPtr packet)
{
	std::string filename = packet->getString();
	return _music->load(filename);
}

void MusicProcess::timepass()
{
	if (!_startedPlaying)
	{
		_music->play();
		_startedPlaying = true;
	}
	else if (!_music->isPlaying())
		remove();
}