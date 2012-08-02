// A mutlitexturing terrain shader
//  author: Don Holden
//  project: Curse of the Revenant 3D
//  usage:
//    set lightDirection for directional lighting
//    blendSlope should be for cliff walls high slope terrain
//    blendOne corresponds to guide red channel
//    blendTwo corresponds to guide green channel
//    blendThree corresponds to guide blue channel
//    blendGuide determines how to blend the textures
//    freq_____ is the uv frequency of its corresponding texture

//------------------------------------
float4x4 matWorldViewProj : WORLDVIEWPROJECTION;
//float4x4 matWorldView : WORLDVIEW;

//------------------------------------
float3 lightDirection;

//------------------------------------
Texture diffuseTexture;

//------------------------------------
sampler sampDiffuse = sampler_state
{
	Texture = (diffuseTexture);
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};

//------------------------------------
struct AppToVertex
{
    float3 _pos	: POSITION;
    float3 _norm : NORMAL;
    float2 _uvmap : TEXCOORD0;
};

struct VertexToPixel
{
    float4 _hpos : POSITION;
    float4 _col : COLOR0;
    float2 _uvmap : TEXCOORD2;
};

struct PixelToSurf
{
	float4 _col : COLOR;
};

//------------------------------------
VertexToPixel myVertexShader(AppToVertex input) 
{
    VertexToPixel output;
    output._hpos = mul( float4(input._pos.xyz , 1) , matWorldViewProj);
	output._col = float4(1,1,1,1)*(.3+pow(dot(input._norm,lightDirection),64));
	output._uvmap = input._uvmap;
    return output;
}

PixelToSurf myPixelShader(VertexToPixel input)
{
	PixelToSurf output;
	output._col = tex2D(sampDiffuse,input._uvmap)*input._col;
	return output;
}

//-----------------------------------
technique t0
{
    pass Pass0 
    {		
		VertexShader = compile vs_1_1 myVertexShader();
		PixelShader = compile ps_1_1 myPixelShader();
    }
}
