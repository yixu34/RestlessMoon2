#include "bloodemitter.hpp"
#include "event.hpp"
#include "entity.hpp"

BloodEmitter::BloodEmitter(unsigned maxParticles)
	: ParticleEmitter(maxParticles, 2048, 512)
{
	setOnHitMode(Entity::on_hit_nothing);
	_hitterVelocity = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
}

BloodEmitter::~BloodEmitter()
{
}

void BloodEmitter::resetParticle(ParticleAttribute *particle)
{
	MYASSERT(_onHitMode != Entity::on_hit_nothing);

	particle->_alive = true;
	particle->_position = _origin;
	particle->_velocity = -_hitterVelocity;
	particle->_acceleration = D3DXVECTOR3(0, 0, 0);

	// Jitter the velocity a bit, and then scale its magnitude.
	D3DXVec3Normalize(&particle->_velocity, &particle->_velocity);
	ParticleEmitter::jitterVector(
		particle->_velocity, 
		-0.25f, 0.25f, 
		-0.25f, 0.25f);
	particle->_velocity *= std::pow(util::randFloat(0.0f, 1.0f),3)*800;

	particle->_age      = 0.0f;
	particle->_lifeTime = 1.0f;

	switch (_onHitMode)
	{
	case Entity::on_hit_blood_red:
		particle->_color = D3DCOLOR_RGBA(255, 0, 0, 255);
		break;

	case Entity::on_hit_blood_green:
		particle->_color = D3DCOLOR_RGBA(0, 255, 0, 255);
		break;

	default:
		particle->_color = D3DCOLOR_RGBA(255, 0, 0, 255);
		break;
	};
}

void BloodEmitter::registerHandlers()
{
	registerHandler(
		EventType(Ev_Entity_Hit::eventName), 
		EventHandlerFunctorPtr(new BaseEventHandlerFunctor<BloodEmitter>(this, &BloodEmitter::onHit)));
}

bool BloodEmitter::onHit(const Event &event)
{
	EvEntityHit_data *eventData = event.getRawData<EvEntityHit_data>();
	_origin         = eventData->_projectilePosition;
	_hitterVelocity = eventData->_projectileVelocity;

	setOnHitMode(eventData->_onHitMode);

	if (_onHitMode != Entity::on_hit_nothing && _onHitMode != Entity::on_hit_sparks)
	{
		addParticles(100);
	}

	return false;
}

void BloodEmitter::setOnHitMode(Entity::on_hit_mode onHitMode)
{
	_onHitMode = onHitMode;
}