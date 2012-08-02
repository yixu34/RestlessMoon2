// An effect to render fill textures
// Formally called planespace texturing.
//  author: Don Holden
//  project: Restless Moon 2
//  usage:
//    set matWorldView to the modeling and viewing transform
//    set matProj to the projection transform
//    set skyboxTexture to the skybox texture

//------------------------------------
float4x4 worldViewProjMatrix : WORLDVIEWPROJECTION;
float4x4 worldMatrix : WORLD;

// Technical
float frequencyModulator = 1;
float3 seedTangentA = float3(0,1,0);
float3 seedTangentB = float3(0,0,1);
float tangentBThreshold = 0.867;

// Lighting
bool pointLighting;
float3 lightDirection;
float3 lightIntensity;
float3 ambientColor;

texture diffuseTexture;
sampler diffuseSampler = sampler_state
{
	Texture = <diffuseTexture>;
	MinFilter = linear;
	MagFilter = linear;
	MipFilter = linear;
};
sampler diffuseSamplerAref = sampler_state
{
	Texture = <diffuseTexture>;
	MinFilter = point;
	MagFilter = linear;
	MipFilter = none;
};

struct VertexInputFundamental
{
	float3 _pos : POSITION;
};

struct VertexInput3D
{
	float3 _pos : POSITION;
};

struct VertexInputUV
{
	float3 _pos : POSITION;
	float3 _tex : TEXCOORD0;
};

struct VertexInputRGB
{
	float3 _pos : POSITION;
	float4 _diff : COLOR0;
};

struct VertexInputPlane
{
	float3 _pos : POSITION;
	float3 _norm : NORMAL;
};

struct VertexOutputFundamental
{
	float4 _pos : POSITION;
};

struct VertexOutputPlane
{
	float4 _pos : POSITION;
	float3 _wpos : TEXCOORD0;
	float3 _norm : TEXCOORD1;
	float2 _tex : TEXCOORD2;
};

struct VertexOutput3D
{
	float4 _pos : POSITION;
	float3 _shpos : TEXCOORD0;
};

struct VertexOutputRGB
{
	float4 _pos : POSITION;
	float4 _diff : COLOR0;
};

VertexOutputFundamental fundamentalVS(VertexInputFundamental input)
{
	VertexOutputFundamental output;
	
	output._pos = mul(float4(input._pos,1),worldViewProjMatrix);
	
	return output;
}


VertexOutput3D objectSpaceVS(VertexInput3D input)
{
	VertexOutput3D output;
	
	output._pos = mul(float4(input._pos,1), worldViewProjMatrix);
	output._shpos = input._pos;
	
	return output;
}

VertexOutput3D worldSpaceVS(VertexInput3D input)
{
	VertexOutput3D output;
	
	output._pos = mul(float4(input._pos,1), worldViewProjMatrix);
	output._shpos = mul(float4(input._pos,1), worldMatrix);
	
	return output;
}

VertexOutputPlane planeSpaceVS(VertexInputPlane input)
{
	VertexOutputPlane output;
	
	// Find v, the projection of our position onto the normal plane
	float3 v = input._pos - dot(input._pos,input._norm)*input._norm;
	
	// Construct an orthonormal basis for the plane
	float3 seedY = (abs(dot(seedTangentA,input._norm)) > tangentBThreshold)?
					seedTangentB:seedTangentA;
	float3 tangentX = normalize(cross(input._norm,seedY));
	float3 tangentY = cross(tangentX,input._norm);
	
	// Find world position for pixel lighting
	float4 wpos = mul(float4(input._pos,1), worldMatrix);
	
	output._pos = mul(float4(input._pos,1), worldViewProjMatrix);
	output._wpos = wpos;
	output._norm = input._norm;
	output._tex = float2(dot(v,tangentX),dot(v,tangentY));
	
	return output;
}

VertexOutputRGB bakedLightVS(VertexInputRGB input)
{
	VertexOutputRGB output;
	
	output._pos = mul(float4(input._pos,1), worldViewProjMatrix);
	output._diff = input._diff;
	
	return output;
}

float4 fundamentalPS(VertexOutputFundamental input) : COLOR0
{
	return float4(0,0,0,0);
}

float4 fillTexture3DPS(VertexOutput3D input) : COLOR0
{
	float2 xy = frequencyModulator*input._shpos.xy;
	float2 yz = frequencyModulator*input._shpos.yz;
	float2 zx = frequencyModulator*input._shpos.zx;
	
	float4 xyTex = tex2D(diffuseSampler,xy);
	float4 yzTex = tex2D(diffuseSampler,yz);
	float4 zxTex = tex2D(diffuseSampler,zx);
	
	return 0.333*(xyTex+yzTex+zxTex);
}

float4 fillTexture2DPS(VertexOutputPlane input) : COLOR0
{
	float4 lightModulation = float4(lightIntensity,1)
	                         * max(0,dot(lightDirection,input._norm))
							 + float4(ambientColor,0);
						 
	return lightModulation*
		   tex2D(diffuseSampler,frequencyModulator*input._tex);
}

float4 fillTexture2DPSAref(VertexOutputPlane input) : COLOR0
{
	float4 lightModulation = float4(lightIntensity,1)
	                         * max(0,dot(lightDirection,input._norm))
							 + float4(ambientColor,0);
						 
	return lightModulation*
		   tex2D(diffuseSamplerAref,frequencyModulator*input._tex);
}

float4 vertexLightingPS(VertexOutputRGB input) : COLOR0
{
	return input._diff;
}

Technique fundamental
{
	Pass p0
	{
		VertexShader = compile vs_2_0 fundamentalVS();
		PixelShader = compile ps_2_0 fundamentalPS();
	}
}

Technique objectSpaceDiffuse
{
	Pass p0
	{
		VertexShader = compile vs_2_0 objectSpaceVS();
		PixelShader = compile ps_2_0 fillTexture3DPS();
		
		ZWriteEnable = false;
		ZEnable = true;
		ZFunc = equal;
		
		SrcBlend = one;
		DestBlend = one;
	}
}

Technique worldSpaceDiffuse
{
	Pass p0
	{
		VertexShader = compile vs_2_0 worldSpaceVS();
		PixelShader = compile ps_2_0 fillTexture3DPS();
		
		VertexShader = compile vs_2_0 objectSpaceVS();
		PixelShader = compile ps_2_0 fillTexture3DPS();
		
		ZWriteEnable = false;
		ZEnable = true;
		ZFunc = equal;
		
		SrcBlend = one;
		DestBlend = one;
	}
}

Technique planeSpaceDiffuse
{
	Pass p0
	{
		VertexShader = compile vs_2_0 planeSpaceVS();
		PixelShader = compile ps_2_0 fillTexture2DPS();
		
		ZWriteEnable = false;
		ZEnable = true;
		ZFunc = equal;
		
		SrcBlend = one;
		DestBlend = one;
	}
}

Technique planeSpaceDiffuseAlphaRef
{
	Pass p0
	{
		VertexShader = compile vs_2_0 planeSpaceVS();
		PixelShader = compile ps_2_0 fillTexture2DPSAref();
		
		AlphaTestEnable = true;
		AlphaFunc = greaterEqual;
		AlphaRef = 0x00000033;
	}
}

Technique bakedLight
{
	Pass p0
	{
		VertexShader = compile vs_2_0 bakedLightVS();
		PixelShader = compile ps_2_0 vertexLightingPS();
	}
}