#include <cmath>

#include "camera.hpp"
#include "timer.hpp"
#include "command.hpp"
#include "common.hpp"
#include "input.hpp"
#include "util.hpp"
#include "cvar.hpp"

CVar<float> cameraMoveSpeed("cameraMoveSpeed", 10.0f);
CVar<float> cameraTurnSpeed("cameraTurnSpeed", 0.01f);

CameraPtr camera;

Camera::Camera()
{
	//_type = type;
	_type  = camera_type_land_object;

	_pos    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	_right  = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	_up     = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	_look   = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	_walkingForward  = false;
	_walkingBackward = false;

	_strafingLeft  = false;
	_strafingRight = false;

	_flyingUp   = false;
	_flyingDown = false;

	_yawingLeft  = false;
	_yawingRight = false;

	_rollingLeft  = false;
	_rollingRight = false;

	_pitchingUp   = false;
	_pitchingDown = false;

	_shakeStrength = 0.0f;
	_shakeTimer    = 0.0f;
}

Camera::~Camera()
{
}

void Camera::registerHandlers()
{
	registerHandler(
		EventType(Ev_Shake_Screen::eventName), 
		EventHandlerFunctorPtr(new BaseEventHandlerFunctor<Camera>(this, &Camera::onShakeScreen)));
}

//CVar<float> shakeStrength("shakeStrength", 0.15f);
CVar<float> shakeSpeed("shakeSpeed", 50.0f);
//CVar<float> shakeDuration("shakeDuration", 1.2f);

bool Camera::onShakeScreen(const Event &event)
{
	EvShakeScreen_data *shakeData = event.getRawData<EvShakeScreen_data>();
	_shakeStrength = shakeData->_shakeStrength;
	_shakeDuration = shakeData->_shakeTime;
	_shakeTimer    = shakeData->_shakeTime;

	return false;
}

void Camera::updateShaking()
{
	if (_shakeTimer <= 0.0f)
		return;
	
	_shakeTimer -= getDtSeconds();

	float currShakeStrength = _shakeStrength * (_shakeTimer / _shakeDuration);
	float currShakeSpeed    = shakeSpeed * (_shakeTimer / _shakeDuration);

	float dispYaw   = currShakeStrength * sin(_shakeTimer * currShakeSpeed * util::randFloat(0.5f, 1.5f));
	float dispPitch = currShakeStrength * cos(_shakeTimer * currShakeSpeed * util::randFloat(0.5f, 1.5f));

	D3DXMATRIX shakeYaw;
	D3DXMatrixRotationY(&shakeYaw, dispYaw);

	D3DXMATRIX shakePitch;
	D3DXMatrixRotationX(&shakePitch, dispPitch);

	D3DXMatrixMultiply(&_viewMatrix, &shakePitch, &_viewMatrix);
	D3DXMatrixMultiply(&_viewMatrix, &shakeYaw, &_viewMatrix);
}

void Camera::setType(camera_type type)
{
    _type = type;
}

void Camera::walk(float units)
{
	// move only on xz plane for land object
	if( _type == camera_type_land_object )
		_pos += D3DXVECTOR3(_look.x, 0.0f, _look.z) * units;
	else
		_pos += _look * units;
}

void Camera::strafe(float units)
{
	// move only on xz plane for land object
	if( _type == camera_type_land_object )
		_pos += D3DXVECTOR3(_right.x, 0.0f, _right.z) * units;
	else
		_pos += _right * units;
}

void Camera::fly(float units)
{
	// move only on y-axis for land object
	if( _type == camera_type_land_object )
		_pos.y += units;
	else
		_pos += _up * units;
}

void Camera::pitch(float angle)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &_right,	angle);

	// rotate _up and _look around _right vector
	D3DXVec3TransformCoord(&_up,&_up, &T);
	D3DXVec3TransformCoord(&_look,&_look, &T);
}

void Camera::yaw(float angle)
{
	D3DXMATRIX T;

	// Rotate around world y (0, 1, 0) always for land object,
	// rotate around own up vector for aircraft.
	if( _type == camera_type_land_object )
		D3DXMatrixRotationY(&T, angle);
	else
		D3DXMatrixRotationAxis(&T, &_up, angle);

	// rotate _right and _look around _up or y-axis
	D3DXVec3TransformCoord(&_right,&_right, &T);
	D3DXVec3TransformCoord(&_look,&_look, &T);
}

void Camera::roll(float angle)
{
	// only roll for camera_type_aircraft type
	if( _type == camera_type_aircraft )
	{
		D3DXMATRIX T;
		D3DXMatrixRotationAxis(&T, &_look,	angle);

		// rotate _up and _right around _look vector
		D3DXVec3TransformCoord(&_right,&_right, &T);
		D3DXVec3TransformCoord(&_up,&_up, &T);
	}
}

void Camera::normalizeAxes()
{
	// Keep camera's axes orthogonal to each other.
	D3DXVec3Normalize(&_look, &_look);

	D3DXVec3Cross(&_right, &_up, &_look);
	D3DXVec3Normalize(&_right, &_right);

	D3DXVec3Cross(&_up, &_look, &_right);
	D3DXVec3Normalize(&_up, &_up);
}

const D3DXMATRIX &Camera::getViewMatrix()
{
	normalizeAxes();

	// Build the view matrix:
	float x = -D3DXVec3Dot(&_right, &_pos);
	float y = -D3DXVec3Dot(&_up, &_pos);
	float z = -D3DXVec3Dot(&_look, &_pos);

	_viewMatrix(0, 0) = _right.x;
	_viewMatrix(0, 1) = _up.x;
	_viewMatrix(0, 2) = _look.x;
	_viewMatrix(0, 3) = 0.0f;

	_viewMatrix(1, 0) = _right.y;
	_viewMatrix(1, 1) = _up.y;
	_viewMatrix(1, 2) = _look.y;
	_viewMatrix(1, 3) = 0.0f;

	_viewMatrix(2, 0) = _right.z;
	_viewMatrix(2, 1) = _up.z;
	_viewMatrix(2, 2) = _look.z;
	_viewMatrix(2, 3) = 0.0f;

	_viewMatrix(3, 0) = x;
	_viewMatrix(3, 1) = y;
	_viewMatrix(3, 2) = z;
	_viewMatrix(3, 3) = 1.0f;

	updateShaking();

	return _viewMatrix;
}

void Camera::attachToEntity(const EntityPtr &entity)
{
    _attachedEntity = entity;
}

void Camera::timepass()
{
    if (!_attachedEntity)
        return;

	// Move the camera to where the entity is.
	float yawAngle   = _attachedEntity->getYawAngle();
	float pitchAngle = _attachedEntity->getPitchAngle();
	float offsetY, offsetZ;
	_attachedEntity->getCameraOffsets(offsetY, offsetZ);
	_pos    = _attachedEntity->getPos();
	_pos.y += offsetY;

	// Reconstruct the look vector.
	_look = D3DXVECTOR3(0, 0, 1);
	D3DXMATRIX lookTransform;
	D3DXMatrixRotationY(&lookTransform, yawAngle + D3DX_PI);
	D3DXVec3TransformCoord(&_look, &_look, &lookTransform);

	_up = D3DXVECTOR3(0, 1, 0);
	
	// Derive the right vector by crossing (up X look).
	D3DXVec3Cross(&_right, &_up, &_look);

	D3DXMATRIX pitchTransform;
	D3DXMatrixRotationAxis(&pitchTransform, &_right, pitchAngle);

	// Rotate the up and look vectors around the right vector.
	D3DXVec3TransformCoord(&_up, &_up, &pitchTransform);
	D3DXVec3TransformCoord(&_look, &_look, &pitchTransform);

	normalizeAxes();

	// Move the camera behind the player by the offset.
	_pos = _pos - (_look * offsetZ);
}

void Camera::updateMovement()
{
	// If we're following an entity, then just update the camera position to
	// follow that entity.  Don't bother moving the camera by itself.
	if (_attachedEntity)
		return;

    const float moveSpeed = cameraMoveSpeed * getDtMilliseconds();
    const float turnSpeed = cameraTurnSpeed * getDtMilliseconds();

    // Now actually update the camera position based on its control state.
    if (_walkingForward)
        walk(moveSpeed);
    else if (_walkingBackward)
        walk(-moveSpeed);

    if (_strafingLeft)
        strafe(-moveSpeed);
    else if (_strafingRight)
        strafe(moveSpeed);

    if (_flyingUp)
        fly(moveSpeed);
    else if (_flyingDown)
        fly(-moveSpeed);

    if (_yawingLeft)
        yaw(-turnSpeed);
    else if (_yawingRight)
        yaw(turnSpeed);

    if (_rollingLeft)
        roll(-turnSpeed);
    else if (_rollingRight)
        roll(turnSpeed);

    if (_pitchingUp)
        pitch(turnSpeed);
    else if (_pitchingDown)
        pitch(-turnSpeed);

    trackMouse();
}

const D3DXVECTOR3 &Camera::getPosition() const
{
	return _pos;
}

const D3DXVECTOR3 &Camera::getLookVector() const
{
	return _look;
}

const D3DXVECTOR3 &Camera::getUpVector() const
{
	return _up;
}