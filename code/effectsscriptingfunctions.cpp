#include <boost/optional.hpp>

#include "scripting.hpp"
#include "staticmesh.hpp"
#include "effect.hpp"
#include "graphics.hpp"
#include "game.hpp"
#include "renderer.hpp"
#include "common.hpp"
#include "texture.hpp"
#include "kdtree.hpp"
#include "console.hpp"

//////////////////////////////////////////////////////////////////////////
// Effect settings functions.
//////////////////////////////////////////////////////////////////////////

SCRIPTING_FUNCTION(setTexture)
{
	std::string handleName  = scripting->getArgument<std::string>(1);
	std::string textureName = scripting->getArgument<std::string>(2);

	TexturePtr texture = textureCache.get(textureName);
	ASSERTMSG(texture, "Texture is null!");
	
	if (!renderer->getEffect()->setTexture(handleName.c_str(), texture))
		ERRORMSG("Couldn't set texture.");

	return 0;
}

SCRIPTING_FUNCTION(setCubeTexture)
{
	std::string handleName = scripting->getArgument<std::string>(1);
	std::string cubeTextureName = scripting->getArgument<std::string>(2);

	CubeTexturePtr cubeTexture = cubeTextureCache.get(cubeTextureName);
	ASSERTMSG(cubeTexture, "Cube Texture is null!");

	if(!renderer->getEffect()->setCubeTexture(handleName.c_str(), cubeTexture))
		ERRORMSG("Couldn't set cube texture.");

	return 0;
}

SCRIPTING_FUNCTION(setDynamicTexture)
{
	std::string handleName = scripting->getArgument<std::string>(1);
	std::string dynamicTextureName = scripting->getArgument<std::string>(2);

	DynamicTexturePtr dynamicTexture = dynamicTextureCache.get(dynamicTextureName);
	ASSERTMSG(dynamicTexture, "Dynamic Texture is null!");

	if(!renderer->getEffect()->setDynamicTexture(handleName.c_str(), dynamicTexture))
		ERRORMSG("Couldn't set dynamic texture.");

	return 0;
}

SCRIPTING_FUNCTION(setRenderTarget)
{
	std::string renderTargetName = scripting->getArgument<std::string>(1);

	DynamicTexturePtr renderTarget = dynamicTextureCache.get(renderTargetName);

	ASSERTMSG(renderTarget,"Render Target is NULL!");

	renderer->setRenderTarget(renderTarget);

	return 0;
}

SCRIPTING_FUNCTION(restoreScreen)
{
	renderer->restoreScreen();

	return 0;
}

SCRIPTING_FUNCTION(beginScene)
{
	renderer->beginScene();

	return 0;
}

SCRIPTING_FUNCTION(endScene)
{
	renderer->endScene();

	return 0;
}

SCRIPTING_FUNCTION(present)
{
	renderer->present();

	return 0;
}

SCRIPTING_FUNCTION(setInt)
{
	std::string handleName = scripting->getArgument<std::string>(1);
	int i                  = scripting->getArgument<int>(2);

	renderer->getEffect()->setInt(handleName.c_str(), i);

	return 0;
}

SCRIPTING_FUNCTION(setFloat)
{
	std::string handleName = scripting->getArgument<std::string>(1);
	float f                = scripting->getArgument<float>(2);

	renderer->getEffect()->setFloat(handleName.c_str(), f);

	return 0;
}

SCRIPTING_FUNCTION(setBool)
{
	std::string handleName = scripting->getArgument<std::string>(1);
	bool b                 = scripting->getArgument<bool>(2);

	renderer->getEffect()->setBool(handleName.c_str(), b);

	return 0;
}

SCRIPTING_FUNCTION(setMatrix)
{
	// We need 16 floats to construct a 4x4 matrix, plus the variable name.
	if (scripting->getNumArgs() < 17)
	{
		ERRORMSG("Insufficient arguments to setMatrix().");
		return 0;
	}

	std::string handleName = scripting->getArgument<std::string>(1);

	D3DXMATRIX matrix;
	for (int i = 0; i < 16; i++)
	{
		const int scriptIndex = 2 + i;
		float currElement = scripting->getArgument<float>(scriptIndex);

		// D3D matrices use 1's notation.
		int matrixRow = (i / 4) + 1;
		int matrixCol = (i % 4) + 1;

		matrix(matrixRow, matrixCol) = currElement;
	}

	renderer->getEffect()->setMatrix(handleName.c_str(), matrix);

	return 0;
}

SCRIPTING_FUNCTION(setVector)
{
	const int numVectorComponents = scripting->getNumArgs() - 1;

	std::string handleName = scripting->getArgument<std::string>(1);
	
	if (numVectorComponents >= 4)
	{
		// Enough args for a 4-vector...
		float x = scripting->getArgument<float>(2);
		float y = scripting->getArgument<float>(3);
		float z = scripting->getArgument<float>(4);
		float w = scripting->getArgument<float>(5);

		renderer->getEffect()->setVector4(handleName.c_str(), D3DXVECTOR4(x, y, z, w));
	}
	else if (numVectorComponents >= 3)
	{
		 // 3-vector
		float x = scripting->getArgument<float>(2);
		float y = scripting->getArgument<float>(3);
		float z = scripting->getArgument<float>(4);

		renderer->getEffect()->setVector3(handleName.c_str(), D3DXVECTOR3(x, y, z));
	}
	else if (numVectorComponents >= 2)
	{
		 // 2-vector
		float x = scripting->getArgument<float>(2);
		float y = scripting->getArgument<float>(3);

		renderer->getEffect()->setVector2(handleName.c_str(), D3DXVECTOR2(x, y));
	}
	else 
	{
		// insufficient args.
		ERRORMSG("Error:  Insufficient vector components.");
	}

	return 0;
}

SCRIPTING_FUNCTION(setTechnique)
{
	std::string techniqueName = scripting->getArgument<std::string>(1);

	renderer->getEffect()->setTechnique(techniqueName.c_str());

	return 0;
}

SCRIPTING_FUNCTION(setEffect)
{
	std::string effectName = scripting->getArgument<std::string>(1);
	renderer->setEffect(effectName);

	return 0;
}

SCRIPTING_FUNCTION(beginEffect)
{
	renderer->getEffect()->begin();

	return 0;
}

SCRIPTING_FUNCTION(endEffect)
{
	renderer->getEffect()->end();

	return 0;
}

SCRIPTING_FUNCTION(clearZBuffer)
{
	graphics->getDevice()->Clear(0, 0, D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	return 0;
}

SCRIPTING_FUNCTION(clearColorBuffer)
{
	int numArgs = scripting->getNumArgs();

	if(numArgs == 4) {
		float r = scripting->getArgument<float>(1);
		float g = scripting->getArgument<float>(2);
		float b = scripting->getArgument<float>(3);
		float a = scripting->getArgument<float>(4);
		UINT clearColor = D3DCOLOR_COLORVALUE(r,g,b,a);
		graphics->getDevice()->Clear(0, 0, D3DCLEAR_TARGET, clearColor, 1.0f, 0);
	} else {
		graphics->getDevice()->Clear(0, 0, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	}

	return 0;
}
SCRIPTING_FUNCTION(clearBuffers)
{
	int numArgs = scripting->getNumArgs();

	if(numArgs == 4) {
		float r = scripting->getArgument<float>(1);
		float g = scripting->getArgument<float>(2);
		float b = scripting->getArgument<float>(3);
		float a = scripting->getArgument<float>(4);
		UINT clearColor = D3DCOLOR_COLORVALUE(r,g,b,a);
		graphics->getDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clearColor, 1.0f, 0);
	} else {
		graphics->getDevice()->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	}

	return 0;
}

SCRIPTING_FUNCTION(beginPass)
{
	int passNumber = scripting->getArgument<int>(1);
	renderer->beginPass(passNumber);

	return 0;
}

SCRIPTING_FUNCTION(endPass)
{
	renderer->endPass();

	return 0;
}

SCRIPTING_FUNCTION(renderObjectLayer)
{
	int numArgs = scripting->getNumArgs();

	if(numArgs == 0)
	{
		ERRORMSG("renderObjectLayer() takes at least one argument!");
	}

	std::string layerName = scripting->getArgument<std::string>(1);
	if(numArgs == 1)
	{
		renderer->renderObjectLayer(layerName);
	}
	else
	{
		int numSubsets = numArgs - 1;
		int currSubset;
		for(int i = 0; i < numSubsets; i++)
		{
			currSubset = scripting->getArgument<int>(i+2);
			renderer->renderObjectLayerSubset(layerName,currSubset);
		}
	}

	return 0;
}

SCRIPTING_FUNCTION(renderQuad)
{
	int numArgs = scripting->getNumArgs();

	if(numArgs != 4 && numArgs != 8)
	{
		ERRORMSG("renderQuad() takes 4 or 8 arguments.");
	}
	
	float x1 = scripting->getArgument<float>(1);
	float x2 = scripting->getArgument<float>(2);
	float y1 = scripting->getArgument<float>(3);
	float y2 = scripting->getArgument<float>(4);

	if(numArgs == 4)
	{
		renderer->renderQuad(x1,x2,y1,y2);
	} else {

		float u1 = scripting->getArgument<float>(5);
		float u2 = scripting->getArgument<float>(6);
		float v1 = scripting->getArgument<float>(7);
		float v2 = scripting->getArgument<float>(8);
		renderer->renderQuad(x1,x2,y1,y2,u1,u2,v1,v2);
	}

	return 0;
}

SCRIPTING_FUNCTION(setBoneMatricesName)
{
	std::string boneMatrixArrayName = scripting->getArgument<std::string>(1);
	renderer->setBoneMatricesName(boneMatrixArrayName);

	return 0;
}

SCRIPTING_FUNCTION(setViewInverse)
{
	std::string viewInverseName = scripting->getArgument<std::string>(1);
	renderer->setEffectViewInverse(viewInverseName);

	return 0;
}

SCRIPTING_FUNCTION(setProjection)
{
	std::string projectionName = scripting->getArgument<std::string>(1);
	float fov = scripting->getArgument<float>(2);
	float aspect = scripting->getArgument<float>(3);
	float nearz = scripting->getArgument<float>(4);
	float farz = scripting->getArgument<float>(5);

	D3DXMATRIX projectionMatrix;
	D3DXMatrixPerspectiveFovLH(&projectionMatrix,fov,aspect,nearz,farz);

	renderer->getEffect()->setMatrix(projectionName.c_str(),projectionMatrix);

	return 0;
}

SCRIPTING_FUNCTION(setViewProjection)
{
	int numArgs = scripting->getNumArgs();
	
	ASSERTMSG(numArgs == 14,"Number of arguments to setViewProjection() should be 14!");

	std::string matrixName = scripting->getArgument<std::string>(1);

	D3DXVECTOR3 eye(scripting->getArgument<float>(2),
					scripting->getArgument<float>(3),
					scripting->getArgument<float>(4));
	D3DXVECTOR3 at(scripting->getArgument<float>(5),
				   scripting->getArgument<float>(6),
				   scripting->getArgument<float>(7));
	D3DXVECTOR3 up(scripting->getArgument<float>(8),
				   scripting->getArgument<float>(9),
				   scripting->getArgument<float>(10));
	float fov = scripting->getArgument<float>(11);
	float aspect = scripting->getArgument<float>(12);
	float nearz = scripting->getArgument<float>(13);
	float farz = scripting->getArgument<float>(14);

	D3DXMATRIX projMatrix, viewMatrix, finalMatrix;
	D3DXMatrixLookAtLH(&viewMatrix,&eye,&at,&up);
	D3DXMatrixPerspectiveFovLH(&projMatrix,fov,aspect,nearz,farz);
	D3DXMatrixMultiply(&finalMatrix,&viewMatrix,&projMatrix);
	renderer->getEffect()->setMatrix(matrixName.c_str(),finalMatrix);

	return 0;
}

SCRIPTING_FUNCTION(addRenderable)
{
	int numArgs          = scripting->getNumArgs();
	std::string meshName = scripting->getArgument<std::string>(1);
	StaticMeshPtr mesh   = staticMeshCache.get(meshName);
	for (int i = 2; i <= numArgs; i++)
	{
		std::string currLayer = scripting->getArgument<std::string>(i);
		renderer->addRenderable(mesh, currLayer, 0);
	}

	return 0;
}

SCRIPTING_FUNCTION(addCollisionGeometry)
{
    std::string meshName = scripting->getArgument<std::string>(1);
    StaticMeshPtr mesh   = staticMeshCache.get(meshName);

    kdTree.addObject(mesh);

    return 0;
}

SCRIPTING_FUNCTION(getRenderString)
{
	std::string varName = scripting->getArgument<std::string>(1);

	boost::optional<std::string> strOption = game->getRenderString(varName);
	if (strOption)
	{
		std::string s = *strOption;
		scripting->pushArgument<std::string>(s);
	}
	else
		scripting->pushNil();
	return 1;
}

SCRIPTING_FUNCTION(getRenderNumber)
{
	std::string varName = scripting->getArgument<std::string>(1);

	boost::optional<float> numOption = game->getRenderNumber(varName);
	if (numOption)
	{
		float f = *numOption;
		scripting->pushArgument<float>(f);
	}
	else
		scripting->pushNil();
	return 1;
}

SCRIPTING_FUNCTION(planespaceBaker)
{
	int numArgs = scripting->getNumArgs();

	std::string layerName = scripting->getArgument<std::string>(1);

	renderer->bakePlanespace(layerName);

	return 0;
}

SCRIPTING_FUNCTION(emitParticlesAt)
{
	std::string emitterName  = scripting->getArgument<std::string>(1);
	float x                  = scripting->getArgument<float>(2);
	float y                  = scripting->getArgument<float>(3);
	float z                  = scripting->getArgument<float>(4);
	unsigned particleDensity = scripting->getArgument<unsigned>(5);

	ParticleEmitterPtr emitter = renderer->getParticleEmitter(emitterName);
	if (!emitter)
	{
		console->write(emitterName + " is not a valid particle emitter!");
		return 0;
	}

	emitter->addEmitterPosition(D3DXVECTOR3(x, y, z), particleDensity);

	return 0;
}

void registerEffectScriptingFunctions()
{
	// Effect settings functions.
    REGISTER_SCRIPTING_FUNCTION(setTexture);
	REGISTER_SCRIPTING_FUNCTION(setCubeTexture);
	REGISTER_SCRIPTING_FUNCTION(setDynamicTexture);
    REGISTER_SCRIPTING_FUNCTION(setInt);
    REGISTER_SCRIPTING_FUNCTION(setFloat);
    REGISTER_SCRIPTING_FUNCTION(setBool);
    REGISTER_SCRIPTING_FUNCTION(setVector);
    REGISTER_SCRIPTING_FUNCTION(setMatrix);
    REGISTER_SCRIPTING_FUNCTION(setTechnique);

	REGISTER_SCRIPTING_FUNCTION(beginEffect);
    REGISTER_SCRIPTING_FUNCTION(setEffect);
    REGISTER_SCRIPTING_FUNCTION(endEffect);

	REGISTER_SCRIPTING_FUNCTION(clearZBuffer);
	REGISTER_SCRIPTING_FUNCTION(clearColorBuffer)
	REGISTER_SCRIPTING_FUNCTION(clearBuffers);

	REGISTER_SCRIPTING_FUNCTION(setRenderTarget);
	REGISTER_SCRIPTING_FUNCTION(restoreScreen);

	REGISTER_SCRIPTING_FUNCTION(beginScene);
	REGISTER_SCRIPTING_FUNCTION(endScene);
	REGISTER_SCRIPTING_FUNCTION(present);

    REGISTER_SCRIPTING_FUNCTION(beginPass);
    REGISTER_SCRIPTING_FUNCTION(endPass);
    REGISTER_SCRIPTING_FUNCTION(renderObjectLayer);
	REGISTER_SCRIPTING_FUNCTION(renderQuad);
	REGISTER_SCRIPTING_FUNCTION(setBoneMatricesName);
	REGISTER_SCRIPTING_FUNCTION(setViewInverse);
	REGISTER_SCRIPTING_FUNCTION(setProjection);
	REGISTER_SCRIPTING_FUNCTION(setViewProjection);

	REGISTER_SCRIPTING_FUNCTION(addRenderable);
    REGISTER_SCRIPTING_FUNCTION(addCollisionGeometry);
	REGISTER_SCRIPTING_FUNCTION(planespaceBaker)

	REGISTER_SCRIPTING_FUNCTION(getRenderString);
	REGISTER_SCRIPTING_FUNCTION(getRenderNumber);

	REGISTER_SCRIPTING_FUNCTION(emitParticlesAt);
}
