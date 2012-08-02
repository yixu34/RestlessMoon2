/**
 * Restless Moon 2
 *  HUD shader
 */
 
 texture hudTexture;
 sampler hudSampler = sampler_state
 {
 	texture = <hudTexture>;
 };
 
 // Minimum position and delta (usually (.01,0
 float2 minPosition;
 float2 maxPosition;
 
 float barAmount;
 
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
 
 VertexOutput hudRenderVP(VertexInput input)
 {
 	VertexOutput output;
 	output._hpos = float4(input._pos,1.0f);
 	output._tex = input._tex;
 	
 	return output;
 }
 
 VertexOutput barRenderVP(VertexInput input)
 {
 	VertexOutput output;
 	float xCoordinate = minPosition.x + input._pos.x*barAmount*(maxPosition.x - minPosition.x);
 	float yCoordinate = minPosition.y + input._pos.y*(maxPosition.y - minPosition.y);
 	
 	output._hpos = float4(xCoordinate,yCoordinate,0.0,1.0);
 	
 	float xTexture = input._pos.x*barAmount;
 	float yTexture = -input._pos.y;

 	output._tex = float2(xTexture,yTexture);
 	
 	return output;
 }
 
 float4 hudRenderFP(VertexOutput input) : COLOR0
 {
 	return tex2D(hudSampler,input._tex);
 }
 
 Technique renderHUD
 {
 	Pass p0
 	{
 		VertexShader = compile vs_1_1 hudRenderVP();
 		PixelShader = compile ps_1_1 hudRenderFP();
 		
 		AlphaBlendEnable = true;
 		SrcBlend = srcalpha;
 		DestBlend = invsrcalpha;
 		
 		ZWriteEnable = false;
 		ZEnable = true;
 		ZFunc = always;
 		
 		CullMode = none;
 	}
 }
 
 Technique renderBar
 {
 	Pass p0
 	{
 		VertexShader = compile vs_1_1 barRenderVP();
 		PixelShader = compile ps_1_1 hudRenderFP();
 		
 		AlphaBlendEnable = true;
 		SrcBlend = srcalpha;
 		DestBlend = invsrcalpha;
 		
 		ZWriteEnable = false;
 		ZEnable = true;
 		ZFunc = always;
 		
 		CullMode = none;
 	}
 }