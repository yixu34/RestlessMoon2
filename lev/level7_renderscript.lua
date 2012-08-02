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
		renderObjectLayer("levelGeometry",cementFill1,cementFill3)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_fill_caution.jpg")
	beginPass(0)
		renderObjectLayer("levelGeometry",cautionStripe)
	endPass()
	
	
	setTexture("diffuseTexture","media/tex_lev_fill_concreteB.jpg")
	setVector("lightColor",0.6,0.6,0.6)
	beginPass(0)
		renderObjectLayer("levelGeometry",cementFill2,plating)
	endPass()
	
	setTexture("diffuseTexture","media/tex_lev_chainlink.png")
	setFloat("frequencyModulator",0.0095)
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
	setFloat("frequencyModulator",0.0015)
	beginPass(0)
		renderObjectLayer("levelGeometry",metalOne,metalTwo)
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
	renderTyrell(ambientR,ambientG,ambientB)
end