#ifndef EXPLOSIONEMITTER_HPP
#define EXPLOSIONEMITTER_HPP

//
// Particle system for playing explosion effects.
//

#include "particleemitter.hpp"

class ExplosionEmitter : public ParticleEmitter
{
public:
	ExplosionEmitter(unsigned maxParticles);
	~ExplosionEmitter();

	void resetParticle(ParticleAttribute *particle);
	void registerHandlers();

private:
	bool onDeath(const Event &event);

};

#endif