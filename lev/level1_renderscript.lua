--
-- LEVEL 2
-- RENDER SCRIPT
-- 
-- RESTLESS MOON 2
--

dofile("lev/common_renderscript.lua")

function doRenderScript()
	if not haveCached then
		haveCached = true
		cacheEffectStates(lightX, lightY, lightZ, "media/tex_etc_skybox.jpg", "media/tex_lev_glass.png")
	end
		
	renderSkybox()
	renderGeometry()
	renderEnemies()
	renderProps(ambientR,ambientG,ambientB)
	renderSpecialGeometry()
	renderWeapons()
	renderParticleEffects()
	renderViewspaceWeapon(ambientR,ambientG,ambientB)
	renderScreenPostFX()
	renderHUD()
	
end

function renderShadowCasters(objectLayer)
	renderObjectLayer(objectLayer,1,2,3,4,5,6)
end

function renderShadowReceivers(objectLayer)
	renderObjectLayer(objectLayer,1,2,3,4,5,6)
end

function renderGeometry()
                         
	setEffect("media/efx_lightingmodels.fx")
	setViewInverse("viewInverseMatrix")
	
	setVector("ambientColor",ambientR,ambientG,ambientB)
	
	setTechnique("PointLambertianTexture")
	beginEffect()
	
	--
	-- Render Cement Fill 1
	--
	setVector("lightColor",2,2,2)
	setTexture("diffuseTexture","media/tex_lev_fill_concreteA.jpg")
	setFloat("frequencyModulator",0.0009)
	beginPass(0)
		renderObjectLayer("levelGeometry",cementFill1,metalOne)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_fill_concreteB.jpg")
	setVector("lightColor",0.6,0.6,0.6)
	beginPass(0)
		renderObjectLayer("levelGeometry",cementFill2,cementFill3,metalTwo)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_fill_rubble.jpg")
	setVector("lightColor",0.6,0.6,0.6)
	beginPass(0)
		renderObjectLayer("levelGeometry",planeDamage)
	endPass()
	
	endEffect()
	
	setTechnique("PointNormalTexture")
	beginEffect()
	
		--
	-- Render Metal Shelfing
	--
	setTexture("diffuseTexture","media/tex_lev_fill_metalB.jpg")
	setTexture("normalTexture","media/tex_lev_floorpattern1_normap.bmp")
	setFloat("frequencyModulator",0.007)
	setFloat("specularColor",5,5,5)
	setFloat("specularPower",18)
	setFloat("bumpiness",50)
	beginPass(0)
		renderObjectLayer("levelGeometry",shelfing,plating)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_moonrock.jpg")
	setTexture("normalTexture","media/tex_lev_roughness_normap.bmp")
	setFloat("frequencyModulator",0.0007)
	setVector("specularColor",0.0,0.0,0.0)
	setFloat("bumpiness",3)
	beginPass(0)
		renderObjectLayer("levelGeometry",moonRock)
	endPass()
	
	endEffect()
	
	-- We're rendering your crashed spacepod
	
	setEffect("media/efx_characters.fx")
	
	setVector("lightColor",1,1,1)
	setTechnique("reflectNoTex")
	beginEffect()
	
	setVector("ambientColor",0.5*ambientR,0.5*ambientG,0.5*ambientB)
	setVector("diffuseColor",0.8,0.75,0.6)
	setVector("specularColor",6,6,6)
	setFloat("specularPower",32)
	setFloat("reflectivity",0.2)
	beginPass(0)
		renderObjectLayer("levelGeometry",planeFuselage,planeStripe)
	endPass()
		
	setVector("ambientColor",0,0,0)
	setVector("diffuseColor",0,0,0)
	setVector("specularColor",3,3,3)
	setFloat("reflectivity",1)
	beginPass(0)
		renderObjectLayer("levelGeometry",planeWindshield)
	endPass()

	endEffect()
	setTechnique("phongNoTex")
	beginEffect()
	
	setVector("ambientColor",0.1*ambientR,0.1*ambientG,0.1*ambientB)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",15,15,15)
	setFloat("specularPower",16)
	beginPass(0)
		renderObjectLayer("levelGeometry",planeEngine)
	endPass()
	
	setVector("diffuseColor",0.3,0.3,0.3)
	setVector("ambientColor",0.2*ambientR,0.2*ambientG,0.2*ambientB)
	setFloat("specularPower",8)
	beginPass(0)
		renderObjectLayer("levelGeometry",planeGuns)
	endPass()
	
	endEffect()
	setTechnique("lambertianNoTex")
	beginEffect()
	
	setVector("ambientColor",0.05*ambientR,0.05*ambientG,0.05*ambientB)
	setVector("diffuseColor",0.2,0.2,0.2)
	beginPass(0)
		renderObjectLayer("levelGeometry",planeCap)
	endPass()

	endEffect()
	

end

function renderSpecialGeometry()

	setEffect("media/efx_special.fx")
	setTechnique("Glass")
	beginEffect()
	
	--
	-- Render Glass
	--
	setVector("specularColor",0,0,0)
	setFloat("specularPower",1)
	setFloat("frequencyModulator",0.002)

	beginPass(0)
		renderObjectLayer("levelGeometry",windows)
	endPass()
	endEffect()
	
end

function renderEnemies()
	startCharacters()
	renderSoldier(ambientR,ambientG,ambientB)
	renderImp(ambientR,ambientG,ambientB)
	renderAirship(ambientR,ambientG,ambientB)
end