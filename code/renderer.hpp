#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <d3dx9.h>
#include <boost/shared_ptr.hpp>
#include <string>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include "eventlistener.hpp"
#include "renderable.hpp"
#include "effect.hpp"
#include "particleemitter.hpp"
#include "util.hpp"

class Renderer;
typedef boost::shared_ptr<Renderer> RendererPtr;

class Entity;
typedef std::set<Entity *> EntityList;

class Renderer: public EventListener
{
public:
	Renderer();
	~Renderer();

	bool initialize();

	void addRenderable(
		const RenderablePtr &renderable, 
		const std::string &layerName, 
		Entity *entity);

	ParticleEmitterPtr getParticleEmitter(const std::string &emitterName) const;

	void setWorldMatrix(const D3DXMATRIX &world);
	void registerHandlers();
	void setBoneMatrices(const D3DXMATRIX *boneMatrices, int size);
	void setBoneMatricesName(const std::string &boneMatricesName);
	void setEffectViewInverse(const std::string &viewInverseName);

	// Pair to change between a texture render target and the screen
	//   as a render target
	void setRenderTarget(DynamicTexturePtr dt);
	void restoreScreen();
	void beginScene();
	void endScene();
	void present();

	void setRenderScript(const std::string &filename);
	void timepass();
	//void render();
	void renderObjectLayer(const std::string &layerName);
	void renderObjectLayerSubset(const std::string &layerName, int subset);
	void bakePlanespace(const std::string &layerName);

	void renderQuad(float x1, float x2, float y1, float y2,
		            float u1 = 0, float u2 = 1, float v1 = 0, float v2 = 1);

	void endEffect();
	void setEffect(const std::string &filename);
	EffectPtr getEffect() const;

	void beginPass(int passNumber);
	void endPass();

	void clearAllObjects();

private:
	bool death(const Event& deathEvent);
	void removeRenderable(int renderableId);

	void renderParticleLayer(const std::string &layerName);

	std::string _boneMatricesName;
	std::string _worldMatrixName;
	std::string _renderScript;

	EffectPtr _currEffect;
	int       _currPass;

	float *_screenSpaceQuad;

	// Ordinary render target stored
	bool _savingRenderTarget;
	IDirect3DSurface9 *_savedRenderTarget;
	IDirect3DSurface9 *_savedDepthStencil;

	typedef std::map<
		std::string, 
		std::pair<RenderablePtr, EntityList>, 
		util::CaseInsensitiveLess> ObjectLayerMap;
	ObjectLayerMap _objectLayers;

	typedef std::map<
		std::string, ParticleEmitterPtr, util::CaseInsensitiveLess> ParticleEmitterMap;
	ParticleEmitterMap _emitters;
};

extern RendererPtr renderer;

#endif