#ifndef PARTICLEEMITTER_HPP
#define PARTICLEEMITTER_HPP

//
// Base class for a particle system, adapted from Chapter 14
// of Introduction to Game Programming with DirectX 9.0, by Frank Luna.
//

#include <d3dx9.h>
#include <list>
#include <utility>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "eventlistener.hpp"
#include "boundingvolume.hpp"
#include "texture.hpp"

class Entity;
typedef boost::shared_ptr<Entity> EntityPtr;

struct Particle
{
	D3DXVECTOR3 _position;
	DWORD _color;
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};

struct ParticleAttribute
{
	ParticleAttribute()
		: _lifeTime(0.0f), _age(0.0f), _alive(true)
	{}

	D3DXVECTOR3 _position;
	D3DXVECTOR3 _velocity;
	D3DXVECTOR3 _acceleration;
	float       _lifeTime;
	float       _age;
	D3DXCOLOR   _color;
	D3DXCOLOR   _fadeColor;
	bool        _alive;
};

class ParticleEmitter;
typedef boost::shared_ptr<ParticleEmitter> ParticleEmitterPtr;

class ParticleEmitter : public EventListener
{
public:
	ParticleEmitter(
		unsigned maxParticles, 
		DWORD vbSize, 
		DWORD vbBatchSize);

	virtual ~ParticleEmitter();

	virtual bool initialize(const std::string &textureFilename);
	void reset();

	void timepass();
	void render();
	bool isEmpty() const;

	void addEmitterPosition(const D3DXVECTOR3 &position, unsigned particleDensity);
	void addEmitterEntity(const EntityPtr &entity, unsigned particleDensity);

protected:
	// Randomly jitters (rotates) a vector.  
	// toJitter is assumed to be normalized!
	// minPitch/maxPitch specify the amount by which you can jitter up and down.
	// minYaw/maxYaw specify the amount by which you can jitter left or right.
	static void jitterVector(
		D3DXVECTOR3 &toJitter, 
		float minPitch, float maxPitch,
		float minYaw, float maxYaw);

	virtual void resetParticle(ParticleAttribute *attribute) = 0;
	void addParticles(unsigned numParticles);
	void processDeadParticles();

	void updateEmitterPositions();
	void updateEmitterEntities();

	bool _updatingEmitterPositions;
	bool _updatingEmitterEntities;
	D3DXVECTOR3 _origin;
	float       _size;
	unsigned    _maxParticles;

	typedef std::list<ParticleAttribute> ParticleList;
	ParticleList _particles;

	typedef std::vector<std::pair<D3DXVECTOR3, unsigned> > EmitterPositionList;
	EmitterPositionList _emitterPositions;

	typedef std::vector<std::pair<EntityPtr, unsigned> > EmitterEntityList;
	EmitterEntityList _emitterEntities;

	TexturePtr _texture;

	IDirect3DVertexBuffer9 *_vertexBuffer;

	// These members are used to draw the particles in batches for efficiency.
	DWORD _vbSize;
	DWORD _vbOffset;
	DWORD _vbBatchSize;
};

#endif