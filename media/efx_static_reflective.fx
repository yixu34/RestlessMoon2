// An environment mapped, textured shader for
// reflective static objects
//  author: Don Holden
//  project: Restless Moon Ex
//  usage:
//    Set the lightDirection to something reasonable
//    Set the halfVector to norm((lightVect+viewVect)/2)
//    Set the specular color to something
//    Set the reflective color to something
//    Set the diffuse texture to, err, a diffuse texture
//    The reflective texture red corresponds to reflectiveness
//    The reflective texture green corresponds to phonginess
//    The environment texture is looked up based on my homemade
//      formula

float4x4 matW : World;
float4x4 matWV : WorldView;
float4x4 matWVP : WorldViewProjection;
float3 halfVector;
float4 specModulation;
float4 reflModulation;
float4 diffModulation;
int power;

//----------------------------------//
// Textures

texture blendDiffuse;
texture blendReflection;
texture blendEnvironment;

sampler sampDiffuse = sampler_state
{
    Texture = (blendDiffuse);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler sampReflection = sampler_state
{
	Texture = (blendReflection);
	MipFilter = POINT;
	MinFilter = POINT;
	MagFilter = POINT;
};

samplerCUBE sampEnvironment = sampler_state
{
    Texture   = (blendEnvironment);
    MipFilter = POINT;
    MinFilter = POINT;
    MagFilter = POINT;
};

//----------------------------------//
// Structs

struct AppToVertex {
    float3 _position : POSITION;
    float3 _normal : NORMAL;
    float2 _texture : TEXCOORD0;
};

struct VertexToFragment {
    float4 _position : POSITION;
    float3 _normal : TEXCOORD0;
    float2 _texture : TEXCOORD1;
   	float3 _environment : TEXCOORD2;
};

struct FragmentToFrameBuffer {
	float4 _color : COLOR;
};
	
//----------------------------------//
// Vertex Shader

VertexToFragment MyVertexShader(AppToVertex IN) 
{
    VertexToFragment OUT;
	
	OUT._position = mul(float4(IN._position,1.0f),matWVP);
	
	OUT._normal = -mul(float4(IN._normal,0.0f),matWV);
	
	OUT._texture = IN._texture;
	
	float3 norm = normalize(mul(IN._normal, matW));
	float3 worldp = normalize(mul(IN._position, matW));
	float3 incident = normalize(worldp - mul(float4(0,0,1,1),matWV));
	OUT._environment = normalize(incident - 2*norm*dot(incident,norm));
	
    return OUT;
}

//----------------------------------//
// Fragment Shader

FragmentToFrameBuffer MyFragmentShader( VertexToFragment IN)
{
	FragmentToFrameBuffer OUT;
	float3 norm = normalize(IN._normal);
  	float4 shadeParam = tex2D(sampReflection,IN._texture);
	float4 diffComponent = diffModulation*
							tex2D(sampDiffuse,IN._texture);
	float4 specComponent = specModulation*
							shadeParam.g*
							pow(dot(halfVector,norm),power);
	float4 envComponent = reflModulation*
							shadeParam.r*
							texCUBE(sampEnvironment,IN._environment);
	OUT._color = diffComponent+specComponent+envComponent;
	return OUT;
}

//-----------------------------------
technique t0
{
    pass p0 
    {		
		VertexShader = compile vs_1_1 MyVertexShader();
		PixelShader  = compile ps_2_0 MyFragmentShader();
    }
}