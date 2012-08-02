/**
  *  Animated texture.  Set the frame and let it burn.
  *  Used for video screens in Restless Moon 2.
  *
  *  Texture coordinates are assumed to be wrapped around
  *  the first frame, in the upper-left-hand corner.
  *
  *  Author: Don Holden
  *  The Game Development Initiative at Cornell University
  *
  **/

/* Transformation parameters */
float4x4 worldViewProjMatrix : worldviewprojection;
float4x4 worldMatrix : world;

/* Animation Parameters */
int numFramesWidth;
float frameWidth;
float frameHeight;
int frame;
float frameF;

/* Textures */
texture animationTexture;
 
/* Samplers */
sampler animationSampler = sampler_state
{
   texture = <animationTexture>;
   minfilter = linear;
   magfilter = linear;
   mipfilter = linear;
};

/* Graphics structures */
struct AppToVert
{
   float3 _pos  : position;
   float2 _tex  : texcoord0;
};

struct VertToFrag
{
   float4 _hpos   : position;
   float2 _tex    : texcoord0;
   float  _worldy : texcoord1;
};

/* Vertex Program */
VertToFrag animationVP(AppToVert input)
{
   VertToFrag output;
   
   output._hpos = mul(float4(input._pos,1),  worldViewProjMatrix);
   
   output._tex = input._tex;
   output._tex.x += (frame % numFramesWidth) * frameWidth;
   output._tex.y += (frame / numFramesWidth) * frameHeight;
   
   float4 worldPosition = mul(float4(input._pos,1), worldMatrix);
   output._worldy = worldPosition.y;
   
   return output;
}

/* Fragment Program */
float4 animationFP(VertToFrag input) : COLOR0
{	
	return 2*tex2D(animationSampler,input._tex)+0.2*sin(input._worldy+frameF);
}

technique t0
{
	pass p0
	{
		vertexShader = compile vs_2_0 animationVP();
		pixelShader  = compile ps_2_0 animationFP();
	}
}

	