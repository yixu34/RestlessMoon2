--
-- COMMON RENDERSCRIPT FUNCTIONS
--
--  This file contains rendering functions
--  common to many levels in Restless Moon 2.
--

--
-- Character Material Definitions
--
arm = 1
flameThrower = 2
battleRifle = 3
muzzleFlash = 4
muzzleFlash2 = 5
rocketLauncher = 6
battleRifleScope = 7
shotgun = 8
rocketLauncherScope = 9

--
-- Muzzle Flash Variables
--
muzzleFlashTime = 0.06
muzzleFlashCount = 0.06

--
-- Renders the viewer weapon
--
function renderViewspaceWeapon(ar,ag,ab)
	clearZBuffer()
	setEffect("media/efx_viewspace.fx")
	setViewInverse("viewInverseMatrix")

	setVector("ambientColor",ar,ag,ab,1.0)
	setTexture("diffuseTexture","media/tex_firstperson_arm_dif.jpg")
	setVector("specularColor",0.1,0.1,0.11,0)
	setFloat("minPower",16.0)
	setFloat("rangePower",0.0)
	setTechnique("SimpleGlossMap")
	beginEffect()
	beginPass(0)
		renderObjectLayer("Selena",arm)
	endPass()
	endEffect()
	currWeapon = getRenderString("weapon")
	if currWeapon == "Rocket Launcher" then
		setTechnique("AdvancedGlossMap")
		setTexture("diffuseTexture","media/tex_wep_rlauncher_difmap.jpg")
		setTexture("glossTexture","media/tex_wep_rlauncher_glossmap.jpg")
		setVector("specularColor",6.5,6.5,6.6,0)
		setFloat("minPower",32.0)
		setFloat("rangePower",0.0)
		beginEffect()
		beginPass(0)
			renderObjectLayer("Selena",rocketLauncher)
		endPass()
		endEffect()
		setTechnique("Scope")
		setVector("scopeColor",1,0.5,0)
		beginEffect()
		beginPass(0)
			renderObjectLayer("Selena",rocketLauncherScope)
		endPass()
		endEffect()
	elseif currWeapon == "Flamethrower" then
		setTechnique("AdvancedGlossMap")
		setTexture("diffuseTexture","media/tex_wep_flamer_difmap.jpg")
		setTexture("glossTexture","media/tex_wep_flamer_glossmap.jpg")
		setVector("specularColor",6.5,6.5,6.6,0)
		setFloat("minPower",32.0)
		setFloat("rangePower",0.0)
		beginEffect()
		beginPass(0)
			renderObjectLayer("Selena",flameThrower)
		endPass()
		endEffect()
	elseif currWeapon == "Battle Rifle" then
		setTechnique("AdvancedGlossMap")
		setTexture("diffuseTexture","media/tex_wep_brifle_difmap.jpg")
		setTexture("glossTexture","media/tex_wep_brifle_glossmap.jpg")
		setVector("specularColor",6.5,6.5,6.6,0)
		setFloat("minPower",16.0)
		setFloat("rangePower",0.0)
		beginEffect()
		beginPass(0)
			renderObjectLayer("Selena",battleRifle)
		endPass()
		endEffect()
		setTechnique("Scope")
		setVector("scopeColor",0,0.7,0)
		beginEffect()
		beginPass(0)
			renderObjectLayer("Selena",battleRifleScope)
		endPass()
		endEffect()
		if getRenderNumber("isShooting") == 1 then
			muzzleFlashCount = 0
		else
			muzzleFlashCount = muzzleFlashCount + getDtSec()
		end
		if muzzleFlashCount < 0.06 then
			setTexture("flashTexture","media/tex_sfx_muzzleflash.jpg")
			setTechnique("MuzzleFlash")	
			beginEffect()
			beginPass(0)
				renderObjectLayer("Selena",muzzleFlash);
			endPass()
			endEffect()
		end
	elseif currWeapon == "Shotgun" then
		setTechnique("AdvancedGlossMap")
		setTexture("diffuseTexture","media/tex_wep_shotgun_difmap.jpg")
		setTexture("glossTexture","media/tex_wep_shotgun_glossmap.jpg")
		setVector("specularColor",6.5,6.5,6.6,0)
		setFloat("minPower",16.0)
		setFloat("rangePower",0.0)
		beginEffect()
		beginPass(0)
			renderObjectLayer("Selena",shotgun)
		endPass()
		endEffect()
		if getRenderNumber("isShooting") == 1 then
			muzzleFlashCount = 0
		else
			muzzleFlashCount = muzzleFlashCount + getDtSec()
		end
		if muzzleFlashCount < 0.06 then
			setTexture("flashTexture","media/tex_sfx_muzzleflash2.jpg")
		
			setTechnique("MuzzleFlash")
			
			beginEffect()
			beginPass(0)
				renderObjectLayer("Selena",muzzleFlash2);
			endPass()
			endEffect()
		end
	end
end

--
-- Computes a shadowmap onto shadowMap with layer objectLayer at (ex,ey,ez) targeted at
--   (tx,ty,tz) with up vector <ux,uy,uz> field-of-view fov and clip planes at near and far
--
function computeShadowMap(shadowMap,objectLayer,ex,ey,ez,tx,ty,tz,ux,uy,uz,fov,near,far)
	setRenderTarget(shadowMap)
	clearBuffers(1,1,1,1)
	beginScene()
	setEffect("media/efx_shadowmap.fx")
	setTechnique("CreateShadowMap")
	setViewProjection("lightViewProjMatrix",ex,ey,ez,tx,ty,tz,ux,uy,uz,fov,1,near,far)
	beginEffect()
	beginPass(0)
	renderShadowCasters(objectLayer)
	endPass()
	endEffect()
	endScene()
	restoreScreen()
end

--
-- Renders a shadowmap from shadowMap onto layer objectLayer at (ex,ey,ez) targeted at
--   (tx,ty,tz) with up vector <ux,uy,uz> field-of-view fov and clip planes at near and far
--   with depth bias
--
function renderShadows(shadowMap,objectLayer,ex,ey,ez,tx,ty,tz,ux,uy,uz,fov,near,far,bias)
	setEffect("media/efx_shadowmap.fx")
	setTechnique("ApplyShadowMap")
	setDynamicTexture("shadowMap",shadowMap)
	setViewProjection("lightViewProjMatrix",ex,ey,ez,tx,ty,tz,ux,uy,uz,fov,1,near,far)
	setFloat("bias",bias)
	beginEffect()
	beginPass(0)
	renderShadowReceivers(objectLayer)
	endPass()
	endEffect()
end

--
-- A good renderscript will dynamically override this to determine which
--  materials in the scene should cast shadows
--
function renderShadowCasters(objectLayer)
	renderObjectLayer(objectLayer,0)
end

--
-- A good renderscript will dynamically override this to determine which
--  materials in the scene should receive shadows
--
function renderShadowReceivers(objectLayer)
	renderObjectLayer(objectLayer,0)
end

--
-- Render particles
--
function renderParticleEffects()
	setEffect("media/efx_particles.fx")
	setTechnique("Explosion")
	setTexture("particleTexture","media/tex_sfx_fire.jpg")
	beginEffect()
	beginPass(0)
		renderObjectLayer("explosions")
	endPass()
	endEffect()

    setTechnique("Blood")
    beginEffect()
    beginPass(0)
		renderObjectLayer("blood")
    endPass()
    endEffect()

	setTechnique("Spark")
	setTexture("particleTexture","media/tex_sfx_dot.jpg")
	beginEffect()
	beginPass(0)
		renderObjectLayer("sparks")
	endPass()
	endEffect()

	setTechnique("Missile")
	setTexture("particleTexture", "media/tex_sfx_fire.jpg")
	beginEffect()
	beginPass(0)
		renderObjectLayer("missiles")
	endPass()
	endEffect()

end


function renderWeapons()
	-- Render battle rifle bullets
	setEffect("media/efx_tracerbullet.fx")
	setTechnique("Shiny")
	beginEffect()
	setCubeTexture("environmentTexture","media/tex_efx_environment.dds")
	setVector("bulletColor",1,1,1,1)
	setFloat("scale",1.0)
	beginPass(0)
		renderObjectLayer("Rocket Launcher",1)
		renderObjectLayer("TyrellWeapon",1)
		renderObjectLayer("Grenade",0)
		renderObjectLayer("GrenadeAmmo",0)
	endPass()
	setVector("bulletColor",0.1,0.1,0.1,1)
	beginPass(0)
		renderObjectLayer("Rocket Launcher",2)
	endPass()
	setFloat("scale",0.3)
	setVector("bulletColor",4,4,2,1)
	beginPass(0)
		renderObjectLayer("EliteWeapon",1,2)
		renderObjectLayer("ToughEliteWeapon",1,2)
		renderObjectLayer("BadassEliteWeapon",1,2)
	endPass()
	setVector("bulletColor",5,5,3,1)
	setFloat("scale",0.5)
	beginPass(0)
		renderObjectLayer("BadeLunoWeapon",1,2)
	endPass()
	endEffect()
	setTechnique("Bullet")
	setFloat("scale",1.0)
	setVector("bulletColor",1,0.9,0.8,1)
	-- Do a pass
	beginEffect()
	beginPass(0)
		renderObjectLayer("Battle Rifle")
		renderObjectLayer("Shotgun")
		renderObjectLayer("SoldierWeapon")
		renderObjectLayer("ToughSoldierWeapon")
		renderObjectLayer("BadassSoldierWeapon")
		renderObjectLayer("AirshipWeapon")
	endPass()
	setVector("bulletColor",0.4,0,0,1)
	beginPass(0)
		renderObjectLayer("Grenade",1)
		renderObjectLayer("GrenadeAmmo",1)
	endPass()
	endEffect()
	setTechnique("Flamer")
	setTexture("fireTexture","media/tex_sfx_fire.jpg")
	setFloat("scale",1.0)
	beginEffect()
	beginPass(0)
		renderObjectLayer("Flamethrower")
	endPass()
	setTexture("fireTexture","media/tex_sfx_impblast.jpg")
	setFloat("scale",0.2)
	beginPass(0)
		renderObjectLayer("ImpWeapon")
	endPass()
	setTexture("fireTexture","media/tex_sfx_impblast2.jpg")
	beginPass(0)
		renderObjectLayer("ToughImpWeapon")
	endPass()
		setTexture("fireTexture","media/tex_sfx_impblast3.jpg")
	beginPass(0)
		renderObjectLayer("BadassImpWeapon")
	endPass()
	endEffect()

end

--
-- Renders the Heads Up Display (HUD)
--
function renderHUD()

	whosTalking = getRenderString("talker")

	setEffect("media/efx_hud.fx")
	setTexture("hudTexture","media/tex_hud_bkgnd.png")
	setTechnique("renderHUD")
	beginEffect()
	beginPass(0)
		if whosTalking == "Nobody" then
			renderQuad(-1,-0.5,-1,-0.333333333,0.742,0.982,0.01,.26)
		elseif whosTalking == "Tyrell" then
			renderQuad(-1,-0.5,-1,-0.333333333,0.5,0.75,0.51,.76)
		elseif whosTalking == "Alex" then
			renderQuad(-1,-0.5,-1,-0.333333333,0.25,0.5,0.51,.76)
		elseif whosTalking == "Bade" then
			renderQuad(-1,-0.5,-1,-0.333333333,0.005,0.25,0.51,.76)
		elseif whosTalking == "Selena" then
			renderQuad(-1,-0.5,-1,-0.333333333,0.75,0.995,0.51,.76)
		end
		if currWeapon == "Battle Rifle" then
			renderQuad(0.5,1,-1,-0.333333333,0.76,1,0.262,.5)
		elseif currWeapon == "Flamethrower" then
			renderQuad(0.5,1,-1,-0.333333333,0.51,0.75,0.262,.5)
		elseif currWeapon == "Rocket Launcher" then
			renderQuad(0.5,1,-1,-0.333333333,0.26,0.5,0.262,.5)
		elseif currWeapon == "Shotgun" then
			renderQuad(0.5,1,-1,-0.333333333,0.01,0.25,0.262,.5)
		end
		renderQuad(-1,1,-1,-0.333333333,0,1,0.76,1)
		renderQuad(-0.04,0.04,-0.06,0.06,0.005,0.120,0.005,0.120)
	endPass()
	endEffect()
	setTexture("hudTexture","media/tex_hud_healthbar.png")
	setTechnique("renderBar")
	setVector("minPosition",-0.57,-1)
	setVector("maxPosition",-0.27,-0.76)
	health = getRenderNumber("health")
	setFloat("barAmount",health/100)
	beginEffect()
	beginPass(0)
		renderQuad(0,1,0,1)
	endPass()
	endEffect()
end

--
-- Remders the skybox with a particular texture set to it
--
function renderSkybox()
	--
	-- Render the skybox
	--   Writes over the z-buffer, too -- so you don't have to
	--   clear it!  One of the snazzy features of shader model 2.0
	--
	setEffect("media/efx_skybox.fx")
	beginEffect()
	beginPass(0)
		renderObjectLayer("skybox")
	endPass()
	endEffect()
end

--
-- Render the props under specified lighting conditions
function renderProps(ar,ag,ab)
	setEffect("media/efx_lightingmodels.fx")
	setVector("ambientColor",0.2*ar,0.2*ag,0.2*ab)
	setTechnique("PointLambertian")
	beginEffect()
	setVector("lightColor",0.8,0.75,0.73)
	beginPass(0)
		renderObjectLayer("crate",0)
	endPass()
	setVector("lightColor",0.2,0,0)
	setVector("ambientColor",ar*0.1,ag*0,ab*0)
	beginPass(0)
		renderObjectLayer("crate",1)
	endPass()
	endEffect()
	setVector("lightColor",1.15,1.05,1)
	setVector("ambientColor",ar*0.45,ag*0.35,ab*0.25)
	setFloat("frequencyModulator",1)
	setTechnique("PointPhongTexture")
	setVector("specularColor",1,1,1)
	setFloat("specularPower",16)
	beginEffect()
	setTexture("diffuseTexture","media/tex_wep_flamer_difmap.jpg")
	beginPass(0)
		renderObjectLayer("FlamethrowerAmmo")
	endPass()
	setTexture("diffuseTexture","media/tex_wep_shotgun_difmap.jpg")
	beginPass(0)
		renderObjectLayer("ShotgunAmmo",1)
	endPass()
	setTexture("diffuseTexture","media/tex_wep_brifle_difmap.jpg")
	beginPass(0)
		renderObjectLayer("BattleRifleAmmo")
	endPass()
	setTexture("diffuseTexture","media/tex_wep_rlauncher_difmap.jpg")
	beginPass(0)
		renderObjectLayer("RocketLauncherAmmo")
	endPass()
	setTexture("diffuseTexture","media/tex_item_medkit_difmap.jpg")
	beginPass(0)
		renderObjectLayer("Health")
	endPass()
	endEffect()
end

--
-- Setup the Characters for Rendering
--
function startCharacters()
	setEffect("media/efx_characters.fx")
end
	
--
-- Render the Elite Character
--
function renderElite(ar,ag,ab)
	setVector("lightColor",1,1,1)
    setVector("lightDiffuse", 0.6, 0.6, 0.6, 1.0)
	setTechnique("reflectNoTex")
	beginEffect()
	
	setVector("ambientColor",0.04*ar,0.04*ag,0.04*ab)
	setVector("diffuseColor",0.5,0.5,0.5)
	setVector("specularColor",7,7,6.7)
	setFloat("specularPower",32)
	setFloat("reflectivity",0.3)
	beginPass(0)
		renderObjectLayer("Elite",0)
	endPass()
	
	setVector("ambientColor",0.01*ar,0.01*ag,0.01*ab)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",10,10,10)
	setFloat("specularPower",32)
	setFloat("reflectivity",0.5)
	beginPass(0)
		renderObjectLayer("Elite",5)
	endPass()
	
	endEffect()
	setTechnique("phongNoTex")
	beginEffect()
	
	setVector("ambientColor",1,0,0)
	setVector("diffuseColor",0.0,0.0,0.0)
	setVector("specularColor",1,1,1)
	setFloat("specularPower",64)
	beginPass(0)
		renderObjectLayer("Elite",2)
	endPass()
	setVector("ambientColor",0.1*ar,0.1*ag,0.2*ab)
	setVector("diffuseColor",0.1,0.25,0.4)
	setVector("specularColor",1,1,1)
	setFloat("specularPower",64)
	beginPass(0)
		renderObjectLayer("Elite",4)
	endPass()
	
	endEffect()
	setTechnique("lambertianNoTex")
	beginEffect()
	
	setVector("ambientColor",0.05*ar,0.05*ag,0.05*ab)
	setVector("diffuseColor",0.2,0.2,0.2)
	beginPass(0)
		renderObjectLayer("Elite",3)
	endPass()
	endEffect()
end

--
-- Render the Imp Character
--
function renderImp(ar,ag,ab)
	setVector("lightColor",1,1,1)
    setVector("lightDiffuse", 0.6, 0.6, 0.6, 1.0)
	setTechnique("reflectNoTex")
	beginEffect()
	setVector("ambientColor",0.5*ar,0.5*ag,0.5*ab)
	setVector("diffuseColor",0.8,0.75,0.6)
	setVector("specularColor",6,6,6)
	setFloat("specularPower",32)
	setFloat("reflectivity",0.2)
	beginPass(0)
		renderObjectLayer("Imp",0)
	endPass()
	setVector("ambientColor",0.01*ar,0.01*ag,0.01*ab)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",1,1,1)
	setFloat("specularPower",8)
	setFloat("reflectivity",0.2)
	beginPass(0)
		renderObjectLayer("Imp",3)
	endPass()
	endEffect()
	setTechnique("phongNoTex")
	beginEffect()
	
	setVector("ambientColor",0,0,0)
	setVector("diffuseColor",0.0,0.0,0.0)
	setVector("specularColor",25,25,25)
	setFloat("specularPower",32)
	beginPass(0)
		renderObjectLayer("Imp",2)
	endPass()
	
	setVector("ambientColor",0.2*ar,0.4*ag,0.4*ab)
	setVector("diffuseColor",0.6,0.8,0.3)
	setVector("specularColor",5,5,5)
	setFloat("specularPower",32)
	beginPass(0)
		renderObjectLayer("Imp",5)
	endPass()
	
	endEffect()
	setTechnique("lambertianNoTex")
	beginEffect()
	
	setVector("ambientColor",0.05*ar,0.05*ag,0.05*ab)
	setVector("diffuseColor",0.2,0.2,0.2)
	beginPass(0)
		renderObjectLayer("Imp",4)
	endPass()
	endEffect()
end

--
-- Render the Soldier Character
--
function renderSoldier(ar,ag,ab)
	setVector("lightColor",1,1,1)
    setVector("lightDiffuse", 0.6, 0.6, 0.6, 1.0)
	setTechnique("reflectNoTex")
	beginEffect()
	setVector("ambientColor",0.1*ar,0.1*ag,0.1*ab)
	setVector("diffuseColor",0.4,0.42,0.45)
	setVector("specularColor",8,8,8)
	setFloat("specularPower",32)
	setFloat("reflectivity",0.2)
	beginPass(0)
		renderObjectLayer("Soldier",3)
	endPass()

	endEffect()
	setTechnique("phongNoTex")
	beginEffect()
	
	setVector("ambientColor",1,0,0)
	setVector("diffuseColor",0.0,0.0,0.0)
	setVector("specularColor",25,25,25)
	setFloat("specularPower",32)
	beginPass(0)
		renderObjectLayer("Soldier",5)
	endPass()
	
	setVector("ambientColor",0.1*ar,0.1*ag,0.4*ab)
	setVector("diffuseColor",0.1,0.1,0.4)
	setVector("specularColor",15,15,15)
	setFloat("specularPower",16)
	beginPass(0)
		renderObjectLayer("Soldier",1)
	endPass()
	
	setVector("diffuseColor",0.3,0.3,0.3)
	setVector("ambientColor",0.2*ar,0.2*ag,0.2*ab)
	setFloat("specularPower",8)
	beginPass(0)
		renderObjectLayer("Soldier",6)
	endPass()
	
	endEffect()
	setTechnique("lambertianNoTex")
	beginEffect()
	
	setVector("ambientColor",0.05*ar,0.05*ag,0.05*ab)
	setVector("diffuseColor",0.7,0.7,0.7)
	beginPass(0)
		renderObjectLayer("Soldier",4)
	endPass()
	endEffect()
end

--
-- Render the Tough Elite Character
--
function renderToughElite(ar,ag,ab)
	setVector("lightColor",1,1,1)
    setVector("lightDiffuse", 0.6, 0.6, 0.6, 1.0)
	setTechnique("reflectNoTex")
	beginEffect()
	
	setVector("ambientColor",0.8*ar,0.8*ag,0.8*ab)
	setVector("diffuseColor",1,1,1)
	setVector("specularColor",7,7,6.7)
	setFloat("specularPower",32)
	setFloat("reflectivity",0.4)
	beginPass(0)
		renderObjectLayer("toughElite",0)
	endPass()
	
	endEffect()
	setTechnique("phongNoTex")
	beginEffect()
	
	setVector("ambientColor",1,0,0)
	setVector("diffuseColor",0.0,0.0,0.0)
	setVector("specularColor",1,1,1)
	setFloat("specularPower",64)
	beginPass(0)
		renderObjectLayer("toughElite",2)
	endPass()
	setVector("ambientColor",0.3*ar,0.1*ag,0.1*ab)
	setVector("diffuseColor",0.6,0.1,0.1)
	setVector("specularColor",1,1,1)
	setFloat("specularPower",64)
	beginPass(0)
		renderObjectLayer("toughElite",4)
	endPass()
	setVector("ambientColor",0,0,0)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",1,1,1)
	setFloat("specularPower",32)
	beginPass(0)
		renderObjectLayer("toughElite",5)
	endPass()
	
	endEffect()
	setTechnique("lambertianNoTex")
	beginEffect()
	
	setVector("ambientColor",0.05*ar,0.05*ag,0.05*ab)
	setVector("diffuseColor",0.2,0.2,0.2)
	beginPass(0)
		renderObjectLayer("toughElite",3)
	endPass()
	endEffect()
end

--
-- Render the Tough Imp Character
--
function renderToughImp(ar,ag,ab)
	setVector("lightColor",1,1,1)
    setVector("lightDiffuse", 0.6, 0.6, 0.6, 1.0)
	setTechnique("reflectNoTex")
	beginEffect()
	setVector("ambientColor",0.7*ar,0.7*ag,0.7*ab)
	setVector("diffuseColor",1,1,1)
	setVector("specularColor",6,6,6)
	setFloat("specularPower",32)
	setFloat("reflectivity",0.2)
	beginPass(0)
		renderObjectLayer("toughImp",0)
	endPass()
	setVector("ambientColor",0.01*ar,0.01*ag,0.01*ab)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",1,1,1)
	setFloat("specularPower",8)
	setFloat("reflectivity",0.2)
	beginPass(0)
		renderObjectLayer("toughImp",3)
	endPass()
	endEffect()
	setTechnique("phongNoTex")
	beginEffect()
	
	setVector("ambientColor",0,0,0)
	setVector("diffuseColor",0.0,0.0,0.0)
	setVector("specularColor",25,25,25)
	setFloat("specularPower",32)
	beginPass(0)
		renderObjectLayer("toughImp",2)
	endPass()
	
	setVector("ambientColor",0.4*ar,0.4*ag,0.2*ab)
	setVector("diffuseColor",0.8,0.6,0.3)
	setVector("specularColor",5,5,5)
	setFloat("specularPower",32)
	beginPass(0)
		renderObjectLayer("toughImp",5)
	endPass()
	
	endEffect()
	setTechnique("lambertianNoTex")
	beginEffect()
	
	setVector("ambientColor",0.1*ar,0.1*ag,0.1*ab)
	setVector("diffuseColor",0.1,0.1,0.1)
	beginPass(0)
		renderObjectLayer("toughImp",4)
	endPass()
	endEffect()
end

--
-- Render the Tough Soldier Character
--
function renderToughSoldier(ar,ag,ab)
	setVector("lightColor",1,1,1)
    setVector("lightDiffuse", 0.6, 0.6, 0.6, 1.0)
	setTechnique("reflectNoTex")
	beginEffect()
	setVector("ambientColor",0.7*ar,0.7*ag,0.7*ab)
	setVector("diffuseColor",1,1,1)
	setVector("specularColor",8,8,8)
	setFloat("specularPower",32)
	setFloat("reflectivity",0.2)
	beginPass(0)
		renderObjectLayer("toughSoldier",3)
	endPass()

	endEffect()
	setTechnique("phongNoTex")
	beginEffect()
	
	setVector("ambientColor",1,0,0)
	setVector("diffuseColor",0.0,0.0,0.0)
	setVector("specularColor",25,25,25)
	setFloat("specularPower",32)
	beginPass(0)
		renderObjectLayer("toughSoldier",5)
	endPass()
	
	setVector("ambientColor",0.1*ar,0.1*ag,0.1*ab)
	setVector("diffuseColor",0.4,0.1,0.1)
	setVector("specularColor",15,15,15)
	setFloat("specularPower",16)
	beginPass(0)
		renderObjectLayer("toughSoldier",1)
	endPass()
	
	setVector("diffuseColor",0.3,0.3,0.3)
	setVector("ambientColor",0.2*ar,0.2*ag,0.2*ab)
	setFloat("specularPower",8)
	beginPass(0)
		renderObjectLayer("toughSoldier",6)
	endPass()
	
	endEffect()
	setTechnique("lambertianNoTex")
	beginEffect()
	
	setVector("ambientColor",0.05*ar,0.05*ag,0.05*ab)
	setVector("diffuseColor",0.2,0.2,0.2)
	beginPass(0)
		renderObjectLayer("toughSoldier",4)
	endPass()
	endEffect()
end



--
-- Render the Badass Elite Character
--
function renderBadassElite(ar,ag,ab)
	setVector("lightColor",1,1,1)
    setVector("lightDiffuse", 0.6, 0.6, 0.6, 1.0)
	setTechnique("reflectNoTex")
	beginEffect()
	
	setVector("ambientColor",0.1*ar,0.1*ag,0.1*ab)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",7,7,7)
	setFloat("specularPower",32)
	setFloat("reflectivity",0.1)
	beginPass(0)
		renderObjectLayer("badassElite",0)
	endPass()
	
	endEffect()
	setTechnique("phongNoTex")
	beginEffect()
	
	setVector("ambientColor",0,1,0)
	setVector("diffuseColor",0.0,0.0,0.0)
	setVector("specularColor",0,0,0)
	setFloat("specularPower",64)
	beginPass(0)
		renderObjectLayer("badassElite",2)
	endPass()
	setVector("ambientColor",0.1*ar,0.1*ag,0.1*ab)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",1,1,1)
	setFloat("specularPower",64)
	beginPass(0)
		renderObjectLayer("badassElite",4)
	endPass()
	setVector("ambientColor",0,0,0)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",1,1,1)
	setFloat("specularPower",32)
	beginPass(0)
		renderObjectLayer("badassElite",5)
	endPass()
	
	endEffect()
	setTechnique("lambertianNoTex")
	beginEffect()
	
	setVector("ambientColor",0.05*ar,0.05*ag,0.05*ab)
	setVector("diffuseColor",0.2,0.2,0.2)
	beginPass(0)
		renderObjectLayer("badassElite",3)
	endPass()
	endEffect()
end

--
-- Render the Badass Imp Character
--
function renderBadassImp(ar,ag,ab)
	setVector("lightColor",1,1,1)
    setVector("lightDiffuse", 0.6, 0.6, 0.6, 1.0)
	setTechnique("reflectNoTex")
	beginEffect()
	setVector("ambientColor",0.1*ar,0.1*ag,0.1*ab)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",7,7,7)
	setFloat("specularPower",32)
	setFloat("reflectivity",0.1)
	beginPass(0)
		renderObjectLayer("badassImp",0)
	endPass()
	setVector("ambientColor",0.01*ar,0.01*ag,0.01*ab)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",1,1,1)
	setFloat("specularPower",8)
	setFloat("reflectivity",0.2)
	beginPass(0)
		renderObjectLayer("badassImp",3)
	endPass()
	endEffect()
	setTechnique("phongNoTex")
	beginEffect()
	
	setVector("ambientColor",0,1,0)
	setVector("diffuseColor",0.0,0.0,0.0)
	setVector("specularColor",0,0,25)
	setFloat("specularPower",32)
	beginPass(0)
		renderObjectLayer("badassImp",2)
	endPass()
	
	setVector("ambientColor",0.1*ar,0.1*ag,0.1*ab)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",5,5,5)
	setFloat("specularPower",32)
	beginPass(0)
		renderObjectLayer("badassImp",5)
	endPass()
	
	endEffect()
	setTechnique("lambertianNoTex")
	beginEffect()
	
	setVector("ambientColor",0.1*ar,0.1*ag,0.1*ab)
	setVector("diffuseColor",0.1,0.1,0.1)
	beginPass(0)
		renderObjectLayer("badassImp",4)
	endPass()
	endEffect()
end

--
-- Render the Badass Soldier Character
--
function renderBadassSoldier(ar,ag,ab)
	setVector("lightColor",1,1,1)
    setVector("lightDiffuse", 0.6, 0.6, 0.6, 1.0)
	setTechnique("reflectNoTex")
	beginEffect()
	setVector("ambientColor",0.1*ar,0.1*ag,0.1*ab)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",7,7,7)
	setFloat("specularPower",32)
	setFloat("reflectivity",0.1)
	beginPass(0)
		renderObjectLayer("badassSoldier",3)
	endPass()

	endEffect()
	setTechnique("phongNoTex")
	beginEffect()
	
	setVector("ambientColor",0,1,0)
	setVector("diffuseColor",0.0,0.0,0.0)
	setVector("specularColor",0,0,0)
	setFloat("specularPower",32)
	beginPass(0)
		renderObjectLayer("badassSoldier",5)
	endPass()
	
	setVector("ambientColor",0.1*ar,0.1*ag,0.1*ab)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",15,15,15)
	setFloat("specularPower",16)
	beginPass(0)
		renderObjectLayer("badassSoldier",1)
	endPass()
	
	setVector("diffuseColor",0.3,0.3,0.3)
	setVector("ambientColor",0.2*ar,0.2*ag,0.2*ab)
	setFloat("specularPower",8)
	beginPass(0)
		renderObjectLayer("badassSoldier",6)
	endPass()
	
	endEffect()
	setTechnique("lambertianNoTex")
	beginEffect()
	
	setVector("ambientColor",0.05*ar,0.05*ag,0.05*ab)
	setVector("diffuseColor",0.2,0.2,0.2)
	beginPass(0)
		renderObjectLayer("badassSoldier",4)
	endPass()
	endEffect()
end

--
-- Render Bade Luno
--
function renderBadeLuno(ar,ag,ab)
	setVector("lightColor",1,1,1)
    setVector("lightDiffuse", 0.3, 0.3, 0.3, 1.0)
	setTechnique("reflectNoTex")
	beginEffect()
	setVector("ambientColor",0.01*ar,0.01*ag,0.01*ab)
	setVector("diffuseColor",0.05,0.05,0.05)
	setVector("specularColor",12,4,4)
	setFloat("specularPower",32)
	setFloat("reflectivity",0.3)
	beginPass(0)
		renderObjectLayer("badeLuno",2)
	endPass()

	endEffect()
	setTechnique("phongNoTex")
	beginEffect()
	
	beginPass(0)
		renderObjectLayer("badeLuno",4)
	endPass()
	
	setVector("ambientColor",0.1*ar,0.1*ag,0.1*ab)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",15,15,15)
	setFloat("specularPower",16)
	beginPass(0)
		renderObjectLayer("badeLuno",1)
	endPass()
	
	setVector("ambientColor",0.3*ar,0.3*ag,0.3*ab)
	setVector("diffuseColor",0.5,0.45,0.2)
	beginPass(0)
		renderObjectLayer("badeLuno",6)
	endPass()
	
	endEffect()
	setTechnique("lambertianNoTex")
	beginEffect()
	

	setVector("ambientColor",0.3*ar,0.3*ag,0.3*ab)
	setVector("diffuseColor",0.3,0.3,0.3)
	beginPass(0)
		renderObjectLayer("badeLuno",3)
	endPass()
	
	setVector("ambientColor",1,0,0)
	setVector("diffuseColor",0.0,0.0,0.0)
	setVector("specularColor",0,0,0)
	setFloat("specularPower",32)
	beginPass(0)
		renderObjectLayer("badeLuno",5)
	endPass()
	
	endEffect()
end


--
-- Render Tyrell Garrison
--
function renderTyrell(ar,ag,ab)
	setVector("lightColor",1,1,1)
    setVector("lightDiffuse", 0.3, 0.3, 0.3, 1.0)
	setTechnique("reflectNoTex")
	beginEffect()
	
	setVector("ambientColor",0,0,0)
	setVector("diffuseColor",1,0,0)
	setVector("specularColor",0,0,0)
	setFloat("specularPower",32)
	setFloat("reflectivity",1)
	beginPass(0)
		renderObjectLayer("Tyrell",5)
	endPass()
	
	setVector("ambientColor",0.01*ar,0.01*ag,0.01*ab)
	setVector("diffuseColor",0.7,0.3,0.3)
	setVector("specularColor",12,4,4)
	setFloat("specularPower",32)
	setFloat("reflectivity",0.2)
	beginPass(0)
		renderObjectLayer("Tyrell",2)
	endPass()

	endEffect()
	setTechnique("phongNoTex")
	beginEffect()
	
	beginPass(0)
		renderObjectLayer("Tyrell",4)
	endPass()
	
	setVector("ambientColor",0.1*ar,0.1*ag,0.1*ab)
	setVector("diffuseColor",0.2,0.2,0.2)
	setVector("specularColor",15,15,15)
	setFloat("specularPower",16)
	beginPass(0)
		renderObjectLayer("Tyrell",1)
	endPass()
	
	setVector("ambientColor",0.45*ar,0.3*ag,0.2*ab)
	setVector("diffuseColor",0.95,0.75,0.55)
	beginPass(0)
		renderObjectLayer("Tyrell",7)
	endPass()
	
	setVector("diffuseColor",1,0.7,0.6)
	beginPass(0)
		renderObjectLayer("Tyrell",6)
	endPass()
	
	endEffect()
	setTechnique("lambertianNoTex")
	beginEffect()
	

	setVector("ambientColor",0.3*ar,0.3*ag,0.3*ab)
	setVector("diffuseColor",0.3,0.3,0.3)
	beginPass(0)
		renderObjectLayer("Tyrell",3)
	endPass()
	
	endEffect()
end

--
-- Render the Airship Enemy
--
function renderAirship(ar,ag,ab)
	setVector("lightColor",1,1,1)
	setTechnique("reflectNoTex")
	beginEffect()
	setVector("ambientColor",0.5*ar,0.5*ag,0.5*ab)
	setVector("diffuseColor",0.8,0.75,0.6)
	setVector("specularColor",6,6,6)
	setFloat("specularPower",32)
	setFloat("reflectivity",0.2)
	beginPass(0)
		renderObjectLayer("Airship",3,5)
	endPass()
	
	setVector("ambientColor",0,0,0)
	setVector("diffuseColor",0,0,0)
	setVector("specularColor",3,3,3)
	setFloat("reflectivity",1)
	beginPass(0)
		renderObjectLayer("Airship",6)
	endPass()

	endEffect()
	setTechnique("phongNoTex")
	beginEffect()
	
	setVector("ambientColor",0.1*ar,0.1*ag,0.1*ab)
	setVector("diffuseColor",0.1,0.1,0.1)
	setVector("specularColor",15,15,15)
	setFloat("specularPower",16)
	beginPass(0)
		renderObjectLayer("Airship",4)
	endPass()
	
	setVector("diffuseColor",0.3,0.3,0.3)
	setVector("ambientColor",0.2*ar,0.2*ag,0.2*ab)
	setFloat("specularPower",8)
	beginPass(0)
		--renderObjectLayer("Airship",6)
	endPass()
	
	endEffect()
	setTechnique("lambertianNoTex")
	beginEffect()
	
	setVector("ambientColor",0.05*ar,0.05*ag,0.05*ab)
	setVector("diffuseColor",0.2,0.2,0.2)
	beginPass(0)
		renderObjectLayer("Airship",1,2)
	endPass()
	endEffect()
end

--
-- cache up some effect states to minimize state changes,
--  and perhaps gain some framerate
function cacheEffectStates(lightX, lightY, lightZ, skyboxTexture, glassTexture)
	setEffect("media/efx_characters.fx")
	setCubeTexture("environmentTexture","media/tex_efx_environment.dds")
	setVector("lightPosition",lightX,lightY,lightZ)
	
	setEffect("media/efx_lightingmodels.fx")
	setVector("lightPosition",lightX,lightY,lightZ)
	
	setEffect("media/efx_skybox.fx")
	setTexture("skyboxTexture", skyboxTexture)
	setTechnique("t0")
	
	setEffect("media/efx_special.fx")
	setVector("lightPosition",lightX, lightY, lightZ)
	setTexture("glassTexture",glassTexture)
	
	setEffect("media/efx_viewspace.fx")
	setProjection("projectionMatrix",1.0,1.333,1.0,1000.0)
	setCubeTexture("environmentTexture","media/tex_efx_environment.dds")
	setVector("lightPosition",lightX, lightY, lightZ)
	
end
	
	

--
-- Post process helper - copy a texture onto a texture
--
function textureToTexture(sourceTex,destTex)
	setRenderTarget(destTex)
	beginScene()
	setEffect("media/efx_postprocess.fx")
	setDynamicTexture("sourceTexture",sourceTex)
	setTechnique("renderOver")
	beginEffect()
	beginPass(0)
		renderQuad(-1,1,-1,1)
	endPass()
	endEffect()
	endScene()
end

--
-- Post process helper - copy a texture onto the framebuffer
--
function textureToScreen(sourceTex)
	setEffect("media/efx_postprocess.fx")
	setDynamicTexture("sourceTexture",sourceTex)
	setTechnique("renderOver")
	beginEffect()
	beginPass(0)
		renderQuad(-1,1,-1,1)
	endPass()
	endEffect()
end

function renderScreenPostFX()

	isDead = getRenderNumber("isPlayerDead")

	if isDead == 1 then
		setEffect("media/efx_postProcess.fx")
		setTexture("sourceTexture","media/tex_efx_red_full.bmp")
		setTechnique("renderOverAlpha")
		setFloat("alpha",0.5)
		beginEffect()
		beginPass(1)
			renderQuad(-1,1,-1,1)
		endPass()
		endEffect()
	else
		lastHitTime = getRenderNumber("lastHitTime")
		spriteTime = 0.3
		if lastHitTime < spriteTime then
			setEffect("media/efx_postProcess.fx")
			setTexture("sourceTexture","media/tex_sfx_red.jpg")
			setTechnique("renderOverAlpha")
			setFloat("alpha",1-(lastHitTime/spriteTime))
			beginEffect()
			beginPass(1)
				renderQuad(-1,1,-1,1)
			endPass()
			endEffect()
		end
	end
	
end
