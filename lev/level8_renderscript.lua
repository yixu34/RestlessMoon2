--
-- LEVEL 8
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
		renderObjectLayer("levelGeometry",cementFill1,metalOne,metalTwo)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_fill_concreteB.jpg")
	setVector("lightColor",0.6,0.6,0.6)
	beginPass(0)
		renderObjectLayer("levelGeometry",cementFill2,cementFill3,cementFill4,plating)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_fill_caution.jpg")
	beginPass(0)
		renderObjectLayer("levelGeometry",cautionStripe)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_moonrock.jpg")
	setFloat("frequencyModulator",0.0004)
	setVector("lightColor",0.7,0.7,0.7)
	beginPass(0)
		renderObjectLayer("levelGeometry",moonRock)
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
	renderBadassSoldier(ambientR,ambientG,ambientB)
	renderBadassImp(ambientR,ambientG,ambientB)
end