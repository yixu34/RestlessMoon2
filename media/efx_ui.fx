/**
 * Restless Moon 2
 *  UI Shading
 */
 
 // Light position
 float3 lightPosition;
 float3 eyePosition;
 
 // Range of height
 float  minimumHeight;
 float  rangeHeight;
 
 float3 specularColor;
 float3 diffuseColor;
 float3 ambientColor;
 
 float phongExponent;
 float bumpiness = 1;
 
 // Simple diffuse texture
 texture diffuseTexture;
 sampler diffuseSampler = sampler_state
 {
 	texture = <diffuseTexture>;
 };
 
 // Height/Normal texture -- normals in rgb,
 //  height in alpha
 texture heightNormalTexture;
 sampler heightNormalSampler = sampler_state
 {
 	texture = <heightNormalTexture>;
 };
 
 struct VertexInput
 {
 	float3 _pos : POSITION;
 	float2 _tex : TEXCOORD0;
 };
 
 struct VertexOutput
 {
 	float4 _hpos : POSITION;
 	float2 _tex : TEXCOORD0;
 	float3 _pos : TEXCOORD1;
 };
 
 VertexOutput menuRenderVP(VertexInput input)
 {
 	VertexOutput output;
 	output._hpos = float4(input._pos,1.0f);
 	output._tex = input._tex;
 	output._pos = input._pos;
 	
 	return output;
 }

 float4 menuRenderFlatFP(VertexOutput input) : COLOR0
 {
 	return tex2D(diffuseSampler,input._tex);
 }
 
 float4 menuRenderBumpyFP(VertexOutput input) : COLOR0
 {
 	float4 bumpRead = tex2D(heightNormalSampler, input._tex);
 	float4 diffuseRead = tex2D(diffuseSampler,   input._tex);
 	
 	float3 myPosition = float3(input._pos.xy,minimumHeight + bumpRead.a*rangeHeight);
 	float3 lightVector = normalize(lightPosition-myPosition);
 	float3 normalVector = normalize(float3(bumpiness*(2*bumpRead.xy-1),bumpRead.z));
 	float3 eyeVector = normalize(eyePosition-myPosition);
 	float3 halfVector = normalize(eyeVector + lightVector);
 	
 	float3 ambientDiffuseTerm = diffuseRead.rgb*(ambientColor + diffuseColor*max(0,dot(lightVector,normalVector)));
	float3 specularTerm = specularColor*pow(max(0,dot(halfVector,normalVector)),phongExponent);
	
	return float4(ambientDiffuseTerm+specularTerm,diffuseRead.a);
}
 
 Technique menuRenderFlat
 {
 	Pass p0
 	{
 		VertexShader = compile vs_2_0 menuRenderVP();
 		PixelShader = compile ps_2_0 menuRenderFlatFP();
 		
 		AlphaBlendEnable = true;
 		SrcBlend = srcalpha;
 		DestBlend = invsrcalpha;
 		
 		ZEnable = false;
 		
 		CullMode = none;
 	}
 	Pass p1
 	{
 		VertexShader = compile vs_2_0 menuRenderVP();
 		PixelShader = compile ps_2_0 menuRenderFlatFP();
 		
 		AlphaBlendEnable = true;
 		SrcBlend = srcAlpha;
 		DestBlend = one;
 		
 		ZEnable = false;
 		
 		CullMode = none;
 	}
 	 Pass p2
 	{
 		VertexShader = compile vs_2_0 menuRenderVP();
 		PixelShader = compile ps_2_0 menuRenderFlatFP();
 		
 		AlphaBlendEnable = true;
 		SrcBlend = srcAlpha;
 		DestBlend = destColor;
 		
 		ZEnable = false;
 		
 		CullMode = none;
 	}
 }
 
 Technique menuRenderBumpy
 {
 	Pass p0
 	{
 		VertexShader = compile vs_2_0 menuRenderVP();
 		PixelShader = compile ps_2_0 menuRenderBumpyFP();
 		
 		AlphaBlendEnable = true;
 		SrcBlend = srcalpha;
 		DestBlend = invsrcalpha;
 		
 		ZEnable = false;
 		
 		CullMode = none;
 	}
 }