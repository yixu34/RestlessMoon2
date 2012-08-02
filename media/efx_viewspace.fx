// A set of shaders used to render viewspace
// animated items, i.e. the RM2 main character's
// weapons.  Such weapons are sent directly to viewspace
// and should not be transformed by the world matrix.
//  author: Don Holden
//  project: Restless Moon 2
//  usage:
//    projectionMatrix must be set by lua, there is no
//      semantic applied so the engine will not know
//      to put the projection there.  This is so you
//      can tweak the fov for better results.
//    worldMatrix is here for environment lighting
//      purposes, and is controlled by semantics.
//    set viewInverseMatrix to the inverse of the viewing
//       matrix.  RM2 lua renderscript has a function to
//       do this.
//    make sure to call setBoneMatrixArrayName() in lua
//       to establish "boneMatrixArray" as the thing for
//       animated entities to write to.
//    influences number of bones considered per vertex.
//       2 is a good number if your game isn't too fancy.

//------------------------------------
// Transforms
//------------------------------------
float4x4 projectionMatrix;
float4x4 worldMatrix : World;
float4x4 viewMatrix : View;
float4x4 viewInverseMatrix : ViewInverse;
float4x4 boneMatrixArray[10];
int influences;

//------------------------------------
// Lighting and Materials
//------------------------------------
//
//  lightPosition should be self-explanatory.
//  diffuseColor is the light/material diffuse
//    color.
//  specularColor is the light/material specular
//    color.
//  ambientColor is the environments ambient
//    light constant
//  minPower is the minimum specular power
//  rangePower is the range of specular power
//    values
//
//  -the maximum power is minPower + rangePower)
//  -for specular lighting uses the Blinn-Phong
//      method.
float3 lightPosition;
float4 diffuseColor;
float4 specularColor;
float4 ambientColor;
float minPower;
float rangePower;
float3 scopeColor;

//------------------------------------
// Textures and Sampling
//------------------------------------
texture diffuseTexture;
sampler diffuseSampler = sampler_state
{
    Texture = (diffuseTexture);
    MipFilter = POINT;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};
texture glossTexture;
sampler glossSampler = sampler_state
{
	Texture = (glossTexture);
	MipFilter = POINT;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};
texture normalTexture;
sampler normalSampler = sampler_state
{
	Texture = (normalTexture);
	MipFilter = POINT;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};
textureCUBE environmentTexture : ENVIRONMENT
<
	string ResourceName = "default_reflection.dds";
	string ResourceType = "Cube";
>;
samplerCUBE environmentSampler = sampler_state
{
    Texture   = (environmentTexture);
    MipFilter = POINT;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};
texture flashTexture;
sampler flashSampler = sampler_state
{
	Texture = (flashTexture);
	MipFilter = POINT;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

//------------------------------------
// I/O structures
//------------------------------------
struct VertexInput
{
	float3 _pos : POSITION;
	float3 _norm : NORMAL;
	float2 _tex : TEXCOORD0;
	float4 _weights : BLENDWEIGHT0;
	int4 _indx : BLENDINDICES0;
};

struct VertexInputFlash
{
	float3 _pos : POSITION;
	float2 _tex : TEXCOORD0;
	float4 _weights : BLENDWEIGHT0;
	int4 _indx : BLENDINDICES0;
};

struct VertexOutputSmp
{
    float4 _hpos : POSITION;
    float2 _tex : TEXCOORD0;
    float3 _norm : TEXCOORD1;
    float3 _light : TEXCOORD2;
    float3 _eye : TEXCOORD3;
};

struct VertexOutputStd
{
    float4 _hpos : POSITION;
    float2 _tex : TEXCOORD0;
    float3 _norm : TEXCOORD1;
    float3 _light : TEXCOORD2;
    float3 _eye : TEXCOORD3;
    float3 _env : TEXCOORD4;
};

struct VertexOutputAdv
{
    float4 _hpos : POSITION;
    float2 _tex : TEXCOORD0;
    float3 _norm : TEXCOORD1;
    float3 _light : TEXCOORD2;
    float3 _eye : TEXCOORD3;
    float3 _env : TEXCOORD4;
};

struct VertexOutputFlash
{
	float4 _hpos : POSITION;
	float2 _tex : TEXCOORD0;
};

struct VertexOutputScope
{
	float4 _hpos : POSITION;
	float3 _normal : TEXCOORD0;
};


//------------------------------------
// HLSL Shaders
//------------------------------------

// Computes all the goodies, including fresnel reflection.
VertexOutputAdv ViewspaceAdvancedVS(VertexInput input) {

    VertexOutputAdv output;
    
    output._hpos = mul(float4(input._pos,1), projectionMatrix);
    output._norm = mul(float4(input._norm,0), worldMatrix);
    
    // output light vector in world space
    float3 worldPos = mul(float4(input._pos,1),worldMatrix);
    output._light    = lightPosition - worldPos;
    
    // output texture
    output._tex = input._tex;
    
    // output eye vector
    float3 eyePos = viewInverseMatrix[3].xyz;
    output._eye = normalize(eyePos - worldPos);
    
    // output environment map lookup
    float3 norm = output._norm;
    float3 incident = output._eye;
	output._env = 2*norm*dot(incident,norm) - incident;

    return output;
}

// Computes reflections
VertexOutputStd ViewspaceStandardVS(VertexInput input) {

    VertexOutputStd output;

    output._hpos = mul(float4(input._pos,1), projectionMatrix);
    output._norm = mul(float4(input._norm,0), worldMatrix);
    
    // output light vector in world space
    float3 worldPos = mul(float4(input._pos,1),worldMatrix);
    output._light    = lightPosition - worldPos;
    
    // output texture
    output._tex = input._tex;
    
    // output eye vector
    float3 eyePos = viewInverseMatrix[3].xyz;
    output._eye = normalize(eyePos - worldPos);
    
    // output environment map lookup
    float3 norm = output._norm;
    float3 incident = output._eye;
	output._env = 2*norm*dot(incident,norm) - incident;
	
    return output;
}

// Just pixel phong
VertexOutputSmp ViewspaceSimpleVS(VertexInput input) {

    VertexOutputSmp output;

    output._hpos = mul(float4(input._pos,1), projectionMatrix);
    output._norm = mul(float4(input._norm,0), worldMatrix);
    
    // output light vector in world space
    float3 worldPos = mul(float4(input._pos,1),worldMatrix);
    output._light    = lightPosition - worldPos;
    
    // output texture
    output._tex = input._tex;
    
	// output eye vector
    float3 eyePos = viewInverseMatrix[3].xyz;
    output._eye = eyePos - worldPos;

    return output;
}

VertexOutputScope ScopeVS(VertexInput input)
{
    VertexOutputScope output;

    output._hpos = mul(float4(input._pos,1), projectionMatrix);
    output._normal = mul(float4(input._norm,0), worldMatrix);
	
	return output;
}


// Muzzle Flash - Simple
VertexOutputFlash MuzzleFlashVS(VertexInputFlash input) {

    VertexOutputFlash output;

    output._hpos = mul(float4(input._pos,1), projectionMatrix);
    
    // output texture
    output._tex = input._tex;

    return output;
}

float4 GlossMappedAdvancedPS( VertexOutputAdv input) : COLOR
{
	float3 lightVector = normalize(input._light);
	float3 normalVector = normalize(input._norm);
	float3 eyeVector = normalize(input._eye);
	float3 halfVector = 0.5*(lightVector+eyeVector);
	
	float4 diffuseRead = tex2D(diffuseSampler,input._tex);
	float4 ambientComponent = ambientColor*diffuseRead;
	
	float4 diffuseComponent = max(0,dot(lightVector,normalVector))*
	                          diffuseRead;
	
	float4 glossRead = tex2D(glossSampler,input._tex);                    
	float4 specularComponent = specularColor*glossRead.x*
	                           pow(max(0,dot(halfVector,normalVector)),
	                               minPower+glossRead.y*rangePower);
	                               
	float4 envComponent = glossRead.z*
	                      texCUBE(environmentSampler,input._env);
	                     
    return ambientComponent + diffuseComponent +
           specularComponent + envComponent;
	
	
}

float4 GlossMappedSimplePS( VertexOutputSmp input) : COLOR0
{
	float3 lightVector = normalize(input._light);
	float3 normalVector = normalize(input._norm);
	float3 eyeVector = normalize(input._eye);
	float3 halfVector = 0.5*(lightVector+eyeVector);
	
	float4 diffuseRead = tex2D(diffuseSampler,input._tex);
	float4 ambientComponent = ambientColor*diffuseRead;
	
	float4 diffuseComponent = max(0,dot(lightVector,normalVector))*
	                          diffuseRead;
	
	float4 glossRead = tex2D(glossSampler,input._tex);                    
	float4 specularComponent = specularColor*glossRead.x*
	                           pow(max(0,dot(halfVector,normalVector)),
	                               minPower+glossRead.y*rangePower);
	
	return ambientComponent + diffuseComponent + specularComponent;
}	                          

float4 MuzzleFlashPS(VertexOutputFlash input) : COLOR0
{
	return tex2D(flashSampler, input._tex);
}

float4 ScopePS(VertexOutputScope input) : COLOR0
{						  	 
	float3 refractVect = normalize(input._normal);
	
	return float4(scopeColor,0)*texCUBE(environmentSampler, refractVect);
}

float4 TestPS(VertexOutputAdv input) : COLOR0
{
	return tex2D(diffuseSampler,input._tex);
}

technique AdvancedGlossMap
{
    pass p0
    {
        VertexShader = compile vs_2_0 ViewspaceAdvancedVS();
        PixelShader = compile ps_2_0 GlossMappedAdvancedPS();
        
        ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
    }
}

technique SimpleGlossMap
{
	pass p0
	{
		VertexShader = compile vs_2_0 ViewspaceSimpleVS();
		PixelShader = compile ps_2_0 GlossMappedSimplePS();
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
	}
}

technique MuzzleFlash
{
	pass p0
	{
		VertexShader = compile vs_2_0 MuzzleFlashVS();
		PixelShader = compile ps_2_0 MuzzleFlashPS();
		
		CullMode = none;
		
		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
	}
}

technique Scope
{
	pass p0
	{
		VertexShader = compile vs_2_0 ScopeVS();
		PixelShader = compile ps_2_0 ScopePS();
		
		AlphaBlendEnable = false;
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
    }
}

technique Test
{
	pass p0
	{
		VertexShader = compile vs_2_0 ViewspaceAdvancedVS();
		PixelShader = compile ps_2_0 TestPS();
	}
}