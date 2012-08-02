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
	
	renderSpecialGeometry()
	renderEnemies()
	renderProps(ambientR,ambientG,ambientB)
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
		renderObjectLayer("noShadowMap",2)
		renderObjectLayer("shadowMap1",2)
		renderObjectLayer("shadowMap2",2)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_fill_caution.jpg")
	beginPass(0)
		renderObjectLayer("noShadowMap",1)
		renderObjectLayer("shadowMap1",1)
		renderObjectLayer("shadowMap2",1)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_fill_concreteB.jpg")
	setVector("lightColor",0.6,0.6,0.6)
	beginPass(0)
		renderObjectLayer("noShadowMap",3)
		renderObjectLayer("shadowMap1",3)
		renderObjectLayer("shadowMap2",3)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_fill_rubble.jpg")
	beginPass(0)
		renderObjectLayer("noShadowMap",7)
		renderObjectLayer("shadowMap2",5,6)
	endPass()
	
	setVector("lightColor",1.0,1.0,1.0)
	--
	-- Render Chainlink
	--
	setTexture("diffuseTexture","media/tex_lev_chainlink.png")
	setFloat("frequencyModulator",0.015)
	beginPass(1)
		renderObjectLayer("noShadowMap",4)
		renderObjectLayer("shadowMap1",5)
	endPass()
	
	--
	endEffect()
	
	setTechnique("PointPhongTexture")
	
	beginEffect()
	--
	-- Render Metal Fill
	--
	setTexture("diffuseTexture","media/tex_lev_fill_metalA.jpg")
	setVector("specularColor",0.5,0.5,0.5)
	setFloat("specularPower",16)
	setFloat("frequencyModulator",0.015)
	beginPass(0)
		renderObjectLayer("noShadowMap",5,6)
		renderObjectLayer("shadowMap1",4)
		renderObjectLayer("shadowMap2",4)
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
	setFloat("bumpiness",50)
	beginPass(0)
		renderObjectLayer("noShadowMap",8)
		renderObjectLayer("shadowMap1",5)
		renderObjectLayer("shadowMap2",7)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_fill_asphalt.jpg")
	setTexture("normalTexture","media/tex_lev_roughness2_normap.bmp")
	setVector("specularColor",1,1,1)
	setFloat("frequencyModulator",0.0035)
	setFloat("bumpiness",6)
	beginPass(0)
		renderObjectLayer("noShadowMap",0)
		renderObjectLayer("shadowMap1",0)
		renderObjectLayer("shadowMap2",0)
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
		renderObjectLayer("noShadowMap",9)
		renderObjectLayer("shadowMap1",6)
		renderObjectLayer("shadowMap2",8)
	endPass()
	endEffect()
	
end

function renderEnemies()
	startCharacters()
	renderToughElite(ambientR,ambientG,ambientB)
	renderToughSoldier(ambientR,ambientG,ambientB)
	renderToughImp(ambientR,ambientG,ambientB)
end