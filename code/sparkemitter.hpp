#ifndef SPARKEMITTER_HPP
#define SPARKEMITTER_HPP

#include "particleemitter.hpp"

class SparkEmitter : public ParticleEmitter
{
public:
	SparkEmitter(unsigned maxParticles);
	~SparkEmitter();

	void resetParticle(ParticleAttribute *particle);
	void registerHandlers();

private:
	bool onHit(const Event &event);

	D3DXVECTOR3 _hitterVelocity;
};

#endif