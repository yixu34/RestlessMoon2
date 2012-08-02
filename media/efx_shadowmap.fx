// A simple software shadowmapper.  Has techniques to render
//   shadow maps onto things
//  author: Don Holden
//  project: Restless Moon 2
//  usage:
//    camWorldViewProjMatrix is the matrix going from object to eye
//  space.  worldMatrix goes from object to world space.
//  lightViewProjMatrix has no semantics, and is set by the
//  RM2 renderscript to go from world space to light space.

//------------------------------------
// Tweaks
//------------------------------------
float bias;
float delta = 0.001;

//------------------------------------
// For animated entity shadowing
//------------------------------------
float4x4 boneMatrixArray[40];
int influences;

//------------------------------------
// Transforms
//------------------------------------
float4x4 camWorldViewProjMatrix : WORLDVIEWPROJECTION;
float4x4 worldMatrix : WORLD;
float4x4 lightViewProjMatrix;

//------------------------------------
// Texturing and sampling
//------------------------------------
texture shadowMap;
sampler shadowMapSampler = sampler_state
{
	Texture = (shadowMap);
	MinFilter = point;
	MagFilter = point;
	MipFilter = none;
	AddressU = clamp;
	AddressV = clamp;
};

//------------------------------------
// Structures
//------------------------------------
struct VertexInput
{
	float3 _pos : POSITION;
};

struct VertexOutputCreate
{
	float4 _hpos : POSITION;
	float2 _depth : TEXCOORD0;
};

struct VertexOutputApply
{
	float4 _hpos : POSITION;
	float3 _wpos : TEXCOORD0;
	float4 _cpos : TEXCOORD1;
};

//------------------------------------
// Shaders for Shadow Map Creation
//------------------------------------
VertexOutputCreate ShadowMapCreateVS(VertexInput input)
{
	VertexOutputCreate output;
	
	output._hpos = mul(float4(input._pos,1),worldMatrix);
	output._hpos = mul(output._hpos,lightViewProjMatrix);
//	output._hpos = mul(float4(input._pos,1),camWorldViewProjMatrix);
	output._depth = output._hpos.zw;
	
	return output;
}

float4 ShadowMapCreatePS(VertexOutputCreate input) : COLOR
{
	// perform homogeneous divide here
	//Color = input._depth.x / input._depth.y;
	float output = 0.5*(input._depth.x/ input._depth.y) + 0.5;
	//output = 0.5*(1.0 - output);
	
	return output;
}

//------------------------------------
// Shaders for Shadow Map Application
//------------------------------------
VertexOutputApply ShadowMapApplyVS(VertexInput input)
{
	VertexOutputApply output;
	
	output._hpos = mul(float4(input._pos,1),camWorldViewProjMatrix);
	output._cpos = output._hpos;
	output._wpos = mul(float4(input._pos,1),worldMatrix);

	return output;
}

float4 ShadowMapApplyPS(VertexOutputApply input) : COLOR0
{
	float4 shadowCoord = mul(float4(input._wpos,1), lightViewProjMatrix);
    shadowCoord.xyz /= shadowCoord.w;
    shadowCoord.y = -shadowCoord.y;
    shadowCoord.xyz = 0.5*(shadowCoord.xyz+1);
    
    float4 clipCoord = input._cpos;
    clipCoord.xy /= clipCoord.w;
    clipCoord.y = -clipCoord.y;
    clipCoord.xy = 0.5*(clipCoord.xy+1);
    
    float shadowRead;
    shadowRead = tex2D(shadowMapSampler, shadowCoord.xy);
 //   float4 shadowerRead = tex2D(shadowerSampler, shadowCoord.xy);
    
//	float finalColor = (shadowRead.x + bias < shadowCoord.z)? (shadowerRead):(1.0);
 //	finalColor = (shadowCoord.z > 1)? 1.0 : finalColor;
 	
 	float4 finalColor = (shadowRead.x + bias < shadowCoord.z)? 0.5:1.0;
    return finalColor;
 }
 
 Technique CreateShadowMap
 {
 	Pass p0
 	{
 		VertexShader = compile vs_2_0 ShadowMapCreateVS();
 		PixelShader = compile ps_2_0 ShadowMapCreatePS();
 		
 		AlphaBlendEnable = false;
 		
 		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
        
        CullMode = none;
 	}
 }
 
 Technique ApplyShadowMap
 {
 	Pass p0
 	{
 		VertexShader = compile vs_2_0 ShadowMapApplyVS();
 		PixelShader = compile ps_2_0 ShadowMapApplyPS();
 		
 		AlphaBlendEnable = true;
 		
 		SrcBlend = zero;
 		DestBlend = srccolor;
 		
 		ZEnable = true;
        ZWriteEnable = false;
        ZFunc = equal;
        
        CullMode = ccw;
 	}
 }