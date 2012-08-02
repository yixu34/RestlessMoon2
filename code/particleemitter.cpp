#include <algorithm>

#include "particleemitter.hpp"
#include "common.hpp"
#include "util.hpp"
#include "graphics.hpp"

ParticleEmitter::ParticleEmitter(
	unsigned maxParticles, 
	DWORD vbSize, 
	DWORD vbBatchSize)
{
	//_emitRate     = 0.0f;
	_size         = 0.0f;
	_maxParticles = maxParticles;

	_vertexBuffer = 0;
	_vbSize       = vbSize;
	_vbOffset     = 0;
	_vbBatchSize  = vbBatchSize;

	_updatingEmitterEntities  = false;
	_updatingEmitterPositions = false;
}

ParticleEmitter::~ParticleEmitter()
{
	util::release(_vertexBuffer);
	_texture->unload();
}

bool ParticleEmitter::initialize(const std::string &textureFilename)
{
	HRESULT vbHr = graphics->getDevice()->CreateVertexBuffer(
											_vbSize * sizeof(Particle), 
											D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY, 
											Particle::FVF, 
											D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with dynamic vertex buffers!
											&_vertexBuffer, 
											0);

	if (FAILED(vbHr))
	{
		ERRORMSG("Could not initialize particle system!");
		return false;
	}

	_texture = textureCache.get(textureFilename);
	MYASSERT(_texture);

	return true;
}

void ParticleEmitter::reset()
{
	_emitterEntities.clear();
	_emitterPositions.clear();
	_particles.clear();
}

void ParticleEmitter::addParticles(unsigned numParticles)
{
	ParticleAttribute newParticle;
	for (unsigned i = 0; i < numParticles; i++)
	{
		resetParticle(&newParticle);
		_particles.push_back(newParticle);
	}
}

void ParticleEmitter::render()
{
	//
	// The render method works by filling a section of the vertex buffer with data,
	// then we render that section.  While that section is rendering we lock a new
	// section and begin to fill that section.  Once that sections filled we render it.
	// This process continues until all the particles have been drawn.  The benefit
	// of this method is that we keep the video card and the CPU busy.  

	if (_particles.empty())
		return;

	//
	// set render states
	//

	//preRender();

	// GODDAMNIT YI NO FF!!!
	//_texture->bind();
	graphics->getDevice()->SetFVF(Particle::FVF);
	graphics->getDevice()->SetStreamSource(0, _vertexBuffer, 0, sizeof(Particle));

	//
	// render batches one by one
	//

	// start at beginning if we're at the end of the vb
	if(_vbOffset >= _vbSize)
		_vbOffset = 0;

	Particle* v = 0;

	_vertexBuffer->Lock(
		_vbOffset    * sizeof( Particle ),
		_vbBatchSize * sizeof( Particle ),
		(void**)&v,
		_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

	DWORD numParticlesInBatch = 0;

	//
	// Until all particles have been rendered.
	//
	std::list<ParticleAttribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		if(!i->_alive)
			continue;

		//
		// Copy a batch of the living particles to the
		// next vertex buffer segment
		//
		v->_position = i->_position;
		float alphaFraction = (1 - (i->_age / i->_lifeTime));
		D3DXCOLOR color(i->_color);
		color *= alphaFraction;
		v->_color = color;//D3DCOLOR_RGBA(alpha, alpha, alpha, alpha);
		v++; // next element;

		numParticlesInBatch++; //increase batch counter

		// if this batch full?
		if(numParticlesInBatch == _vbBatchSize) 
		{
			//
			// Draw the last batch of particles that was
			// copied to the vertex buffer. 
			//
			_vertexBuffer->Unlock();

			graphics->getDevice()->DrawPrimitive(
				D3DPT_POINTLIST,
				_vbOffset,
				_vbBatchSize);

			//
			// While that batch is drawing, start filling the
			// next batch with particles.
			//

			// move the offset to the start of the next batch
			_vbOffset += _vbBatchSize; 

			// don't offset into memory thats outside the vb's range.
			// If we're at the end, start at the beginning.
			if(_vbOffset >= _vbSize) 
				_vbOffset = 0;       

			_vertexBuffer->Lock(
				_vbOffset    * sizeof( Particle ),
				_vbBatchSize * sizeof( Particle ),
				(void**)&v,
				_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

			numParticlesInBatch = 0; // reset for new batch
		}
	}

	_vertexBuffer->Unlock();

	// its possible that the LAST batch being filled never 
	// got rendered because the condition 
	// (numParticlesInBatch == _vbBatchSize) would not have
	// been satisfied.  We draw the last partially filled batch now.

	if (numParticlesInBatch > 0)
	{
		graphics->getDevice()->DrawPrimitive(
			D3DPT_POINTLIST,
			_vbOffset,
			numParticlesInBatch);
	}

	// next block
	_vbOffset += _vbBatchSize; 

	////
	//// reset render states
	////
	//postRender();
}

bool ParticleEmitter::isEmpty() const
{
	return _particles.empty();
}

void ParticleEmitter::processDeadParticles()
{
	for (ParticleList::iterator it = _particles.begin(); it != _particles.end();)
	{
		if (!it->_alive)
			it = _particles.erase(it);
		else
			it++;
	}
}

void ParticleEmitter::timepass()
{
	for (ParticleList::iterator it = _particles.begin(); 
		 it != _particles.end(); it++)
	{
		ParticleAttribute &currParticle = *it;

		float dtSec = getDtSeconds();
		currParticle._velocity += currParticle._acceleration * dtSec;
		currParticle._position += currParticle._velocity * dtSec;
		currParticle._age += dtSec;

		// Flag the particle as dead if it's too old.
		if (currParticle._age > currParticle._lifeTime)
			currParticle._alive = false;
	}

	updateEmitterEntities();
	updateEmitterPositions();

	processDeadParticles();
}

void ParticleEmitter::addEmitterEntity(const EntityPtr &entity, unsigned particleDensity)
{
	_emitterEntities.push_back(std::make_pair(entity, particleDensity));
}

void ParticleEmitter::addEmitterPosition(const D3DXVECTOR3 &position, unsigned particleDensity)
{
	_emitterPositions.push_back(std::make_pair(position, particleDensity));
}

void ParticleEmitter::updateEmitterEntities()
{
	_updatingEmitterEntities = true;
	for (EmitterEntityList::iterator it = _emitterEntities.begin();
		 it != _emitterEntities.end(); )
	{
		const EntityPtr &currEntity = it->first;
		unsigned numParticles       = it->second;

		if (!currEntity || currEntity->isDead())
			it = _emitterEntities.erase(it);
		else
		{
			_origin = currEntity->getPos();
			addParticles(numParticles);
			++it;
		}
	}
	_updatingEmitterEntities = false;
}

void ParticleEmitter::updateEmitterPositions()
{
	_updatingEmitterPositions = true;
	for (unsigned i = 0; i < _emitterPositions.size(); i++)
	{
		const D3DXVECTOR3 &currPosition = _emitterPositions[i].first;
		unsigned numParticles           = _emitterPositions[i].second;

		_origin = currPosition;
		addParticles(numParticles);
	}
	_updatingEmitterPositions = false;
}

void ParticleEmitter::jitterVector(
	D3DXVECTOR3 &toJitter, 
	float minPitch, float maxPitch, 
	float minYaw, float maxYaw)
{
	// Pick a random pitch rotation.
	float pitchAngle = util::randFloat(minPitch, maxPitch);
	D3DXMATRIX pitchMatrix;
	D3DXMatrixRotationX(&pitchMatrix, pitchAngle);
	D3DXVec3TransformCoord(&toJitter, &toJitter, &pitchMatrix);

	// Pick a random yaw rotation.
	float yawAngle = util::randFloat(minYaw, maxYaw);
	D3DXMATRIX yawMatrix;
	D3DXMatrixRotationY(&yawMatrix, yawAngle);
	D3DXVec3TransformCoord(&toJitter, &toJitter, &yawMatrix);
}