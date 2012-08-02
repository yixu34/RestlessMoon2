
--
-- Continue game screen
--
function createContinueGame()
    local frameName = "continue_game"

    createFrame(frameName, 7)
	setRenderScriptFunction(frameName, 'renderContinueGame("' .. frameName ..  '")')

	-- BACK TO GAME
	createSpriteWidget(
		frameName, .520742, .3433, .25, .03125, 0, .625, 1, .75,
		'setFrameEnabled("continue_game", 0); setFrameEnabled("game", 1)')
    
    -- NEW GAME
    createSpriteWidget(
		frameName, .520742, .3933, .25, .03125, 0, 0, 1, .125,
		'startNewGame("lev/level1_loadscript.lev", 0)')
		
	-- LOAD GAME
    createSpriteWidget(
		frameName, .520742, .4433, .25, .03125, 0, .125, 1, .25,
		'setFrameEnabled("continue_game", 0); setFrameEnabled("load_game", 1)')
		
	-- OPTIONS (TODO:  Make one of these buttons actually say "continue game")
    --createSpriteWidget(
	--	frameName, .520742, .4933, .25, .03125, 0, .25, 1, .375,
	--	'setFrameEnabled("continue_game", 0); setFrameEnabled("game", 1)')
		
	-- CREDITS
	createSpriteWidget(
		frameName, .520742, .4933, .25, .03125, 0, .375, 1, .5,
		'setFrameEnabled("continue_game", 0); setFrameEnabled("credits", 1)')
		
	-- QUIT
	createSpriteWidget(
		frameName, .520742, .5433, .25, .03125, 0, .5, 1, .625,
		'quit()')

		
    --processLoadGame(frameName)

    playMusic("media/music/IntroMusic.mp3")
    setFrameEnabled(frameName, 0)
end
createContinueGame()

menuLightTime = 0

function renderContinueGame(frameName)
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
	setTexture("diffuseTexture","media/tex_ui_mainmenu_offbuttons.png")
	beginEffect()
	beginPass(1)
		renderMouseOver(frameName, 0)
	endPass()
	setTexture("diffuseTexture","media/tex_ui_mainmenu_onbuttons.png")
	beginPass(1)
		renderMouseOver(frameName, 1)
	endPass()
	setTexture("diffuseTexture","media/tex_ui_cursor.png")
	beginPass(0)
		renderQuad(cursorX,cursorX+0.075,cursorY,cursorY-0.1,0,0.95,0.95,0)
	endPass()
	endEffect()
	
end
