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
float4 baseColor;
float4 hilightColor;
int power;
float3 offsetVector;
float multiplier;

//----------------------------------//
// Textures

texture blendDiffuse;
texture blendReflection;
texture blendEnvironment;

//----------------------------------//
// Structs

struct AppToVertex {
    float3 _position : POSITION;
    float4 _diffuse : COLOR0;
    float3 _normal : NORMAL;
};

struct VertexToFragment {
    float4 _position : POSITION;
    float4 _diffuse : COLOR0;
    float3 _normal : TEXCOORD0;
};

struct FragmentToFrameBuffer {
	float4 _color : COLOR;
};
	
//----------------------------------//
// Vertex Shader

VertexToFragment MyVertexShader(AppToVertex IN) 
{
    VertexToFragment OUT;
	
	OUT._position = mul(float4(IN._position+offsetVector,1.0f),matWVP);
	OUT._diffuse = IN._diffuse;
	OUT._normal = -mul(float4(IN._normal,0.0f),matWV);
	
    return OUT;
}

//----------------------------------//
// Fragment Shader

FragmentToFrameBuffer MyFragmentShader( VertexToFragment IN)
{
	FragmentToFrameBuffer OUT;
	float3 norm = normalize(IN._normal);
	//OUT._color = multiplier*(baseColor+
		//		 pow(max(0,dot(norm,halfVector)),power)*hilightColor);
	OUT._color = IN._diffuse;
	return OUT;
}

//-----------------------------------
technique t0
{
    pass p0 
    {		
		VertexShader = compile vs_1_1 MyVertexShader();
		PixelShader  = compile ps_2_0 MyFragmentShader();
		CullMode = ccw;
    }
}