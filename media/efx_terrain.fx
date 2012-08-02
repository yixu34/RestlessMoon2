// A mutlitexturing terrain shader
//  author: Don Holden
//  project: Restless Moon 2

//------------------------------------
float4x4 worldViewProjMat : WORLDVIEWPROJECTION;

//------------------------------------
float3 lightPosition;
float3 ambientColor;

//------------------------------------
Texture slopeTexture;
Texture flatLowTexture;
Texture flatHighTexture;

//------------------------------------
sampler slopeSampler = sampler_state
{
	Texture = (slopeTexture);
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};
sampler flatLowSampler = sampler_state
{
	Texture = (flatLowTexture);
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};
sampler floatHighTexture = sampler_state
{
	Texture = (flatHighTexture);
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};
	

//------------------------------------
struct VertexInput
{
    float3 _pos	: POSITION;
    float3 _norm : NORMAL;
};

struct VertexOutput
{
    float4 _hpos : POSITION;
    float3 _norm : TEXCOORD0;
    float2 _slopetex : TEXCOORD1;
    float2 _lowtex : TEXCOORD2;
    float2 _hightex : TEXCOORD3;
    float3 _light : TEXCOORD4;
    float _height : TEXCOORD5;
    
};

//------------------------------------
VertexOutput terrainVS(VertexInput input) 
{
    VertexOutput output;
    output._hpos = mul( float4(input._pos.xyz , 1) , worldViewProjMat);
	output._norm = input._norm;

	output._lowtex = 0.0001*input._pos.xy;
	output._hightex = 0.00008928437*input._pos.xy;
	output._slopetex = 0.0001248753*input._pos.xy;
	
	output._height = input._pos.y;
	
	output._light = lightPosition - input._pos;
    return output;
}

float4 terrainPS(VertexOutput input) : COLOR0
{
	float4 slopeRead = tex2D(slopeSampler,input._slopetex);
	float4 lowRead = tex2D(slopeSampler,input._lowtex);
	float4 highRead = tex2D(slopeSampler,input._hightex);
	
	float3 normalVector = normalize(input._norm);
	float3 lightVector = normalize(input._light);
	
	float slopeAmount = 1.0f - normalVector.y;
	float highAmount = 0.0001 * (1500.0f - input._height);
	return 1;
	//return output;
}

//-----------------------------------
technique Technique0
{
    pass p0 
    {		
		VertexShader = compile vs_2_0 terrainVS();
		PixelShader = compile ps_2_0 terrainPS();
    }
}