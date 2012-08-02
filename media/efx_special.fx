// A misc. shader file for glass, light beams,
// and whatever else might get tossed in here.
//  author: Don Holden
//  project: Restless Moon 2


//------------------------------------
// Transforms
//------------------------------------
float4x4 worldViewProjMatrix : WORLDVIEWPROJECTION;
float4x4 worldMatrix : WORLD;
float4x4 viewInverseMatrix;

//------------------------------------
// Lighting and Color
//------------------------------------
float3 lightPosition;
float3 specularColor;
float specularPower;
float4 lightBeamColor;
float opacity = 0.7;

//------------------------------------
// Technical
//------------------------------------
float3 seedTangentA = float3(0,1,0);
float3 seedTangentB = float3(0,0,1);
float tangentBThreshold = 0.867;
float frequencyModulator = 1;

//------------------------------------
// Texturing and sampling
//------------------------------------
texture lightBeamTexture;
sampler lightBeamSampler = sampler_state
{
	Texture = (lightBeamTexture);
	MinFilter = linear;
	MagFilter = linear;
	MipFilter = none;
};
texture glassTexture;
sampler glassSampler = sampler_state
{
	Texture = (glassTexture);
	MinFilter = linear;
	MagFilter = linear;
	MipFilter = linear;
};

//------------------------------------
// Structures
//------------------------------------
struct LightVertexInput
{
	float3 _pos : POSITION;
	float2 _tex : TEXCOORD0;
};

struct GlassVertexInput
{
	float3 _pos : POSITION;
	float3 _norm : NORMAL;
	float2 _tex : TEXCOORD0;
};

struct LightVertexOutput
{
	float4 _hpos : POSITION;
	float2 _tex : TEXCOORD0;
};

struct GlassVertexOutput
{
	float4 _hpos : POSITION;
	float2 _tex : TEXCOORD0;
	float3 _norm : TEXCOORD1;
	float3 _light : TEXCOORD2;
	float3 _eye : TEXCOORD3;
};

//------------------------------------
// Shaders for rendering light beams
//------------------------------------
LightVertexOutput LightBeamVS(LightVertexInput input)
{
	LightVertexOutput output;
	
	output._hpos = mul(float4(input._pos,1),worldViewProjMatrix);
	output._tex = input._tex;
	
	return output;
}

float4 LightBeamPS(LightVertexOutput input) : COLOR0
{
	return lightBeamColor*tex2D(lightBeamSampler,input._tex);
}

//------------------------------------
// Shaders for rendering glass
//------------------------------------
GlassVertexOutput GlassVS(GlassVertexInput input)
{
	GlassVertexOutput output;
	
	// Shade Position, Normal, and Color
	output._hpos = mul(float4(input._pos,1), worldViewProjMatrix);
	output._norm = mul(float4(input._norm,0), worldViewProjMatrix);
	
	output._tex = frequencyModulator*input._tex;
	
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

float4 GlassPS(GlassVertexOutput input) : COLOR0
{
	float3 normalVector = normalize(input._norm);
	float3 lightVector = normalize(input._light);
	float3 eyeVector = normalize(input._eye);
	float3 halfVector = 0.5*(lightVector + eyeVector);
	
	float4 textureRead = tex2D(glassSampler, input._tex);
	
	return opacity*textureRead;
}

//------------------------------------
// Techniques and passes
//------------------------------------
Technique LightBeam
{
	Pass p0
	{
		VertexShader = compile vs_2_0 LightBeamVS();
		PixelShader = compile ps_2_0 LightBeamPS();
		
		AlphaBlendEnable = true;
		
		SrcBlend = one;
		DestBlend = one;
		
		ZWriteEnable = false;
		
		CullMode = cw;
	}
}

Technique Glass
{
	Pass p0
	{
		VertexShader = compile vs_2_0 GlassVS();
		PixelShader = compile ps_2_0 GlassPS();
		
		AlphaBlendEnable = true;
		
		SrcBlend = srcalpha;
		DestBlend = invsrcalpha;
		
		ZWriteEnable = false;
		
		CullMode = ccw;
	}
}