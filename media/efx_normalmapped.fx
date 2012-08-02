/**
  *  Normal-mapped by on-the-fly computed tangent space.
  *  Used for the walls of buildings in Restless Moon 2.
  *
  *  Author: Don Holden
  *  The Game Development Initiative at Cornell University
  *
  **/

/* Transformation parameters */
float4x4 worldViewProjMatrix : worldviewprojection;
float4x4 worldMatrix         : world;
float4x4 inverseViewMatrix   : viewinverse;

/* Shading parameters */
float4 ambientColor;
float4 specularColor;
float  specularPower;
float  bumpiness;

/* Lighting parameters */
float3 lightDirection;

/* Textures */
texture diffuseTexture;
texture normalTexture;
 
/* Samplers */
sampler diffuseSampler = sampler_state
{
   texture = <diffuseTexture>;
   minfilter = linear;
   magfilter = linear;
   mipfilter = linear;
};
sampler normalSampler = sampler_state
{
   texture = <normalTexture>;
   minfilter = linear;
   magfilter = linear;
   mipfilter = linear;
};

/* Graphics structures */
struct AppToVert
{
   float3 _pos  : position;
   float3 _norm : normal;
   float3 _tex  : texcoord0;
};

struct VertToFrag
{
   float4 _hpos  : position;
   float3 _norm  : texcoord0;
   float3 _tang  : texcoord1;
   float3 _bino  : texcoord2;
   float3 _tex   : texcoord3;
   float3 _eyev  : texcoord4;
};

/* Vertex Program */
VertToFrag normalMappedVP(AppToVert input)
{
   VertToFrag output;
   
   output._hpos = mul(float4(input._pos,1),  worldViewProjMatrix);
   output._norm = mul(float4(input._norm,0), worldMatrix);
   float3 seedTangent = (output._norm.y > 0.9f)?
                         float3(1.0f,0.0f,1.0f) : float3(0.0f,1.0f,0.0f);
   output._bino = cross(output._norm, seedTangent);
   output._tang = cross(output._bino, output._norm);
   output._tex  = input._tex;
   output._eyev = inverseViewMatrix[3].xyz -
                  mul(float4(input._pos,1), worldMatrix);
   
   return output;
}

/* Fragment Program */
float4 normalMappedFP(VertToFrag input) : COLOR0
{
	float4 output;
	
	float4 normals = tex2D(normalSampler,  input._tex);
	float4 colors  = tex2D(diffuseSampler, input._tex);
	
	// Lighting and shading calculations
	normals = (2.0f*normals)-1.0f;
	float3 normalLC = (normals.z * input._norm) +
	                  bumpiness *
	                  ((normals.g * input._tang) +
	                   (normals.r * input._bino));
	normalLC = normalize(normalLC);
	float3 reflectLC = 2*dot(normalLC,lightDirection)*normalLC - 
	                   lightDirection;
	float3 eyevectLC = normalize(input._eyev);
	float4 diffuseLC  = colors * dot(lightDirection, normalLC);
	float4 specularLC = specularColor *
	                    pow(max(0,dot(eyevectLC,reflectLC)),specularPower);
	
	output = ambientColor + diffuseLC + specularLC;
	return output;
}

technique t0
{
	pass p0
	{
		vertexShader = compile vs_2_0 normalMappedVP();
		pixelShader  = compile ps_2_0 normalMappedFP();
		cullmode = ccw;
	}
}

	