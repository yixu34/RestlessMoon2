#include <string>
#include <algorithm>

#include "packet.hpp"
#include "process.hpp"
#include "processmanager.hpp"
#include "common.hpp"

ProcessManagerPtr processManager;
const int ProcessManager::processNotFound = -1;

// Comparison function used for searching a list for a process id.
template <typename T, typename U>
class ProcessSearch : public std::binary_function<T, U, bool>
{
public:
	bool operator()(const T &process, U id) const
	{
		return process->getId() == id;
	}
};

ProcessManager::ProcessManager()
{
	registerProcessConstructor<WaitProcess>();
	registerProcessConstructor<QuitProcess>();
	registerProcessConstructor<SoundProcess>();
	registerProcessConstructor<MusicProcess>();
}

ProcessManager::~ProcessManager()
{
}

int ProcessManager::attachProcess(const ProcessPtr &process)
{
	int processId = process->getId();

	// If the process hasn't been registered at all, then add it to the map.
	if (!findProcess(processId))
		_createdProcesses[processId] = process;

	// If the process is NOT enabled, then add it to the enabled list.
	ProcessList::iterator it = std::find_if(
		_enabledProcesses.begin(), 
		_enabledProcesses.end(), 
		std::bind2nd(ProcessSearch<ProcessPtr, int>(), processId));

	if (it == _enabledProcesses.end())
		_enabledProcesses.push_back(process);

	process->setAttached(true);

	return processId;
}

int ProcessManager::attachProcess(PacketPtr packet)
{
	ProcessPtr process = createProcess(packet);
	if (process)
		return attachProcess(process);
	else
		return processNotFound;
}

int ProcessManager::createDisabledProcess(PacketPtr packet)
{
	ProcessPtr process = createProcess(packet);
	if (process)
	{
		int processId = process->getId();

		_createdProcesses[processId] = process;
		return processId;;
	}
	else
		return processNotFound;
}

int ProcessManager::connectProcesses(int sourceId, int targetId)
{
	ProcessPtr source = findProcess(sourceId);
	ProcessPtr target = findProcess(targetId);

	// If either process was NOT created, then fail.
	if (!source || !target)
		return processNotFound;
	else
	{
		// If the source is not reachable, then fail.
		if (!source->isReachable())
			return processNotFound;

		ProcessList::iterator targetIt = std::find_if(
			_enabledProcesses.begin(), 
			_enabledProcesses.end(), 
			std::bind2nd(ProcessSearch<ProcessPtr, int>(), targetId));

		// If the target is already enabled, then fail.
		if (targetIt != _enabledProcesses.end())
			return processNotFound;

		// By here, the source and target are both valid pointers, 
		// the source is enabled, and the target is disabled.
		source->setNext(target);
		return sourceId;
	}
}

ProcessPtr ProcessManager::createProcess(PacketPtr packet)
{
	std::string packetStr = packet->getString();

	ProcessPtr process;
	ProcessConstructionMap::iterator it = _processConstructors.find(packetStr);
	if (it != _processConstructors.end())
		process.reset((*it->second)());
	else
	{
		ERRORMSG("Unknown process type.");
		return ProcessPtr();
	}

	process->readFromPacket(packet);
	return process;
}

void ProcessManager::timepass()
{
	for (ProcessList::iterator it = _enabledProcesses.begin();
		 it != _enabledProcesses.end(); it++)
	{
		// Skip over and get rid of null processes.
		if (!*it)
		{
			it = _enabledProcesses.erase(it);
			if (_enabledProcesses.empty())
				return;
		}

		ProcessPtr currProcess = *it;
		currProcess->timepass();

		Process::removal_status removalStatus = currProcess->_removalStatus;
		int processId = currProcess->getId();
		if (removalStatus == Process::removal_status_removeself)
		{
			*it = currProcess->_nextProcess;
			currProcess->setNext(ProcessPtr());
			_createdProcesses.erase(processId);
		}
		else if (removalStatus == Process::removal_status_removeall)
		{
			it = _enabledProcesses.erase(it--);
			_createdProcesses.erase(processId);
		}
	}
}

void ProcessManager::clear()
{
	_enabledProcesses.clear();
	_createdProcesses.clear();
}

ProcessPtr ProcessManager::findProcess(int processId)
{
	ProcessMap::iterator it = _createdProcesses.find(processId);
	if (it != _createdProcesses.end())
		return it->second;
	else
		return ProcessPtr();
}