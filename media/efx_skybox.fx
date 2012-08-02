// An effect to render the skybox
//  author: Don Holden
//  project: Restless Moon 2
//  usage:
//    set matWorldView to the modeling and viewing transform
//    set matProj to the projection transform
//    set skyboxTexture to the skybox texture

//------------------------------------
float4x4 matWorldView : WORLDVIEW;
float4x4 matProj : PROJECTION;

//------------------------------------
texture skyboxTexture;

//------------------------------------
// Set texturing to simple old-school
//  point sampling to give the stars
//  a twinkling effect, not to mention
//  much faster sampling
//------------------------------------
sampler sampSkybox = sampler_state
{
    Texture   = (skyboxTexture);
    MipFilter = NONE;
    MinFilter = POINT;
    MagFilter = POINT;
};

//------------------------------------
struct AppToVertex
{
    float3 _pos : POSITION;
    float2 _tex : TEXCOORD0;
};
struct VertexToPixel
{
    float4 _hpos : POSITION;
    float2 _tex : TEXCOORD0;
};
struct PixelToSurf
{
    float4 _col : COLOR;
    float _depth : DEPTH;
};

//------------------------------------
VertexToPixel myVertexShader(AppToVertex input)
{
    VertexToPixel output;
    
    float4 pos = mul(float4(input._pos, 0), matWorldView);
    output._hpos = mul(float4(pos.xyz, 1), matProj);
    
    output._tex = input._tex;

    return output;
}

//------------------------------------
PixelToSurf myPixelShader(VertexToPixel input)
{
    PixelToSurf output;
    
    output._col = tex2D(sampSkybox,input._tex);
    output._depth = 1.0f;
    
    return output;
}

//------------------------------------
technique t0
{
    pass Pass0
    {

        VertexShader = compile vs_2_0 myVertexShader();
        PixelShader  = compile ps_2_0 myPixelShader();
     
     	ZWriteEnable = true;
     	ZFunc = always;
    }
}

