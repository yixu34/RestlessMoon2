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
		computeShadowMap("ds09 sm1","sm1Geometry",
					      -5400,1300,-4100,  -5400,100,-4100,  1,0,0,
					      2.1,100,1700)
		haveComputedShadows = true
	end
	
	renderShadows("ds09 sm1","sm1Geometry",
				  -5400,1300,-4100,  -5400,100,-4100,  1,0,0,
				  2.1,100,1700,.00065)
	
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
	renderObjectLayer(objectLayer,0,1)
end

function renderShadowReceivers(objectLayer)
	renderObjectLayer(objectLayer,0,1)
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
	setVector("lightColor",1,1,1)
	setFloat("frequencyModulator",0.0009)
	beginPass(0)
		renderObjectLayer("levelGeometry",cementFill1)
		renderObjectLayer("sm1Geometry",0)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_fill_concreteB.jpg")
	beginPass(0)
		renderObjectLayer("levelGeometry",cementFill2,cementFill3)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_fill_caution.jpg")
	beginPass(0)
		renderObjectLayer("levelGeometry",cautionStripe)
	endPass()
	
	--
	-- Render Chainlink
	--
	setTexture("diffuseTexture","media/tex_lev_chainlink.png")
	setFloat("frequencyModulator",0.015)
	beginPass(1)
		renderObjectLayer("levelGeometry",chainlink)
	endPass()
	
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
	endPass()
	
	--
	-- Render Simple Metal
	--
	setVector("specularColor",0.5,0.5,0.5)
	beginPass(0)
		renderObjectLayer("levelGeometry",metalTwo,metalThree)
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
		renderObjectLayer("sm1Geometry",1)
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
		renderObjectLayer("sm1Geometry",2)
	endPass()
	endEffect()
	
end

function renderEnemies()
	startCharacters()
	renderElite(ambientR,ambientG,ambientB)
	renderImp(ambientR,ambientG,ambientB)
	renderSoldier(ambientR,ambientG,ambientB)
end