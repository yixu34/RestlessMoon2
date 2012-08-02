#include "scripting.hpp"
#include "renderer.hpp"
#include "common.hpp"
#include "graphics.hpp"
#include "entity.hpp"
#include "staticmesh.hpp"
#include "eventmanager.hpp"
#include "explosionemitter.hpp"
#include "bloodemitter.hpp"
#include "sparkemitter.hpp"
#include "missiletrailemitter.hpp"

RendererPtr renderer;

Renderer::Renderer()
{
	_currPass = 0;
	_savingRenderTarget = false;
	_savedRenderTarget = 0;
	_savedDepthStencil = 0;
	// Init fullscreen quad
	_screenSpaceQuad = new float[20];
}

Renderer::~Renderer()
{
	delete [] _screenSpaceQuad;
}

bool Renderer::initialize()
{
	//
	// Put all of the emitters into the map...
	//

	// Explosions.
	ParticleEmitterPtr explosionEmitter(new ExplosionEmitter(2000));
	if (!explosionEmitter->initialize("media/tex_sfx_fire.jpg"))
		return false;
	_emitters["explosions"] = explosionEmitter;

	// Blood.
	ParticleEmitterPtr bloodEmitter(new BloodEmitter(2000));
	if (!bloodEmitter->initialize("media/tex_sfx_fire.jpg"))
		return false;
	_emitters["blood"] = bloodEmitter;

	// Sparks.
	ParticleEmitterPtr sparkEmitter(new SparkEmitter(2000));
	if (!sparkEmitter->initialize("media/tex_sfx_fire.jpg"))
		return false;
	_emitters["sparks"] = sparkEmitter;

	// Missile trails.
	ParticleEmitterPtr missileTrailEmitter(new MissileTrailEmitter(2000));
	if (!missileTrailEmitter->initialize("media/tex_sfx_fire.jpg"))
		return false;
	_emitters["missiles"] = missileTrailEmitter;

	return true;
}

ParticleEmitterPtr Renderer::getParticleEmitter(const std::string &emitterName) const
{
	ParticleEmitterMap::const_iterator it = _emitters.find(emitterName);
	if (it != _emitters.end())
		return it->second;
	else
		return ParticleEmitterPtr();
}

void Renderer::addRenderable(
	const RenderablePtr &renderable, 
	const std::string &layerName, 
	Entity *entity)
{
	ASSERTMSG(renderable, "Null renderable");

	if (_objectLayers.find(layerName) == _objectLayers.end())
	{
		// This layer has not been seen before; create an empty set.
		EntityList entities;
		entities.insert(entity);
		_objectLayers[layerName] = std::make_pair(renderable, entities);
	}
	else
	{
		// Make sure that the old renderable for this layer matches...
		const RenderablePtr &oldRenderable = _objectLayers[layerName].first;
		EntityList &entList                = _objectLayers[layerName].second;

		if (oldRenderable != renderable)
		{
			ERRORMSG("SHIIIITTTT!");
			return;
		}

		entList.insert(entity);
	}
}

void Renderer::bakePlanespace(const std::string &layerName)
{
	if (_objectLayers.find(layerName) == _objectLayers.end())
	{
		ERRORMSG("Cannot bake planespace texture coordinates to layer, it does not exist!");
	}
	else
	{
		const RenderablePtr &renderable = _objectLayers[layerName].first;
		StaticMesh *staticMesh = dynamic_cast<StaticMesh*>(renderable.get());

		staticMesh->bakePlanespace();
	}
}
		

void Renderer::setWorldMatrix(const D3DXMATRIX &world)
{
	// Loop through all of the parameters in this effect file, and extract
	// -all- world matrices in it.
	D3DXEFFECT_DESC effectDesc;
	_currEffect->getData()->GetDesc(&effectDesc);

	for (int i = 0; i < effectDesc.Parameters; i++)
	{
		D3DXPARAMETER_DESC currDesc;
		D3DXHANDLE currHandle = _currEffect->getData()->GetParameter(0, i);
		_currEffect->getData()->GetParameterDesc(currHandle, &currDesc);

		// Found a world matrix...
		if (currDesc.Semantic != 0 && !strcasecmp(currDesc.Semantic, "world"))
			_currEffect->getData()->SetMatrix(currHandle, &world);
	}

	// Now extract the remaining matrices by their semantics.
	// To be more correct, we'll actually look up the parameter names and
	// store them, rather than using arbitrary names for the matrices.
	D3DXHANDLE viewHandle                = _currEffect->getData()->GetParameterBySemantic(0, "View");
	D3DXHANDLE projectionHandle          = _currEffect->getData()->GetParameterBySemantic(0, "Projection");
	D3DXHANDLE viewProjectionHandle      = _currEffect->getData()->GetParameterBySemantic(0, "ViewProjection");
	D3DXHANDLE worldViewHandle           = _currEffect->getData()->GetParameterBySemantic(0, "WorldView");
	D3DXHANDLE worldViewProjectionHandle = _currEffect->getData()->GetParameterBySemantic(0, "WorldViewProjection");
	
	if (viewHandle != 0)
	{
		D3DXPARAMETER_DESC viewDesc;
		_currEffect->getData()->GetParameterDesc(viewHandle, &viewDesc);
		MYASSERT(viewDesc.Name != 0);

		D3DXMATRIX viewMatrix;
		graphics->getDevice()->GetTransform(D3DTS_VIEW, &viewMatrix);
		_currEffect->getData()->SetMatrix(viewHandle, &viewMatrix);
	}
	if (projectionHandle != 0)
	{
		D3DXPARAMETER_DESC projectionDesc;
		_currEffect->getData()->GetParameterDesc(projectionHandle, &projectionDesc);
		MYASSERT(projectionDesc.Name != 0);

		D3DXMATRIX projectionMatrix;
		graphics->getDevice()->GetTransform(D3DTS_PROJECTION, &projectionMatrix);
		_currEffect->getData()->SetMatrix(projectionHandle, &projectionMatrix);
	}
	if (worldViewHandle != 0)
	{
		D3DXPARAMETER_DESC worldViewDesc;
		_currEffect->getData()->GetParameterDesc(worldViewHandle, &worldViewDesc);
		MYASSERT(worldViewDesc.Name != 0);

		D3DXMATRIX viewMatrix;
		D3DXMATRIX worldViewMatrix;
		graphics->getDevice()->GetTransform(D3DTS_VIEW, &viewMatrix);

		worldViewMatrix = world * viewMatrix;
		_currEffect->getData()->SetMatrix(worldViewHandle, &worldViewMatrix);
	}
	if (viewProjectionHandle != 0)
	{
		D3DXPARAMETER_DESC viewProjectionDesc;
		_currEffect->getData()->GetParameterDesc(
			viewProjectionHandle, 
			&viewProjectionDesc);
		MYASSERT(viewProjectionDesc.Name);

		D3DXMATRIX viewMatrix;
		D3DXMATRIX projectionMatrix;
		D3DXMATRIX viewProjectionMatrix;
		graphics->getDevice()->GetTransform(D3DTS_VIEW, &viewMatrix);
		graphics->getDevice()->GetTransform(D3DTS_PROJECTION, &projectionMatrix);

		viewProjectionMatrix = viewMatrix * projectionMatrix;
		HRESULT hr = _currEffect->getData()->SetMatrix(
			viewProjectionHandle, 
			&viewProjectionMatrix);
	}
	if (worldViewProjectionHandle != 0)
	{
		D3DXPARAMETER_DESC worldViewProjectionDesc;
		_currEffect->getData()->GetParameterDesc(
			worldViewProjectionHandle, 
			&worldViewProjectionDesc);
		MYASSERT(worldViewProjectionDesc.Name);

		D3DXMATRIX viewMatrix;
		D3DXMATRIX projectionMatrix;
		D3DXMATRIX worldViewProjectionMatrix;
		graphics->getDevice()->GetTransform(D3DTS_VIEW, &viewMatrix);
		graphics->getDevice()->GetTransform(D3DTS_PROJECTION, &projectionMatrix);

		worldViewProjectionMatrix = world * viewMatrix * projectionMatrix;
		HRESULT hr = _currEffect->getData()->SetMatrix(
			worldViewProjectionHandle,
			&worldViewProjectionMatrix);

		MYASSERT(SUCCEEDED(hr));
	}
//	_currEffect->setMatrix(_worldMatrixName.c_str(), world);
}

void Renderer::setBoneMatricesName(const std::string &boneMatricesName)
{
	_boneMatricesName = boneMatricesName;
}

void Renderer::setEffectViewInverse(const std::string &viewInverseName)
{
	D3DXMATRIX view, viewInverse;
	graphics->getDevice()->GetTransform(D3DTS_VIEW,&view);
	D3DXMatrixInverse(&viewInverse,NULL,&view);
	_currEffect->setMatrix(viewInverseName.c_str(),viewInverse);
}

// Your effect should turn depth writes off if the texture has
//   no depth stencil attached!
void Renderer::setRenderTarget(DynamicTexturePtr dt)
{
	if(!_savingRenderTarget)
	{
		MYASSERT(!(_savedRenderTarget || _savedDepthStencil));

		if(SUCCEEDED(graphics->getDevice()->GetRenderTarget(0,&_savedRenderTarget)) &&
		   SUCCEEDED(graphics->getDevice()->GetDepthStencilSurface(&_savedDepthStencil)))
		{
			_savingRenderTarget = true;
		} else {
			ERRORMSG("Failed to save current render target and depth buffer!");
		}
	}
	dt->setAsRenderTarget();
}

void Renderer::restoreScreen()
{
	if(_savingRenderTarget)
	{
		MYASSERT(_savedRenderTarget && _savedDepthStencil);
		graphics->getDevice()->SetRenderTarget(0,_savedRenderTarget);
		graphics->getDevice()->SetDepthStencilSurface(_savedDepthStencil);
		util::release(_savedRenderTarget);
		util::release(_savedDepthStencil);
		_savingRenderTarget = false;
	}
}

void Renderer::beginScene()
{
	graphics->getDevice()->BeginScene();
}

void Renderer::endScene()
{
	graphics->getDevice()->EndScene();
}

void Renderer::present()
{
	graphics->getDevice()->Present(0,0,0,0);
}

void Renderer::setBoneMatrices(const D3DXMATRIX *boneMatrices, int size)
{
	_currEffect->getData()->EndPass();
	_currEffect->setMatrixArray(_boneMatricesName.c_str(), boneMatrices, size);
	_currEffect->getData()->BeginPass(_currPass);
}

void Renderer::setRenderScript(const std::string &filename)
{
	_renderScript = filename;
	scripting->executeFile(filename);
}

//void Renderer::render()
//{
//	//ASSERTMSG(_renderScript != "", "No render script specified.");
//	scripting->executeString("doRenderScript()");
//}

void Renderer::timepass()
{
	for (ParticleEmitterMap::iterator it = _emitters.begin(); 
		 it != _emitters.end(); it++)
	{
		ParticleEmitterPtr currEmitter = it->second;
		currEmitter->timepass();
	}

	for (ObjectLayerMap::iterator it = _objectLayers.begin();
		 it != _objectLayers.end(); it++)
	{
		const RenderablePtr &currRenderable = it->second.first;
		currRenderable->timepass();
	}
}

void Renderer::renderObjectLayer(const std::string &layerName)
{
	renderObjectLayerSubset(layerName, 0);
}

void Renderer::renderParticleLayer(const std::string &layerName)
{
	ParticleEmitterMap::iterator it = _emitters.find(layerName);
	if (it != _emitters.end())
	{
		ParticleEmitterPtr emitter = it->second;
		emitter->render();
	}
}

void Renderer::renderObjectLayerSubset(const std::string &layerName, int subset)
{
	ObjectLayerMap::iterator layerIt = _objectLayers.find(layerName);
	if (layerIt != _objectLayers.end())
	{
		const RenderablePtr &currRenderable = _objectLayers[layerName].first;
		const EntityList &entList           = _objectLayers[layerName].second;

		if(!currRenderable)
			return;

		bool isStatic = currRenderable->isStatic();

		// For each entity, set its world matrix transformation and render.
		for (EntityList::const_iterator it = entList.begin();
			it != entList.end(); it++)
		{
			Entity *currEntity = *it;
			if (currEntity != 0)
			{
				_currEffect->getData()->EndPass();
				D3DXMATRIX worldMatrix;
				currEntity->getWorldMatrix(worldMatrix);
				setWorldMatrix(worldMatrix);
				_currEffect->getData()->BeginPass(_currPass);
			}
			else
			{
				_currEffect->getData()->EndPass();
				D3DXMATRIX identityMatrix;
				setWorldMatrix(*(D3DXMatrixIdentity(&identityMatrix)));
				_currEffect->getData()->BeginPass(_currPass);
			}
			if (isStatic)
				currRenderable->renderSubset(subset);
			else
				currEntity->renderInstance(subset);
		}
	}
	else
	{
		// If we can't find this object layer, then maybe it's a particle effect.
		renderParticleLayer(layerName);
	}
}

void Renderer::renderQuad(float x1, float x2, float y1, float y2, float u1, float u2, float v1, float v2) {

	// Top Left Vertex
	_screenSpaceQuad[0] = x1;
	_screenSpaceQuad[1] = y1;
	_screenSpaceQuad[2] = 0;
	_screenSpaceQuad[3] = u1;
	_screenSpaceQuad[4] = v2;

	// Top Right Vertex
	_screenSpaceQuad[5] = x2;
	_screenSpaceQuad[6] = y1;
	_screenSpaceQuad[7] = 0;
	_screenSpaceQuad[8] = u2;
	_screenSpaceQuad[9] = v2;

	// Bottom Right Vertex
	_screenSpaceQuad[10] = x2;
	_screenSpaceQuad[11] = y2;
	_screenSpaceQuad[12] = 0;
	_screenSpaceQuad[13] = u2;
	_screenSpaceQuad[14] = v1;

	// Bottom Left Vertex
	_screenSpaceQuad[15] = x1;
	_screenSpaceQuad[16] = y2;
	_screenSpaceQuad[17] = 0;
	_screenSpaceQuad[18] = u1;
	_screenSpaceQuad[19] = v1;

	graphics->getDevice()->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
	graphics->getDevice()->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,(void*)_screenSpaceQuad,5*sizeof(float));
}

void Renderer::registerHandlers()
{
	registerHandler(
		EventType(Ev_Entity_Die::eventName), 
		EventHandlerFunctorPtr(new BaseEventHandlerFunctor<Renderer>(this, &Renderer::death)));

	//
	// Register event handlers for all of the particle emitters.
	//
	for (ParticleEmitterMap::iterator it = _emitters.begin(); 
		 it != _emitters.end(); it++)
	{
		ParticleEmitterPtr currEmitter = it->second;
		currEmitter->registerHandlers();
	}
}
bool Renderer::death(const Event& deathEvent)
{
	EvEntityDie_data *eventData = deathEvent.getRawData<EvEntityDie_data>();

	Entity *entityToDie          = eventData->_entityToDie; 
	D3DXVECTOR3 _position        = eventData->_deathPosition;
	std::string entityName       = eventData->_entityName;
	Entity::death_mode deathMode = eventData->_deathMode;

	// If this entity should leave a corpse, then don't remove it from
	// the render list, since we want to render the corpse!
	if (deathMode == Entity::death_leaves_corpse)
		return false;

	if(_objectLayers.find(entityName) != _objectLayers.end())
		_objectLayers[entityName].second.erase(entityToDie);
	
	return false;
}

void Renderer::endEffect()
{
	_currEffect->getData()->End();
}

void Renderer::setEffect(const std::string &filename)
{
	_currEffect = effectCache.get(filename);
}

EffectPtr Renderer::getEffect() const
{
	return _currEffect;
}

void Renderer::beginPass(int passNumber)
{
	_currEffect->getData()->BeginPass(passNumber);
	_currPass = passNumber;
}

void Renderer::endPass()
{
	_currEffect->getData()->EndPass();
}

void Renderer::clearAllObjects()
{
	_objectLayers.clear();
	for (ParticleEmitterMap::iterator it = _emitters.begin(); 
		 it != _emitters.end(); it++)
	{
		const ParticleEmitterPtr &currEmitter = it->second;
		currEmitter->reset();
	}
}