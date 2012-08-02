#include "controllable.hpp"
#include "input.hpp"
#include "cvar.hpp"

CVar<float> sensitivity("sensitivity", 0.45f);

Controllable::Controllable()
{
	_walkingForward  = false;
	_walkingBackward = false;

	_strafingLeft   = false;
	_strafingRight  = false;

	_flyingUp       = false;
	_flyingDown     = false;

	_yawingLeft     = false;
	_yawingRight    = false;

	_pitchingUp     = false;
	_pitchingDown   = false;

    _dashing        = false;
}

Controllable::~Controllable()
{
}

void Controllable::trackMouse()
{
    float deltaX, deltaY;
    input->getMouseDelta(deltaX, deltaY);

    // Adjust the deltas by the sensitivity.
    deltaX *= sensitivity / 100;
    deltaY *= sensitivity / 100;  // DirectInput maps +y to down in screen coords.

    yaw(deltaX);
    pitch(deltaY);
}

void Controllable::walkForward(bool walkingForward)
{
    _walkingForward = walkingForward;
}

void Controllable::walkBackward(bool walkingBackward)
{
    _walkingBackward = walkingBackward;
}

void Controllable::strafeLeft(bool strafingLeft)
{
    _strafingLeft = strafingLeft;
}

void Controllable::strafeRight(bool strafingRight)
{
    _strafingRight = strafingRight;
}

void Controllable::flyUp(bool flyingUp)
{
    _flyingUp = flyingUp;
}

void Controllable::flyDown(bool flyingDown)
{
    _flyingDown = flyingDown;
}

void Controllable::yawLeft(bool yawingLeft)
{
    _yawingLeft = yawingLeft;
}

void Controllable::yawRight(bool yawingRight)
{
    _yawingRight = yawingRight;
}

void Controllable::pitchUp(bool pitchingUp)
{
    _pitchingUp = pitchingUp;
}

void Controllable::pitchDown(bool pitchingDown)
{
    _pitchingDown = pitchingDown;
}
void Controllable::dash(bool dashing)
{
    _dashing = dashing;
}