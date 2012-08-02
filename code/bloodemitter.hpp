#ifndef BLOODEMITTER_HPP
#define BLOODEMITTER_HPP

#include <d3dx9.h>

#include "particleemitter.hpp"

class BloodEmitter : public ParticleEmitter
{
public:
	BloodEmitter(unsigned maxParticles);
	~BloodEmitter();

	void resetParticle(ParticleAttribute *particle);
	void registerHandlers();

private:
	bool onHit(const Event &event);
	void setOnHitMode(Entity::on_hit_mode onHitMode);

	// The velocity of the projectile that hit you.
	D3DXVECTOR3 _hitterVelocity;
	Entity::on_hit_mode _onHitMode;
};

#endif