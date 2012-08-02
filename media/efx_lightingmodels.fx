// An effect to render scene objects using standard lighting
//   models.  If the planespace baker is used, be sure to specify
//   a frequency modulator or things may look weird.
//
//  author: Don Holden
//  project: Restless Moon 2
//  usage:
//    set worldViewProjMatrix to the modeling, viewing,
//       and projecting transform, automatic for RM2
//    set worldMatrix to modeling matrix for pixel lighting,
//       also automatic for RM2
//    set viewInverseMatrix to the inverse of the viewing
//       matrix.  RM2 lua renderscript has a function to
//       do this.
//    frequencyModulator controls how frequently the texture
//       occurs.  Tweak this to get the right texture pattern
//       scale.
//    use pass 1 instead of pass 0 for alpha-ref
//    
//  Available Lighting Models:
//    No Falloff Point-Light Lambertian Textured
//
//  PLEASE PAY NOTE:
//    A word on normal mapping.  The normal mapping
//   method here is VERY nonstandard.  The normal map
//   is in surface local space which is constructed
//   by the same means as the planespace texture.
//    This is very effective if you want to slap
//   bumpy geometry on a surface not parallel to the
//   xz plane.  It has severe limitations on planes
//   parallel to the xz plane.

//------------------------------------
// Transforms
//------------------------------------
float4x4 worldViewProjMatrix : WORLDVIEWPROJECTION;
float4x4 worldMatrix : WORLD;
float4x4 viewInverseMatrix : VIEWINVERSE;

//------------------------------------
// Technical
//------------------------------------
float frequencyModulator = 1;
float3 seedTangentA = float3(0,1,0);
float3 seedTangentB = float3(0,0,1);
float tangentBThreshold = 0.867;

//------------------------------------
// Lighting and Materials
//------------------------------------
float3 lightPosition;
float3 lightColor;
float3 ambientColor;
float3 specularColor;
float specularPower;
float bumpiness = 1;
float reflectivity;

//------------------------------------
// Texturing and Sampling
//------------------------------------
texture diffuseTexture;
sampler diffuseSampler = sampler_state
{
	//Texture = <diffuseTexture>;
	MinFilter = linear;
	MagFilter = linear;
	MipFilter = linear;
};
sampler diffuseSamplerAref = sampler_state
{
	//Texture = <diffuseTexture>;
	MinFilter = point;
	MagFilter = linear;
	MipFilter = none;
};
texture normalTexture;
sampler normalSampler = sampler_state
{
	//Texture = <normalTexture>;
	MinFilter = linear;
	MagFilter = linear;
	MipFilter = linear;
};

//------------------------------------
// Shading Structures
//------------------------------------

// Textured Lambertian
struct VertexInputPLT
{
	float3 _pos : POSITION;
	float3 _norm : NORMAL;
	float2 _tex : TEXCOORD0;
};
struct VertexOutputPLT
{
	float4 _hpos : POSITION;
	float3 _norm : TEXCOORD0;
	float2 _tex : TEXCOORD1;
	float3 _light : TEXCOORD2;
};

// Textured Phong
struct VertexInputPPT
{
	float3 _pos : POSITION;
	float3 _norm : NORMAL;
	float2 _tex : TEXCOORD0;
};
struct VertexOutputPPT
{
	float4 _hpos : POSITION;
	float3 _norm : TEXCOORD0;
	float2 _tex : TEXCOORD1;
	float3 _light : TEXCOORD2;
	float3 _eye : TEXCOORD3;
};

// Textured Normal-Mapped Phong
struct VertexInputPNT
{
	float3 _pos : POSITION;
	float3 _norm : NORMAL;
	float2 _tex : TEXCOORD0;
};
struct VertexOutputPNT
{
	float4 _hpos : POSITION;
	float3 _norm : TEXCOORD0;
	float2 _tex : TEXCOORD1;
	float3 _light : TEXCOORD2;
	float3 _eye : TEXCOORD3;
	float3 _tan : TEXCOORD4;
	float3 _bin : TEXCOORD5;
};

// Untextured Lambertian
struct VertexInputPLU
{
	float3 _pos : POSITION;
	float3 _norm : NORMAL;
};
struct VertexOutputPLU
{
	float4 _hpos : POSITION;
	float3 _norm : TEXCOORD0;
	float3 _light : TEXCOORD1;
};

// Untextured Phong
struct VertexInputPPU
{
	float3 _pos : POSITION;
	float3 _norm : NORMAL;
};
struct VertexOutputPPU
{
	float4 _hpos : POSITION;
	float3 _norm : TEXCOORD0;
	float3 _light : TEXCOORD1;
	float3 _eye : TEXCOORD2;
};

// Vertex Shader for simple lambertian point-light shading.
VertexOutputPLT pltVS(VertexInputPLT input)
{
	VertexOutputPLT output;
	
	// Shade Position, Normal, and Color
	output._hpos = mul(float4(input._pos,1), worldViewProjMatrix);
	output._norm = input._norm;
	output._tex = frequencyModulator*input._tex;
	
	// Find the light vector - don't normalize (fragment program
	//   has to do this anyway)
	float3 worldPos = mul(float4(input._pos,1), worldMatrix);
	output._light = lightPosition - worldPos;

	return output;
}

// Fragment Shader for simple lambertian point-light shading.
float4 pltPS(VertexOutputPLT input) : COLOR0
{
	float3 normalVector = normalize(input._norm);
	float3 lightVector = normalize(input._light);
	float4 diffuseComponent = max(0,dot(normalVector,lightVector))*
							  float4(lightColor,0);
	
	return tex2D(diffuseSampler, input._tex)*
		   (float4(ambientColor,1) + diffuseComponent);
}

// Vertex Shader for simple phong point-light shading.
VertexOutputPPT pptVS(VertexInputPLT input)
{
	VertexOutputPPT output;
	
	// Shade Position, Normal, and Color
	output._hpos = mul(float4(input._pos,1), worldViewProjMatrix);
	output._norm = input._norm;
	output._tex = frequencyModulator*input._tex;
	
	// Find the light vector and eye vector
	float3 worldPos = mul(float4(input._pos,1), worldMatrix);
	output._light = lightPosition - worldPos;
    output._eye = viewInverseMatrix[3].xyz - worldPos;

	return output;
}

// Fragment Shader for simple phong point-light shading.
float4 pptPS(VertexOutputPPT input) : COLOR0
{
	float3 normalVector = normalize(input._norm);
	float3 lightVector = normalize(input._light);
	float3 eyeVector = normalize(input._eye);
	float3 halfVector = 0.5*(lightVector + eyeVector);
	float4 diffuseComponent = max(0,dot(normalVector,lightVector))*
							  float4(lightColor,0);
	float4 specularComponent = float4(specularColor,0)*
							   pow(max(0,dot(normalVector,halfVector)),
							   specularPower);
	
	return tex2D(diffuseSampler, input._tex)*
		   (float4(ambientColor,1) + diffuseComponent) + specularComponent;
}

VertexOutputPNT pntVS(VertexInputPNT input)
{
	VertexOutputPNT output;
	
	// Shade Position, Normal, and Color
	output._hpos = mul(float4(input._pos,1), worldViewProjMatrix);
	output._norm = input._norm;
	output._tex = frequencyModulator*input._tex;
	
	// Construct an orthonormal basis for the plane
	float3 seedY = (abs(dot(seedTangentA,input._norm)) > tangentBThreshold)?
					seedTangentB:seedTangentA;
	float3 tangentX = normalize(cross(input._norm,seedY));
	float3 tangentY = cross(tangentX,input._norm);
    output._tan = tangentX;
    output._bin = tangentY;
    
	// Find the light vector - don't normalize (fragment program
	//   has to do this anyway)
	float3 worldPos = mul(float4(input._pos,1), worldMatrix);
	output._light = lightPosition - worldPos;
	
	// Find the eye vector using the view inverse, again leaving
	//   the fragment program to normalize.
    output._eye = viewInverseMatrix[3].xyz - worldPos;
   
	return output;
}

float4 pntPS(VertexOutputPNT input) : COLOR0
{
	float3 zVector = normalize(input._norm);
	float3 yVector = normalize(input._tan);
	float3 xVector = normalize(input._bin);
	
	float3 lightVector = normalize(input._light);
	float3 eyeVector = normalize(input._eye);
	float3 halfVector = 0.5*(lightVector + eyeVector);
	
	float4 textureRead = tex2D(diffuseSampler, input._tex);
	float4 normalRead = 2*(tex2D(normalSampler, input._tex)-0.5);
	
	float3 normalVector = normalRead.z*zVector +
						  bumpiness*(normalRead.y*yVector +
						  			 normalRead.x*xVector);
//	float3 normalVector = normalRead.z*input._norm +
//						  bumpiness*(normalRead.y*input._tan +
//						  			 normalRead.x*input._bin);
	normalVector = normalize(normalVector);
	
	float4 ambientComponent = float4(ambientColor,1);
	float4 diffuseComponent = float4(lightColor,0)*
	                          max(0,dot(normalVector,lightVector));
	float4 specularComponent = float4(specularColor,0)*
							   pow(max(0,dot(normalVector,halfVector)),
							   specularPower);
	
	return textureRead*(ambientComponent + diffuseComponent) + specularComponent;
}


// Vertex Shader for simple lambertian point-light shading.
VertexOutputPLU pluVS(VertexInputPLU input)
{
	VertexOutputPLU output;
	
	// Shade Position, Normal, and Color
	output._hpos = mul(float4(input._pos,1), worldViewProjMatrix);
	output._norm = input._norm;
	
	// Find the light vector - don't normalize (fragment program
	//   has to do this anyway)
	float3 worldPos = mul(float4(input._pos,1), worldMatrix);
	output._light = lightPosition - worldPos;

	return output;
}

// Fragment Shader for simple lambertian point-light shading.
float4 pluPS(VertexOutputPLU input) : COLOR0
{
	float3 normalVector = normalize(input._norm);
	float3 lightVector = normalize(input._light);
	float4 diffuseComponent = max(0,dot(normalVector,lightVector))*
							  float4(lightColor,0);
	
	return (float4(ambientColor,1) + diffuseComponent);
}

// Vertex Shader for simple phong point-light shading.
VertexOutputPPU ppuVS(VertexInputPLU input)
{
	VertexOutputPPU output;
	
	// Shade Position, Normal, and Color
	output._hpos = mul(float4(input._pos,1), worldViewProjMatrix);
	output._norm = input._norm;
	
	// Find the light vector and eye vector
	float3 worldPos = mul(float4(input._pos,1), worldMatrix);
	output._light = lightPosition - worldPos;
    output._eye = viewInverseMatrix[3].xyz - worldPos;

	return output;
}

// Fragment Shader for simple phong point-light shading.
float4 ppuPS(VertexOutputPPU input) : COLOR0
{
	float3 normalVector = normalize(input._norm);
	float3 lightVector = normalize(input._light);
	float3 eyeVector = normalize(input._eye);
	float3 halfVector = 0.5*(lightVector + eyeVector);
	float4 diffuseComponent = max(0,dot(normalVector,lightVector))*
							  float4(lightColor,0);
	float4 specularComponent = float4(specularColor,0)*
							   pow(max(0,dot(normalVector,halfVector)),
							   specularPower);
	
	return float4(ambientColor,1) + diffuseComponent + specularComponent;
}

Technique PointLambertianTexture
{
	Pass p0
	{
		VertexShader = compile vs_2_0 pltVS();
		PixelShader = compile ps_2_0 pltPS();
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
        
        CullMode = ccw;
		
		Sampler[0] = (diffuseSampler);
		Texture[0] = (diffuseTexture);
	}
	pass p1
	{
		VertexShader = compile vs_2_0 pltVS();
		PixelShader = compile ps_2_0 pltPS();
				
		AlphaTestEnable = true;
		AlphaFunc = greater;
		AlphaRef = 0x00000088;
		
		CullMode = none;
		        
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
		
		Sampler[0] = (diffuseSamplerAref);
		Texture[0] = (diffuseTexture);
	}
}

Technique PointPhongTexture
{
	Pass p0
	{
		VertexShader = compile vs_2_0 pptVS();
		PixelShader = compile ps_2_0 pptPS();
		
		CullMode = ccw;
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
		
		Sampler[0] = (diffuseSampler);
		Texture[0] = (diffuseTexture);
	}
	Pass p1
	{
		VertexShader = compile vs_2_0 pptVS();
		PixelShader = compile ps_2_0 pptPS();
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
        
        CullMode = none;
		
		AlphaTestEnable = true;
		AlphaFunc = greater;
		AlphaRef = 0x00000088;
		
		Sampler[0] = (diffuseSamplerAref);
		Texture[0] = (diffuseTexture);
	}
}

Technique PointNormalTexture
{
	Pass p0
	{
		VertexShader = compile vs_2_0 pntVS();
		PixelShader = compile ps_2_0 pntPS();
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
        
        CullMode = ccw;
        
		Sampler[0] = (diffuseSampler);
		Texture[0] = (diffuseTexture);
		Sampler[1] = (normalSampler);
		Texture[1] = (normalTexture);
	}
	
	pass p1
	{
		VertexShader = compile vs_2_0 pntVS();
		PixelShader = compile ps_2_0 pntPS();
				
		AlphaTestEnable = true;
		AlphaFunc = greater;
		AlphaRef = 0x00000088;
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
        
        CullMode = none;
		
		Sampler[0] = (diffuseSamplerAref);
		Texture[0] = (diffuseTexture);
		Sampler[1] = (normalSampler);
		Texture[1] = (normalTexture);
	}
	
}


Technique PointLambertian
{
	Pass p0
	{
		VertexShader = compile vs_2_0 pluVS();
		PixelShader = compile ps_2_0 pluPS();
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
        
        CullMode = ccw;
		
	}
}

Technique PointPhong
{
	Pass p0
	{
		VertexShader = compile vs_2_0 ppuVS();
		PixelShader = compile ps_2_0 ppuPS();
		
		CullMode = ccw;
		
		ZEnable = true;
        ZWriteEnable = true;
        ZFunc = lessequal;
	}
}