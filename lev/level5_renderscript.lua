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
	
	if not haveComputedShadows then
		computeShadowMap("ds10 sm1","sm1Geometry",
					      5350,2250,0,  5350,0,0,  1,0,0,
					      2.1,100,4000)
		haveComputedShadows = true
	end
	
	renderShadows("ds10 sm1","sm1Geometry",
				  5350,2250,0,  5350,0,0,  1,0,0,
				  2.1,100,4000,.00045)
	
	renderSpecialGeometry()
	renderEnemies()
	renderProps(ambientR,ambientG,ambientB)
	renderWeapons()
	renderParticleEffects()
	renderViewspaceWeapon(ambientR,ambientG,ambientB)
	renderScreenPostFX()
	renderHUD()
	
	--textureToScreen("ds10 seg2sm2")
	
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
	setTexture("diffuseTexture","media/tex_lev_fill_concreteA.jpg")
	setVector("lightColor",1.5,1.5,1.5)
	setFloat("frequencyModulator",0.0009)
	beginPass(0)
		renderObjectLayer("levelGeometry",cementFill1)
		renderObjectLayer("sm1Geometry",cementFill1)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_fill_concreteB.jpg")
	setVector("lightColor",0.5,0.5,0.5)
	beginPass(0)
		renderObjectLayer("levelGeometry",cementFill2)
		renderObjectLayer("sm1Geometry",cementFill2)
	endPass()
	
	setVector("lightColor",1,1.1,1)
	beginPass(0)
		renderObjectLayer("levelGeometry",cementFill3)
		renderObjectLayer("sm1Geometry",cementFill3)
	endPass()
	
	setVector("lightColor",1.0,1.0,1.0)
	--
	-- Render Chainlink
	--
	setTexture("diffuseTexture","media/tex_lev_chainlink.png")
	setFloat("frequencyModulator",0.015)
	beginPass(1)
		renderObjectLayer("levelGeometry",chainlink)
		renderObjectLayer("sm1Geometry",chainlink)
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
		renderObjectLayer("levelGeometry",metalOne)
		renderObjectLayer("sm1Geometry",metalOne)
	endPass()
	
	--
	-- Render Simple Metal
	--
	setVector("specularColor",0.5,0.5,0.5)
	beginPass(0)
		renderObjectLayer("levelGeometry",metalTwo)
		renderObjectLayer("sm1Geometry",metalTwo)
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
		renderObjectLayer("levelGeometry",plating)
		renderObjectLayer("sm1Geometry",plating)
	endPass()
	
	endEffect()
	
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
		renderObjectLayer("sm1Geometry",windows)
	endPass()
	endEffect()
	
end

function renderEnemies()
	startCharacters()
	renderToughElite(ambientR,ambientG,ambientB)
	renderToughImp(ambientR,ambientG,ambientB)
	renderToughSoldier(ambientR,ambientG,ambientB)
end