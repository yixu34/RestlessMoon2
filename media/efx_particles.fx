//
// Effect file for rendering explosion particle effects.
// All this does is set various D3D parameters.
//

texture particleTexture;
sampler particleSampler = sampler_state
{
    texture = <particleTexture>;
};

technique Blood
{
	pass p0
	{
		Lighting = false;
		PointSpriteEnable = true;
		PointScaleEnable  = true;
		PointSize = 10.0f;
		PointSize_Min = 0.0f;

		PointScale_A = 0.0f;
		PointScale_B = 0.0f;
		PointScale_C = 1.0f;

		AlphaBlendEnable = true;
		SrcBlend = srccolor;
		DestBlend = one;

		ColorOp[0] = Modulate;
		ColorArg1[0] = Texture;
		ColorArg2[0] = Diffuse;

		Sampler[0] = (particleSampler);

		ZEnable = true;
		ZWriteEnable = false;

	}
}

technique Explosion
{
    pass p0
    {
        Lighting = false;
		PointSpriteEnable = true;
		PointScaleEnable  = true;
		PointSize = 80.0f;
		PointSize_Min = 15.0f;

		PointScale_A = 0.0f;
		PointScale_B = 0.0f;
		PointScale_C = 1.0f;

		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;

		ColorOp[0] = Modulate;
		ColorArg1[0] = Texture;
		ColorArg2[0] = Diffuse;

		Sampler[0] = (particleSampler);

		ZEnable = true;
		ZWriteEnable = false;
		
    }
}

technique Spark
{
    pass p0
    {
        Lighting = false;
		PointSpriteEnable = true;
		PointScaleEnable  = true;
		PointSize = 4.0f;
		PointSize_Min = 4.0f;

		PointScale_A = 0.0f;
		PointScale_B = 0.0f;
		PointScale_C = 1.0f;

		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;

		ColorOp[0] = Modulate;
		ColorArg1[0] = Texture;
		ColorArg2[0] = Diffuse;
		
		Sampler[0] = (particleSampler);

		ZEnable = true;
		ZWriteEnable = false;
		
    }
}

technique Missile
{
	pass p0
	{
		Lighting = false;
		PointSpriteEnable = true;
		PointScaleEnable  = true;
		PointSize = 15.0f;
		PointSize_Min = 0.0f;

		PointScale_A = 0.0f;
		PointScale_B = 0.0f;
		PointScale_C = 1.0f;

		AlphaBlendEnable = true;
		SrcBlend = srccolor;
		DestBlend = one;

		ColorOp[0] = Modulate;
		ColorArg1[0] = Texture;
		ColorArg2[0] = Diffuse;

		Sampler[0] = (particleSampler);

		ZEnable = true;
		ZWriteEnable = false;

	}
}
