// An effect to render some sweet characters!

//------------------------------------
// Transforms
//------------------------------------
float4x4 worldViewProjMatrix : WORLDVIEWPROJECTION;
float4x4 worldMatrix : WORLD;
float4x4 viewInverseMatrix : VIEWINVERSE;

//------------------------------------
// Lighting and Materials
//------------------------------------
float3 lightPosition;
float3 lightColor;
float3 ambientColor;
float3 diffuseColor;
float3 specularColor;
float specularPower;
float reflectivity;

//------------------------------------
// Texturing and Sampling
//------------------------------------
texture diffuseTexture;
sampler diffuseSampler = sampler_state
{
	//Texture = <diffuseTexture>;
	MinFilter = linear;
	MagFilter = linear;
	MipFilter = linear;
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

//------------------------------------
// Shading Structures
//------------------------------------
struct VertexInputNoTex
{
	float3 _pos : POSITION;
	float3 _norm : NORMAL;
};

struct VertexOutputNoTexNoEye
{
	float4 _hpos : POSITION;
	float3 _norm : TEXCOORD0;
	float3 _light : TEXCOORD1;
};

struct VertexOutputNoTex
{
	float4 _hpos : POSITION;
	float3 _norm : TEXCOORD0;
	float3 _light : TEXCOORD1;
	float3 _eye : TEXCOORD2;
};
	
// Computes fragment lighting shade with no texture
VertexOutputNoTexNoEye lambertianLightingNoTexVS(VertexInputNoTex input)
{
	VertexOutputNoTexNoEye output;
	
	// Shade Position, Normal, and Color
	output._hpos = mul(float4(input._pos,1), worldViewProjMatrix);
	float4 norm = mul(float4(input._norm,0), worldMatrix);
	output._norm = norm.xyz;
	
	// Find the light vector - don't normalize (fragment program
	//   has to do this anyway)
	float3 worldPos = mul(float4(input._pos,1), worldMatrix);
	output._light = lightPosition - worldPos;

	return output;
}

// Computes fragment lighting shade with no texture
VertexOutputNoTex shinyLightingNoTexVS(VertexInputNoTex input)
{
	VertexOutputNoTex output;
	
	// Shade Position, Normal, and Color
	output._hpos = mul(float4(input._pos,1), worldViewProjMatrix);
	float4 norm = mul(float4(input._norm,0), worldMatrix);
	output._norm = norm.xyz;
	
	// Find the light vector - don't normalize (fragment program
	//   has to do this anyway)
	float3 worldPos = mul(float4(input._pos,1), worldMatrix);
	output._light = lightPosition - worldPos;
	
	// Find the eye vector using the view inverse, again leaving
	//   the fragment program to normalize.
	float3 eyePos = viewInverseMatrix[3].xyz;
    output._eye = eyePos - worldPos;

	return output;
}

// Pixel Shading, Lambertian, no texture
float4 lambertianNoTexPS(VertexOutputNoTexNoEye input) : COLOR0
{
	float3 normalVector = normalize(input._norm);
	float3 lightVector = normalize(input._light);
	
	float4 ambientComponent = float4(ambientColor,1);
	float4 diffuseComponent = float4(lightColor*diffuseColor,0)*
							  max(0,dot(normalVector,lightVector));
	
	return ambientComponent + diffuseComponent;
}

// Pixel Shading, Blinn-Phong, no texture
float4 phongNoTexPS(VertexOutputNoTex input) : COLOR0
{
	float3 normalVector = normalize(input._norm);
	float3 lightVector = normalize(input._light);
	float3 eyeVector = normalize(input._eye);
	float3 halfVector = 0.5*(lightVector + eyeVector);
	
	float4 ambientComponent = float4(ambientColor,1);
	float4 diffuseComponent = float4(lightColor*diffuseColor,0)*
							  max(0,dot(normalVector,lightVector));
							  
	float4 specularComponent = float4(specularColor,0)*
							   pow(max(0,dot(normalVector,halfVector)),
							       specularPower);
	
	return ambientComponent + diffuseComponent + specularComponent;
}

// Pixel Shading, Phong, reflective surface, no texture
float4 reflectNoTexPS(VertexOutputNoTex input) : COLOR0
{
	float3 normalVector = normalize(input._norm);
	float3 lightVector = normalize(input._light);
	float3 eyeVector = normalize(input._eye);
	float3 eyeReflection = 2*dot(normalVector,eyeVector)*normalVector - eyeVector;
	
	float4 envSample = texCUBE(environmentSampler, eyeReflection);
	
	float4 ambientComponent = float4(ambientColor,1);
	float4 diffuseComponent = float4(lightColor*diffuseColor,0)*
							  max(0,dot(normalVector,lightVector));
							  
	float4 specularComponent = float4(specularColor,0)*
							   pow(max(0,dot(lightVector,eyeReflection)),
							       specularPower);
	float4 envComponent = reflectivity*envSample;
	
	return ambientComponent + diffuseComponent + specularComponent + envComponent;
}

Technique lambertianNoTex
{
	Pass p0
	{
		VertexShader = compile vs_2_0 lambertianLightingNoTexVS();
		PixelShader = compile ps_2_0 lambertianNoTexPS();
		
		AlphaBlendEnable = false;
		
		CullMode = ccw;
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
	}
}

Technique phongNoTex
{
	Pass p0
	{
		VertexShader = compile vs_2_0 shinyLightingNoTexVS();
		PixelShader = compile ps_2_0 phongNoTexPS();
		
		AlphaBlendEnable = false;
		
		CullMode = ccw;
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
   }
}

Technique reflectNoTex
{
	Pass p0
	{
		VertexShader = compile vs_2_0 shinyLightingNoTexVS();
		PixelShader = compile ps_2_0 reflectNoTexPS();
		
		AlphaBlendEnable = false;
		
		CullMode = ccw;
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
   }
}