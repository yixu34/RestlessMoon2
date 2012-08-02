// A cool, shiny shading pair for animated entities.

//------------------------------------
// Transforms
//------------------------------------
float4x4 mWVP : WorldViewProjection;
float4x4 mW : World;
float4x4 mV : View;
float4x4 mWV : WorldView;
float4x4 boneMatrixArray[40]; 
int influences;

//------------------------------------
// Hacked-Up Lighting
//------------------------------------
float3 halfVector;
float3 lightDirection;
float4 specModulation;
float4 reflModulation;
float4 diffModulation;
int power;

//------------------------------------
// Textures and Sampling
//------------------------------------
texture tDiffuse;
sampler sDiffuse = sampler_state {
    Texture = (tDiffuse);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};
texture tReflection;
sampler sReflection = sampler_state {
	Texture = (tReflection);
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};
texture tEnvironment;
samplerCUBE sEnvironment = sampler_state {
    Texture   = (tEnvironment);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

//------------------------------------
// I/O structures
//------------------------------------
struct appToVert {
	float3 _pos : POSITION;
	float3 _norm : NORMAL;
	float2 _tex : TEXCOORD0;
	float4 _weights : BLENDWEIGHT0;
	int4 _indx : BLENDINDICES0;
};

struct vertToPixel {
    float4 _hpos      : POSITION;
    float2 _texd : TEXCOORD0;
    float2 _texr : TEXCOORD1;
    float3 _texe : TEXCOORD2;
    float3 _norm : TEXCOORD3;
};

vertToPixel myVS(appToVert input) {

    vertToPixel output;

    float4 fpos = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 fnorm = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float totalWeights = 0.0f;
    int k = influences-1;
    for(int i = 0; i < k; i++) {
        totalWeights += input._weights[i];
		fpos += input._weights[i]*
			    mul( float4(input._pos, 1.0f),
			         boneMatrixArray[input._indx[i]] );
		fnorm += input._weights[i]*
				mul( float4(input._norm, 0.0f),
					 boneMatrixArray[input._indx[i]] );
    }
    float lastWeight = 1.0f - totalWeights;
    fpos += lastWeight*
   			mul( float4(input._pos, 1.0f),
   			     boneMatrixArray[input._indx[k]]);
   	fnorm += lastWeight*
   			mul( float4(input._norm, 0.0f),
   				 boneMatrixArray[input._indx[k]]);
    fpos.w = 1.0f;
    fnorm.w = 0.0f;
	
    output._hpos      = mul(fpos, mWVP);
    output._norm     = mul(fnorm, mWV);
    output._texd = input._tex;
    output._texr = input._tex;
    
    float3 norm = normalize(mul(fnorm.xyz, mW));
    float3 worldPos = normalize(mul(fpos.xyz, mW));
    float3 incident = normalize(worldPos - mul(float4(0,0,1,1),mV));
	output._texe = normalize(incident - 2*norm*dot(incident,norm));

    return output;
}

float4 myPS( vertToPixel input) : COLOR
{
	float3 norm = normalize(input._norm);
  	float4 shadeParam = tex2D(sReflection,input._texr);
	float4 diffComponent = diffModulation*
							dot(norm,lightDirection)*
							tex2D(sDiffuse,input._texd);
	float4 specComponent = specModulation*
							shadeParam.g*
							pow(dot(halfVector,norm),power);
	float4 envComponent = reflModulation*
							shadeParam.r*
							texCUBE(sEnvironment,input._texe);
	return diffComponent+specComponent+envComponent;
}

technique t0
{
    pass p0
    {
        VertexShader = compile vs_2_0 myVS();
        PixelShader = compile ps_2_0 myPS();
    }
}
