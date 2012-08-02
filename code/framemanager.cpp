#include <algorithm>

#include "common.hpp"
#include "scripting.hpp"
#include "frame.hpp"
#include "menu.hpp"
#include "input.hpp"
#include "framemanager.hpp"

// Comparison functor for frames based on their priorities.
class FrameLess
{
public:
	bool operator()(const FramePtr &leftFrame, const FramePtr &rightFrame) const
	{
		return leftFrame->getPriority() < rightFrame->getPriority();
	}
};

FrameManagerPtr frameManager;

FrameManager::FrameManager()
{
}

FrameManager::~FrameManager()
{
}

void FrameManager::setFrameEnabled(const std::string &frameName, bool enabled)
{
	// Make sure that the frame exists.
	FramePtr frame = getFrame(frameName);
	MYASSERT(frame);

	if (enabled)
	{
		// Also make sure that it isn't already in the enabled list.
		for (FrameList::iterator it = _enabledFrames.begin(); 
			 it != _enabledFrames.end(); it++)
		{
			const FramePtr &currFrame = *it;
			if (currFrame == frame)
				return;
		}

		_enabledFrames.push_back(frame);
	}
	else
	{
		for (FrameList::iterator it = _enabledFrames.begin();
			 it != _enabledFrames.end(); it++)
		{
			const FramePtr &currFrame = *it;
			if (currFrame == frame)
			{
				_enabledFrames.erase(it);
				break;
			}
		}
	}
}

void FrameManager::addFrame(const std::string &frameName, int priority)
{
    if (_knownFrames.find(frameName) == _knownFrames.end())
	{
		_knownFrames[frameName] = FramePtr(new Frame(frameName, priority));
	}
    else
	{
		ERRORMSG("Trying to add a duplicate frame name.");
	}
}

void FrameManager::addFrame(const FramePtr &frame)
{
	const std::string &frameName = frame->getName();
	if (_knownFrames.find(frameName) == _knownFrames.end())
	{
		_knownFrames[frameName] = frame;
	}
	else
	{
		ERRORMSG("Trying to add a duplicate frame name.");
	}
}

FramePtr FrameManager::getFrame(const std::string &frameName) const
{
	FramePool::const_iterator it = _knownFrames.find(frameName);
    if (it != _knownFrames.end())
		return it->second;
	else
        return MenuPtr();
}

bool FrameManager::keyDown(keypress key, keymod mod)
{
	for (FrameList::iterator it = _enabledFrames.begin();
		 it != _enabledFrames.end(); it++)
	{
		const FramePtr &currFrame = *it;
		if (currFrame->keyDown(key, mod))
			return true;
	}

	return false;
}

bool FrameManager::keyUp(keypress key, keymod mod)
{
	for (FrameList::iterator it = _enabledFrames.begin();
		it != _enabledFrames.end(); it++)
	{
		const FramePtr &currFrame = *it;
		if (currFrame->keyUp(key, mod))
			return true;
	}

	return false;
}

bool FrameManager::mouseDown(mouse_button button, float clickX, float clickY)
{
	for (FrameList::iterator it = _enabledFrames.begin();
		it != _enabledFrames.end(); it++)
	{
		const FramePtr &currFrame = *it;
		if (currFrame->mouseDown(button, clickX, clickY))
			return true;
	}

	return false;
}

bool FrameManager::mouseUp(mouse_button button, float clickX, float clickY)
{
	for (FrameList::iterator it = _enabledFrames.begin();
		it != _enabledFrames.end(); it++)
	{
		const FramePtr &currFrame = *it;
		if (currFrame->mouseUp(button, clickX, clickY))
			return true;
	}

    return false;
}

void FrameManager::timepass()
{
	// Sort the frames in non-decreasing order by priority.
	// (A lower priority number means that the frame is more important
	// for consuming input events.)
	std::sort(_enabledFrames.begin(), _enabledFrames.end(), FrameLess());

	bool wasKeyboardSet = false;
	for (FrameList::iterator it = _enabledFrames.begin(); 
		 it != _enabledFrames.end(); it++)
	{
		const FramePtr &currFrame = *it;
		currFrame->timepass();

		if (!wasKeyboardSet && currFrame->isVisible())
		{
			input->setKeyboardExclusive(currFrame->wantsExclusiveKeyboard());
			wasKeyboardSet = true;
		}
	}
}

void FrameManager::render()
{
	// Draw the least important frames first.
	for (FrameList::reverse_iterator it = _enabledFrames.rbegin();
		 it != _enabledFrames.rend(); it++)
	{
		const FramePtr &currFrame = *it;
		currFrame->render();
	}
}

void FrameManager::registerHandlers()
{
	registerHandler(
		EventType(Ev_Start_Game::eventName), 
		EventHandlerFunctorPtr(new BaseEventHandlerFunctor<FrameManager>(this, &FrameManager::onStartGame)));
}

bool FrameManager::onStartGame(const Event &event)
{
	for (FramePool::iterator it = _knownFrames.begin(); 
		 it != _knownFrames.end(); it++)
	{
		const std::string &currName = it->first;
		const FramePtr &currFrame   = it->second;

		if (!(currName == "game" || currName == "console"))
			currFrame->setEnabled(false);
	}

	return false;
}