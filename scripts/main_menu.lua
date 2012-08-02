--
-- Main menu
--
function createMainMenu()
    local frameName = "main_menu"

    createFrame(frameName, 5)
	setRenderScriptFunction(frameName, 'renderMainMenu("' .. frameName ..  '")')
    
    -- NEW GAME
    createSpriteWidget(
		frameName, .520742, .3433, .25, .03125, 0, 0, 0.5, .0625,
		'startNewGame("lev/level1_loadscript.lev", 0)')
		
	-- LOAD GAME
    createSpriteWidget(
		frameName, .520742, .3933, .25, .03125, 0, .0625, 0.5, .125,
		'setFrameEnabled("main_menu", 0); setFrameEnabled("load_game", 1)')
		
	-- CREDITS
	createSpriteWidget(
		frameName, .520742, .4433, .25, .03125, 0, .125, 0.5, .1875,
		'setFrameEnabled("main_menu", 0); setFrameEnabled("credits", 1)')
		
	-- QUIT
	createSpriteWidget(
		frameName, .520742, .4933, .25, .03125, 0, .1875, 0.5, .25,
		'quit()')
		
    --processLoadGame(frameName)

    playMusic("media/music/IntroMusic.mp3")
    setFrameEnabled(frameName, 1)
end
createMainMenu()

menuLightTime = 0

function renderMainMenu(frameName)
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
	setTexture("diffuseTexture","media/tex_ui_offbuttons.png")
	beginEffect()
	beginPass(1)
		renderMouseOver(frameName, 0)
	endPass()
	setTexture("diffuseTexture","media/tex_ui_onbuttons.png")
	beginPass(1)
		renderMouseOver(frameName, 1)
	endPass()
	setTexture("diffuseTexture","media/tex_ui_cursor.png")
	beginPass(0)
		renderQuad(cursorX,cursorX+0.075,cursorY,cursorY-0.1,0,.995,.995,0)
	endPass()
	endEffect()
	
end
