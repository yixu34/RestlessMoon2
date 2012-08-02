// Simple Level Geometry Shader for Restless Moon 2

//------------------------------------
// Transforms
//------------------------------------
float4x4 mWVP : WorldViewProjection;
float4x4 mWV : WorldView;

//------------------------------------
// Lights
//------------------------------------
float3 halfVector;
float3 specularMax;
float3 diffuseMax;

//------------------------------------
// Texturing and Sampling
//------------------------------------
texture tDiffuse;
sampler sDiffuse = sampler_state {
	Texture = <tDiffuse>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};
texture tSpecular;
sampler sSpecular = sampler_state {
	Texture = <tSpecular>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = POINT;
};

//------------------------------------
struct appToVert {
    float3 _pos	: POSITION;
    float3 _norm : NORMAL;
    float2 _tex : TEXCOORD0;
};

struct vertToFrag {
    float4 _hpos : POSITION;
    float3 _vnorm : TEXCOORD0;
    float2 _texd : TEXCOORD1;
    float2 _texs : TEXCOORD2;
};

//------------------------------------
vertToFrag vsSimple(appToVert input)  {
    vertToFrag output;
    output._hpos = mul( float4(input._pos.xyz , 1.0f) , mWVP);
    output._vnorm = normalize(mul( float4(input._norm.xyz , 0.0f) , mWV));
    
    output._texd = input._tex;
    output._texs = input._tex;
    
    return output;
}

float4 psSimple(vertToFrag input) : COLOR0 {
	float3 diffComp = tex2D(sDiffuse,input._texd);
	float3 specComp = tex2D(sSpecular,input._texs);
	
	diffComp *= diffuseMax;
	specComp *= specularMax*(pow(dot(input._vnorm,halfVector),2));
	specComp = max(specComp,0);
	return float4(specComp + diffComp,1.0f);
	
}

//-----------------------------------
technique t0
{
    pass p0 
    {		
		VertexShader = compile vs_2_0 vsSimple();
		PixelShader = compile ps_2_0 psSimple();
		
		CullMode = ccw;
    }
}