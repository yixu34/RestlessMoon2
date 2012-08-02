/**
 * Restless Moon 2
 *  Post Process
 */
 
 float alpha;
 
 texture sourceTexture;
 sampler sourceSampler = sampler_state
 {
 	texture = <sourceTexture>;
 	MinFilter = linear;
 	MagFilter = linear;
 	MipFilter = none;
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
 };
 
 struct VertexOutputMagic
 {
 	float4 _hpos : POSITION;
 	float2 _tex : TEXCOORD0;
 	float3 _angMagic : TEXCOORD1;
 };
 
 VertexOutput renderOverVS(VertexInput input)
 {
 	VertexOutput output;
 	output._hpos = float4(input._pos,1.0f);
 	output._tex = input._tex;
 	
 	return output;
 }
 
 VertexOutputMagic dashMagicVS(VertexInput input)
 {
 	VertexOutputMagic output;
 	
 	output._hpos = float4(input._pos,1.0f);
 	output._tex = input._tex;
 	output._angMagic = float3(cos(alpha),sin(alpha),alpha);
 	
 	return output;
 }
 
 float4 renderOverPS(VertexOutput input) : COLOR0
 {
 	return tex2D(sourceSampler,input._tex);
 }
 
 float4 renderOverAlphaPS(VertexOutput input) : COLOR0
 {
 	return alpha*tex2D(sourceSampler,input._tex);
 }
 
 float4 dashMagicPS(VertexOutputMagic input) : COLOR0
 {
 	float2 tc1 = input._tex;
 	float2 tc2 = -input._tex;
 	
 	return  input._angMagic.z*(input._angMagic.x*(tex2D(sourceSampler,tc1) +
 	       (input._angMagic.y*tex2D(sourceSampler,tc2))));
 }
 
 Technique renderOver
 {
 	Pass p0
 	{
 		VertexShader = compile vs_1_1 renderOverVS();
 		PixelShader = compile ps_1_1 renderOverPS();
 		
 		AlphaBlendEnable = false;
 		
 		ZWriteEnable = false;
 		ZEnable = true;
 		ZFunc = always;
 		
 		CullMode = none;
 	}
 }

Technique renderOverAlpha
{
	Pass p0
	{
		VertexShader = compile vs_2_0 renderOverVS();
		PixelShader = compile ps_2_0 renderOverAlphaPS();
		
		AlphaBlendEnable = true;
		
		SrcBlend = one;
		DestBlend = one;
		
		ZWriteEnable = false;
		ZEnable = true;
		ZFunc = always;
		
		CullMode = none;
	}
	Pass p1
	{
		VertexShader = compile vs_2_0 renderOverVS();
		PixelShader = compile ps_2_0 renderOverAlphaPS();
		
		AlphaBlendEnable = true;
		
		SrcBlend = one;
		DestBlend = one;
		
		ZEnable = false;
		
		CullMode = none;
	}
}

Technique dashMagic
{
	Pass p0
	{
		VertexShader = compile vs_2_0 dashMagicVS();
		PixelShader = compile ps_2_0 dashMagicPS();
		
		AlphaBlendEnable = true;
		
		SrcBlend = one;
		DestBlend = one;
		
		ZWriteEnable = false;
		ZEnable = true;
		ZFunc = always;
		
		CullMode = none;
	}
}