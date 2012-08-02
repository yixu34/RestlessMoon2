#include "sparkemitter.hpp"
#include "event.hpp"
#include "entity.hpp"
#include "cvar.hpp"

CVar<float> sparkDefaultVelocityY("sparkDefaultVelocityY", -1.0f);

SparkEmitter::SparkEmitter(unsigned maxParticles)
	: ParticleEmitter(maxParticles, 2048, 512)
{
	_hitterVelocity = D3DXVECTOR3(0.0f, sparkDefaultVelocityY, 0.0f);
}

SparkEmitter::~SparkEmitter()
{
}

void SparkEmitter::resetParticle(ParticleAttribute *particle)
{
	particle->_alive = true;
	particle->_position = _origin;

	if (_updatingEmitterPositions)
	{
		particle->_velocity.x = util::randFloat(-2.0f, 2.0f);
		particle->_velocity.y = -sparkDefaultVelocityY;
		particle->_velocity.z = util::randFloat(-2.0f, 2.0f);
		particle->_acceleration = D3DXVECTOR3(0, -1200, 0);
	}
	else if (_updatingEmitterEntities)
	{
		particle->_velocity.x = util::randFloat(-2.0f, 2.0f);
		particle->_velocity.y = -sparkDefaultVelocityY;
		particle->_velocity.z = util::randFloat(-2.0f, 2.0f);
		particle->_acceleration = D3DXVECTOR3(0, 0, 0);
	}
	else
	{
		particle->_velocity = -_hitterVelocity;
		particle->_velocity.y = 0;
		particle->_acceleration = D3DXVECTOR3(0, -1200, 0);
	}

	// Jitter the velocity a bit, and then scale its magnitude.
	D3DXVec3Normalize(&particle->_velocity, &particle->_velocity);
	ParticleEmitter::jitterVector(
		particle->_velocity, 
		-0.25f, 0.25f, 
		-0.25f, 0.25f);
	float distributor = util::randFloat(0,1);
	particle->_velocity *= distributor*distributor*distributor*distributor*600.0f;

	particle->_age      = 0.0f;
	particle->_lifeTime = 1.0f;
	particle->_color    = D3DCOLOR_RGBA(255, 200, 128, 255);
}

void SparkEmitter::registerHandlers()
{
	registerHandler(
		EventType(Ev_Entity_Hit::eventName), 
		EventHandlerFunctorPtr(new BaseEventHandlerFunctor<SparkEmitter>(this, &SparkEmitter::onHit)));
}

bool SparkEmitter::onHit(const Event &event)
{
	EvEntityHit_data *eventData = event.getRawData<EvEntityHit_data>();
	_origin         = eventData->_projectilePosition;
	_hitterVelocity = eventData->_projectileVelocity;

	if (eventData->_onHitMode == Entity::on_hit_sparks)
	{
		addParticles(200);
	}

	// Reset the hitter velocity to the default value, so that continuously
	// emitted particles look decent.
	_hitterVelocity = D3DXVECTOR3(0.0f, sparkDefaultVelocityY, 0.0f);

	return false;
}