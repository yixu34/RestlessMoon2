#ifndef PROCESSMANAGER_HPP
#define PROCESSMANAGER_HPP

#include <boost/shared_ptr.hpp>
#include <list>
#include <map>

class Process;
class Packet;
class ProcessManager;
typedef boost::shared_ptr<Process>        ProcessPtr;
typedef boost::shared_ptr<ProcessManager> ProcessManagerPtr;
typedef boost::shared_ptr<Packet>         PacketPtr;

class ProcessManager
{
public:
	ProcessManager();
	~ProcessManager();

	// Adds a process to the ACTIVE processing queue.
	int attachProcess(const ProcessPtr &process);

	// Constructs a process from a packet and appends 
	// the process to the ACTIVE processing queue.
	int attachProcess(PacketPtr packet);

	// Constructs a process, but does not set it active for processing.
	int createDisabledProcess(PacketPtr packet);

	// Sets sourceId's next process pointer to targetId.
	int connectProcesses(int sourceId, int targetId);

	void timepass();
	void clear();

	static const int processNotFound;

private:
	typedef std::list<ProcessPtr> ProcessList;
	ProcessList _enabledProcesses;

	typedef std::map<int, ProcessPtr> ProcessMap;
	ProcessMap _createdProcesses;

	typedef Process *(*ProcessConstructor)();
	typedef std::map<std::string, ProcessConstructor> ProcessConstructionMap;
	ProcessConstructionMap _processConstructors;

	ProcessPtr findProcess(int processId);
	ProcessPtr createProcess(PacketPtr packet);

	template <typename T>
	void registerProcessConstructor()
	{
		_processConstructors[T::processName] = T::create;
	}
};

extern ProcessManagerPtr processManager;

#endif