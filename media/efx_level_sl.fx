texture diffuseTexture;

technique t0
{
	pass p0
	{
		// Turn on bilinear filtering
        Texture[0] = <diffuseTexture>;
  		MinFilter[0] = Linear;
  		MagFilter[0] = Linear;
  		MipFilter[0] = Point;
   
   		// Turn off lighting
        Lighting = false;

		// Set color selection states
        ColorOp[0]   = SelectArg1;
        ColorArg1[0] = Texture;
		
	}

}