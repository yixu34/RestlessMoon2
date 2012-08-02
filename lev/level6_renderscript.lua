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
		renderObjectLayer("levelGeometry",cementFill1,concreteFill1)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_fill_concreteB.jpg")
	setVector("lightColor",0.6,0.6,0.6)
	beginPass(0)
		renderObjectLayer("levelGeometry",cementFill2,concreteFill2)
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
		renderObjectLayer("levelGeometry",metalOne,metalTwo)
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
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_fill_asphalt.jpg")
	setTexture("normalTexture","media/tex_lev_roughness2_normap.bmp")
	setVector("specularColor",1,1,1)
	setFloat("frequencyModulator",0.0035)
	setFloat("bumpiness",6)
	beginPass(0)
		renderObjectLayer("levelGeometry",asphalt)
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
	renderToughElite(ambientR,ambientG,ambientB)
	renderToughSoldier(ambientR,ambientG,ambientB)
	renderToughImp(ambientR,ambientG,ambientB)
	renderAirship(ambientR,ambientG,ambientB)
	renderBadassImp(ambientR,ambientG,ambientB)
end