--
-- RENDER SCRIPT TEST
-- Note:  The render script MUST be wrapped inside of a function
-- called:  doRenderScript()
--

dofile("lev/common_renderscript.lua")

--
-- Render the skybox
--
function doRenderScript()

	if not haveCached then
		haveCached = true
		cacheEffectStates(lightX, lightY, lightZ, "media/tex_etc_skybox.jpg", "media/tex_lev_glass.png")
	end
	
	renderSkybox()
	renderGeometry()
	renderEnemies()
	renderProps(ambientR,ambientG,ambientB)
	renderWeapons()
	renderParticleEffects()
    renderViewspaceWeapon(ambientR,ambientG,ambientB)
    renderHUD()
    
end

function renderGeometry()
	setEffect("media/efx_level_sh.fx")
	setTexture("tDiffuse","lev/minestex.jpg")
	setTexture("tSpecular","lev/minestex.jpg")
	setVector("halfVector",0.0,0.0,-1.0)
	setVector("specularMax",0.35,0.25,0.15)
	setVector("diffuseMax",0.4,0.4,0.4)
	setTechnique("t0")
	beginEffect()
	beginPass(0)
		renderObjectLayer("level",0)
	endPass()
	endEffect()
end

function renderEnemies()
	startCharacters()
	renderElite(ambientR,ambientG,ambientB)
	renderToughElite(ambientR,ambientG,ambientB)
	renderBadassElite(ambientR,ambientG,ambientB)
	renderImp(ambientR,ambientG,ambientB)
	renderToughImp(ambientR,ambientG,ambientB)
	renderBadassImp(ambientR,ambientG,ambientB)
	renderSoldier(ambientR,ambientG,ambientB)
	renderToughSoldier(ambientR,ambientG,ambientB)
	renderBadassSoldier(ambientR,ambientG,ambientB)
	renderBadeLuno(ambientR,ambientG,ambientB)
	renderAirship(ambientR,ambientG,ambientB)
	renderTyrell(ambientR,ambientG,ambientB)
end