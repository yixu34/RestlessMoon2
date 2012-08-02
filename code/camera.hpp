#ifndef CAMERA_HPP
#define CAMERA_HPP

//
// 3D camera class.  Paraphrased from Chapter 12 of:
// Introduction to 3D Game Programming with DirectX 9.0, by Frank Luna.
//

#include <d3dx9.h>
#include <boost/shared_ptr.hpp>

#include "eventlistener.hpp"
#include "controllable.hpp"
#include "entity.hpp"

class Camera;
typedef boost::shared_ptr<Camera> CameraPtr;

class Camera : public Controllable, public EventListener
{
public:
	enum camera_type
	{ 
		camera_type_land_object,
		camera_type_aircraft,
	};

	explicit Camera();
	~Camera();

	void registerHandlers();

    void setType(camera_type type);
	const D3DXMATRIX &getViewMatrix();

    void attachToEntity(const EntityPtr &entity);
    void timepass();
    void updateMovement();

	const D3DXVECTOR3 &getPosition() const;
	const D3DXVECTOR3 &getLookVector() const;
	const D3DXVECTOR3 &getUpVector() const;

private:
	bool onShakeScreen(const Event &event);
	void updateShaking();

	void normalizeAxes();

    void strafe(float units); // left/right
	void fly(float units);    // up/down
	void walk(float units);   // forward/backward
	
	void pitch(float angle); // rotate on right vector
	void yaw(float angle);   // rotate on up vector
	void roll(float angle);  // rotate on look vector

    EntityPtr _attachedEntity;

	camera_type _type;
	D3DXVECTOR3 _right;
	D3DXVECTOR3 _up;
	D3DXVECTOR3 _look;
	D3DXVECTOR3 _pos;
	D3DXMATRIX _viewMatrix;

	float _shakeStrength;
	float _shakeTimer;
	float _shakeDuration;
};

extern CameraPtr camera;

#endif
