--
-- Credits screen
--
function createCredits()
    local frameName = "credits"

    createFrame(frameName, 8)
	setRenderScriptFunction(frameName, 'renderCredits("' .. frameName ..  '")')

--	createTextLabel(frameName, "Diego Asturias", 0.65, 0.35, 1)
--	createTextLabel(frameName, "Walter King", 0.65, 0.4, 1)
--	createTextLabel(frameName, "Jason Liu", 0.65, 0.45, 1)
--	createTextLabel(frameName, "Don Holden", 0.65, 0.5, 1)
--	createTextLabel(frameName, "Yi Xu", 0.65, 0.55, 1)

	-- Back
	createSpriteWidget(
		frameName, 0, 0, 1, 1, 0, 0, 0, 0,
		'setFrameEnabled("credits", 0); setFrameEnabled(getMainMenuScreen(), 1)')

    playMusic("media/music/IntroMusic.mp3")
    setFrameEnabled(frameName, 0)
end
createCredits()

menuLightTime = 0

function renderCredits(frameName)
	setEffect("media/efx_ui.fx")
	setTexture("diffuseTexture","media/tex_ui_mainmenu_back.jpg")
	setTexture("heightNormalTexture","media/tex_ui_mainmenu_backbump.png")
	
	menuLightTime = menuLightTime + 0.4*getDtSec()
	
	setVector("lightPosition",math.cos(menuLightTime),math.sin(menuLightTime),1)
	setVector("eyePosition",0,0,1)
	setFloat("minimumHeight",0)
	setFloat("rangeHeight",1)
	
	setVector("ambientColor",0.7,0.7,0.7)
	setVector("diffuseColor",0.5,0.5,0.7)
	setVector("specularColor",0.4,0.4,1.1)
	setFloat("phongExponent",8)
	
	setTechnique("menuRenderBumpy")
	
	cursorX,cursorY = getCursorPos()
	
	beginEffect()
	beginPass(0)
		renderQuad(-1,1,-1,1,0,1,0,0.75)
	endPass()
	endEffect()
	
	setTechnique("menuRenderFlat")
	setTexture("diffuseTexture","media/tex_ui_credits.png")
	beginEffect()
	beginPass(0)
		renderQuad(-1,1,-1,1,0,1,0,0.75)
	endPass()
	endEffect()
	
end
