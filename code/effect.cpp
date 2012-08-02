#include "effect.hpp"
#include "util.hpp"
#include "common.hpp"
#include "graphics.hpp"

#include "gameapp.hpp"

//EffectPtr Effect::currentEffect = EffectPtr();
//Effect *Effect::currentEffect = 0;
EffectCache effectCache;

Effect::Effect()
{
	_effectData = 0;
}

Effect::~Effect()
{
	unload();
}

bool Effect::loadData(const std::string &filename)
{
	ID3DXBuffer *errorBuffer = 0;
	HRESULT hr = D3DXCreateEffectFromFile(
		graphics->getDevice(),
		filename.c_str(),
		0,                // no preprocessor definitions
		0,                // no ID3DXInclude interface
		D3DXSHADER_DEBUG, // compile flags
		0,                // don't share parameters
		&_effectData,
		&errorBuffer);

	// output any error messages
	if( errorBuffer )
	{
		log((char*)errorBuffer->GetBufferPointer());
		util::release(errorBuffer);
		gameApp->quit();
		return false;
	}

	return SUCCEEDED(hr);
}

void Effect::unload()
{
	util::release(_effectData);
}

bool Effect::setTexture(D3DXHANDLE handle, const TexturePtr &texture)
{
	return SUCCEEDED(_effectData->SetTexture(handle, texture->_texture));
}

bool Effect::setCubeTexture(D3DXHANDLE handle, const CubeTexturePtr &cubeTexture)
{
	return SUCCEEDED(_effectData->SetTexture(handle,cubeTexture->_cubeTexture));
}

bool Effect::setDynamicTexture(D3DXHANDLE handle, const DynamicTexturePtr &dynamicTexture)
{
	return SUCCEEDED(_effectData->SetTexture(handle,dynamicTexture->_texture));
}

bool Effect::setInt(D3DXHANDLE handle, int i)
{
	return SUCCEEDED(_effectData->SetInt(handle, i));
}

bool Effect::setFloat(D3DXHANDLE handle, float f)
{
	return SUCCEEDED(_effectData->SetFloat(handle, f));
}

bool Effect::setBool(D3DXHANDLE handle, bool b)
{
	return SUCCEEDED(_effectData->SetBool(handle, b));
}

bool Effect::setMatrix(D3DXHANDLE handle, const D3DXMATRIX &matrix)
{
	return SUCCEEDED(_effectData->SetMatrix(handle, &matrix));
}

bool Effect::setMatrixArray(D3DXHANDLE handle, const D3DXMATRIX *matrices, int size)
{
	return SUCCEEDED(_effectData->SetMatrixArray(handle, matrices, size));
}

bool Effect::setVector2(D3DXHANDLE handle, const D3DXVECTOR2 &vec)
{
	return SUCCEEDED(_effectData->SetValue(
		handle, 
		reinterpret_cast<const void *>(&vec), 
		sizeof(D3DXVECTOR2)));
}

bool Effect::setVector3(D3DXHANDLE handle, const D3DXVECTOR3 &vec)
{
	return SUCCEEDED(_effectData->SetValue(
		handle, 
		reinterpret_cast<const void *>(&vec), 
		sizeof(D3DXVECTOR3)));
}

bool Effect::setVector4(D3DXHANDLE handle, const D3DXVECTOR4 &vec)
{
	return SUCCEEDED(_effectData->SetVector(handle, &vec));
}

bool Effect::setTechnique(D3DXHANDLE handle)
{
	return SUCCEEDED(_effectData->SetTechnique(handle));
}

bool Effect::begin()
{
	unsigned ignoredPasses; // ignored!
	return SUCCEEDED(_effectData->Begin(&ignoredPasses, 0));
}

bool Effect::end()
{
	return SUCCEEDED(_effectData->End());
}

bool Effect::operator<(const Effect &other) const
{
	return _lastFilename < other._lastFilename;
}
