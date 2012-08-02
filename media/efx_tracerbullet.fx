// For bullets and projectiles and such
//  author: Don Holden
//  project: Restless Moon Ex

float4x4 projMatrix : PROJECTION;
float4x4 worldMatrix : WORLD;
float4x4 worldViewMatrix : WORLDVIEW;
float4x4 worldViewProjMatrix : WORLDVIEWPROJECTION;
float4x4 viewInverseMatrix : VIEWINVERSE;

float4 bulletColor = float4(1,1,1,0);
float scale = 1.0f;

//----------------------------------//
// Textures

texture fireTexture;
sampler fireSampler = sampler_state
{
	Texture = (fireTexture);
	MinFilter = linear;
	MagFilter = linear;
	MipFilter = linear;
};
textureCUBE environmentTexture : ENVIRONMENT
<
	string ResourceType = "Cube";
>;
samplerCUBE environmentSampler = sampler_state
{
    Texture   = (environmentTexture);
    MipFilter = POINT;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

//----------------------------------//
// Structs

struct VertexInput
{
	float3 _pos : POSITION;
};

struct VertexInputTex
{
    float3 _pos : POSITION;
    float2 _tex : TEXCOORD0;
};

struct VertexOutput
{
    float4 _pos : POSITION;
};

struct VertexOutputTex
{
	float4 _pos : POSITION;
	float2 _tex : TEXCOORD0;
};

struct VertexInputRefl
{
	float3 _pos : POSITION;
	float3 _norm : NORMAL;
};

struct VertexOutputRefl
{
	float4 _pos : POSITION;
	float3 _refl : TEXCOORD0;
};
	
//----------------------------------//
// Boolets

VertexOutput BulletVS(VertexInput input) 
{
    VertexOutput output;
	
	output._pos = mul(float4(scale*input._pos,1.0f),worldViewProjMatrix);
	
    return output;
}

float4 BulletPS( VertexOutput input) : COLOR0
{
	return bulletColor;
}

//----------------------------------//
// Shiny Rocket (yes they be shiny)

VertexOutputRefl ShinyVS(VertexInputRefl input)
{
	VertexOutputRefl output;
	
	output._pos = mul(float4(scale*input._pos,1), worldViewProjMatrix);
	
	float4 normVector = mul(float4(input._norm,0), worldMatrix);
	float3 worldPos = mul(float4(input._pos,1), worldMatrix);
	float3 eyeVector = viewInverseMatrix[3].xyz - worldPos;
	output._refl = 2*dot(normVector,eyeVector)*normVector - eyeVector;
	
	return output;
}

float4 ShinyPS(VertexOutputRefl input) : COLOR0
{
	return bulletColor*texCUBE(environmentSampler,input._refl);
}
	

//----------------------------------//
// Flamer

VertexOutputTex FlamerVS(VertexInputTex input)
{
	VertexOutputTex output;
	
	// Instantiate the sprite basis xy
	float4 basisX = mul(float4(1,0,0,0),worldViewMatrix);
	float4 basisY = mul(float4(0,1,0,0),worldViewMatrix);
	float4 posXYZ = mul(float4(0,0,0,1),worldViewMatrix);
	
	output._pos = posXYZ + float4(scale*input._pos.x,scale*input._pos.y,0,0);
	output._pos = mul(output._pos,projMatrix);
	output._tex = input._tex;
	
	return output;
}

float4 FlamerPS(VertexOutputTex input) : COLOR0
{	
	return tex2D(fireSampler,input._tex);
}

//-----------------------------------
technique Bullet
{
    pass p0 
    {		
		VertexShader = compile vs_2_0 BulletVS();
		PixelShader  = compile ps_2_0 BulletPS();
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
        
        AlphaBlendEnable = false;
    }
}

technique Flamer
{
	pass p0
	{
		VertexShader = compile vs_2_0 FlamerVS();
		PixelShader = compile ps_2_0 FlamerPS();
		
		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;
		
		ZEnable = true;
		ZWriteEnable = false;
		ZFunc = lessequal;
		
		CullMode = none;
	}
}

technique Shiny
{
    pass p0 
    {		
		VertexShader = compile vs_2_0 ShinyVS();
		PixelShader  = compile ps_2_0 ShinyPS();
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
        
        AlphaBlendEnable = false;
    }
}