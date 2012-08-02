#ifndef EFFECT_HPP
#define EFFECT_HPP

#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <boost/shared_ptr.hpp>

#include "resource.hpp"
#include "texture.hpp"
#include "cubetexture.hpp"
#include "dynamictexture.hpp"

class SkinnedMesh;
class EffectSetting;
class Effect;
typedef boost::shared_ptr<Effect> EffectPtr;

typedef ResourceCache<Effect> EffectCache;
extern EffectCache effectCache;

class Effect : public Resource
{
public:
	Effect();
	~Effect();

	bool setTexture(D3DXHANDLE handle, const TexturePtr &texture);
	bool setCubeTexture(D3DXHANDLE handle, const CubeTexturePtr &cubeTexture);
	bool setDynamicTexture(D3DXHANDLE handle, const DynamicTexturePtr &dynamicTexture);

	bool setInt(D3DXHANDLE handle, int i);
	bool setFloat(D3DXHANDLE handle, float f);
	bool setBool(D3DXHANDLE handle, bool b);

	bool setMatrix(D3DXHANDLE handle, const D3DXMATRIX &matrix);
	bool setMatrixArray(D3DXHANDLE handle, const D3DXMATRIX *matrices, int size);
	bool setVector2(D3DXHANDLE handle, const D3DXVECTOR2 &vec);
	bool setVector3(D3DXHANDLE handle, const D3DXVECTOR3 &vec);
	bool setVector4(D3DXHANDLE handle, const D3DXVECTOR4 &vec);

	bool setTechnique(D3DXHANDLE handle);

	bool begin();
	bool end();

	/*void enable();
	static void disableCurrent();*/

	void unload();

	bool operator<(const Effect &other) const;

	inline ID3DXEffect *getData() const { return _effectData; }

private:
	bool loadData(const std::string &filename);

	friend SkinnedMesh;
	friend EffectSetting;

	//static Effect *currentEffect;

	ID3DXEffect *_effectData;
};

//////////////////////////////////////////////////////////////////////////

//
// Comparison functor for EffectPtr's.
//
class EffectPtrLess
{
public:
	bool operator()(const EffectPtr &left, const EffectPtr &right) const
	{
		return (*left) < (*right);
	}
};

#endif