#ifndef MISSILETRAILEMITTER_HPP
#define MISSILETRAILEMITTER_HPP

#include "particleemitter.hpp"

class MissileTrailEmitter : public ParticleEmitter
{
public:
	MissileTrailEmitter(unsigned maxParticles);
	~MissileTrailEmitter();

	void resetParticle(ParticleAttribute *particle);
	void registerHandlers();
};

#endif