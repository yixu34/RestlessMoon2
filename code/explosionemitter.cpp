#include "explosionemitter.hpp"
#include "color.hpp"
#include "timer.hpp"
#include "event.hpp"
#include "util.hpp"

ExplosionEmitter::ExplosionEmitter(unsigned maxParticles)
	: ParticleEmitter(maxParticles, 2048, 512)
{
}

ExplosionEmitter::~ExplosionEmitter()
{
}

void ExplosionEmitter::resetParticle(ParticleAttribute *particle)
{
	particle->_position = _origin;
	D3DXVECTOR3 velocity(0.0f, 0.0f, 1.0f);

	ParticleEmitter::jitterVector(
		velocity, 
		(-D3DX_PI / 2) + util::EPSILON, (D3DX_PI / 2) - util::EPSILON, 
		0.0f, (2 * D3DX_PI) - util::EPSILON);

	velocity *= std::pow(util::randFloat(00.0f, 1.0f),3)*1000;

	particle->_velocity = velocity;
	particle->_acceleration = D3DXVECTOR3(0.0f, -100.0f, 0.0f);

	particle->_alive    = true;
	particle->_age      = 0.0f;
	particle->_lifeTime = 1.0f;
	particle->_color    = D3DCOLOR_RGBA(255, 255, 255, 0);
}

void ExplosionEmitter::registerHandlers()
{
	registerHandler(
		EventType(Ev_Entity_Die::eventName), 
		EventHandlerFunctorPtr(new BaseEventHandlerFunctor<ExplosionEmitter>(this, &ExplosionEmitter::onDeath)));
}

bool ExplosionEmitter::onDeath(const Event &event)
{
	EvEntityDie_data *eventData = event.getRawData<EvEntityDie_data>();
	Entity *entityToDie   = eventData->_entityToDie; 
	D3DXVECTOR3 position  = eventData->_deathPosition;

	// If you are a projectile that does radial damage, or if you
	// are an entity who is supposed to explode when dying, (e.g. if 
	// you are an airship) then play an explosion!
	if (entityToDie->getRadialDamage() > 0 || 
		entityToDie->getDeathMode() == Entity::death_explodes)
	{
		//// Center the explosion at the entity's bounding box's center.
		//const BoundingBox &boundingBox = entityToDie->getBoundingBox();
		//D3DXVECTOR3 boundingBoxCenter((boundingBox.min + boundingBox.max) / 2 + position);

		//_origin = boundingBoxCenter;
		_origin = position;
		addParticles(400);
	}

	return false;
}
