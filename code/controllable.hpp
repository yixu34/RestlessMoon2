#ifndef CONTROLLABLE_HPP
#define CONTROLLABLE_HPP

//
// Base class for things that can be controlled by input, like the player or
// the camera.
//

#include <boost/shared_ptr.hpp>

class Controllable;
typedef boost::shared_ptr<Controllable> ControllablePtr;

class Controllable
{
public:
	Controllable();
	virtual ~Controllable();

	void walkForward(bool walkingForward);
	void walkBackward(bool walkingBackward);
	void strafeLeft(bool strafingLeft);
	void strafeRight(bool strafingRight);
	void flyUp(bool flyingUp);
	void flyDown(bool flyingDown);

    void dash(bool dashing);

	void yawLeft(bool yawingLeft);
	void yawRight(bool yawingRight);
	void pitchUp(bool pitchingUp);
	void pitchDown(bool pitchingDown);

	virtual void updateMovement() = 0;
    virtual void yaw(float angle) = 0;
    virtual void pitch(float angle) = 0;

    void trackMouse();

protected:
	bool _walkingForward;
	bool _walkingBackward;

	bool _strafingLeft;
	bool _strafingRight;

	bool _flyingUp;
	bool _flyingDown;

	bool _yawingLeft;
	bool _yawingRight;

	bool _rollingLeft;
	bool _rollingRight;

	bool _pitchingUp;
	bool _pitchingDown;

    bool _dashing;
};

#endif
