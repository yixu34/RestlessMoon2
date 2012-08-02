#include "missiletrailemitter.hpp"
#include "util.hpp"

MissileTrailEmitter::MissileTrailEmitter(unsigned maxParticles)
	: ParticleEmitter(maxParticles, 2048, 512)
{
}

MissileTrailEmitter::~MissileTrailEmitter()
{
}

void MissileTrailEmitter::resetParticle(ParticleAttribute *particle)
{
	particle->_position = _origin;
	D3DXVECTOR3 velocity(0.0f, 0.0f, 1.0f);

	ParticleEmitter::jitterVector(
		velocity, 
		(-D3DX_PI / 2) + util::EPSILON, (D3DX_PI / 2) - util::EPSILON, 
		0.0f, (2 * D3DX_PI) - util::EPSILON);

	velocity *= std::pow(util::randFloat(00.0f, 1.0f),3)*1000;

	particle->_velocity = velocity;
	particle->_acceleration = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	particle->_alive    = true;
	particle->_age      = 0.0f;
	particle->_lifeTime = 1.0f;

	/*float greyChance = util::randFloat(0.0f, 1.0f);
	if (greyChance <= 0.33f)
		particle->_color = D3DCOLOR_RGBA(64, 64, 64, static_cast<unsigned char>(255 * util::randFloat(0.5f, 1.0f)));
	else*/
	particle->_color = D3DCOLOR_RGBA(255, static_cast<unsigned char>(255 * util::randFloat(0.0f, 1.0f)), 0, 255);
}

void MissileTrailEmitter::registerHandlers()
{
}
