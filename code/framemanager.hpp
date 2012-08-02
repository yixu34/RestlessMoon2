#ifndef FRAMEMANAGER_HPP
#define FRAMEMANAGER_HPP

//
// Manages transitions between game screens, and directs input to the 
// appropriate screen.
//

#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include <string>

#include "inputhandler.hpp"
#include "eventlistener.hpp"

class Frame;
class FrameManager;

typedef boost::shared_ptr<Frame> FramePtr;
typedef boost::shared_ptr<FrameManager> FrameManagerPtr;

class FrameManager : public InputHandler, public EventListener
{
public:
    FrameManager();
    ~FrameManager();

	void addFrame(const std::string &frameName, int priority);
	void addFrame(const FramePtr &frame);
    FramePtr getFrame(const std::string &frameName) const;

	void setFrameEnabled(const std::string &frameName, bool enabled);

	bool keyDown(keypress key, keymod mod);
    bool keyUp(keypress key, keymod mod);
    bool mouseDown(mouse_button button, float clickX, float clickY);
    bool mouseUp(mouse_button button, float clickX, float clickY);

	void render();
	void timepass();

	void registerHandlers();

private:
	bool onStartGame(const Event &event);

    typedef std::map<std::string, FramePtr> FramePool;
    FramePool _knownFrames;

	typedef std::vector<FramePtr> FrameList;
	FrameList _enabledFrames;
};

extern FrameManagerPtr frameManager;

#endif
