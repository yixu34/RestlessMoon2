#ifndef PROCESS_HPP
#define PROCESS_HPP

//
// Represents an arbitrary task or action that occurs in the game.
// Each Process type should have a default constructor, because 
// processes can be created via packet.  This is because we can 
// construct packets in a script file.
//

#include <boost/shared_ptr.hpp>

#include "packetconstructable.hpp"

#define VIRTUAL_CONSTRUCTOR(BaseClass, DerivedClass) \
	friend class ProcessManager; \
	static BaseClass *create() { \
		return new DerivedClass();  \
	}

//////////////////////////////////////////////////////////////////////////

class Process;
typedef boost::shared_ptr<Process> ProcessPtr;

class Process : public PacketConstructable
{
public:
	Process();
	virtual ~Process();

	virtual void timepass() = 0;

	void remove();
	void removeAll();

	inline int getId() const { return _id; }

protected:
	friend class ProcessManager;

	void setNext(ProcessPtr nextProcess);
	bool isReachable() const;
	void setAttached(bool attached);

	enum removal_status
	{
		removal_status_keep, 
		removal_status_removeself, 
		removal_status_removeall
	};

private:
	bool  _attachedToList;
	bool *_sourceReachable;
	int _id;
	removal_status _removalStatus;
	ProcessPtr _nextProcess;

	static int nextId;
};

//////////////////////////////////////////////////////////////////////////

class WaitProcess : public Process
{
public:
	WaitProcess(float waitMilliseconds = 0);
	~WaitProcess();

	bool readFromPacket(PacketPtr packet);

	void timepass();

	static const char *const processName;

private:
	VIRTUAL_CONSTRUCTOR(Process, WaitProcess)

	bool _started;
	float _startTime;
	float _waitMilliseconds;
};

//////////////////////////////////////////////////////////////////////////

class QuitProcess : public Process
{
public:
	QuitProcess();
	~QuitProcess();
	
	bool readFromPacket(PacketPtr packet);

	void timepass();

	static const char *const processName;

private:
	VIRTUAL_CONSTRUCTOR(Process, QuitProcess)
};

//////////////////////////////////////////////////////////////////////////

class Sound;
typedef boost::shared_ptr<Sound> SoundPtr;

class SoundProcess : public Process
{
public:
	SoundProcess();
	~SoundProcess();

	bool readFromPacket(PacketPtr packet);

	void timepass();

	static const char *const processName;

private:
	VIRTUAL_CONSTRUCTOR(Process, SoundProcess)

	SoundPtr _sound;
	bool _startedPlaying;
};

//////////////////////////////////////////////////////////////////////////
class Music;
typedef boost::shared_ptr<Music> MusicPtr;

class MusicProcess : public Process
{
public:
	MusicProcess();
	~MusicProcess();

	bool readFromPacket(PacketPtr packet);

	void timepass();

	static const char *const processName;

private:
	VIRTUAL_CONSTRUCTOR(Process, MusicProcess)

	MusicPtr _music;
	bool _startedPlaying;
};

#endif