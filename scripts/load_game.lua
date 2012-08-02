--
-- Load Game screen
--
function createLoadGame()
    local frameName = "load_game"
    createFrame(frameName, 6)
    --processLoadGame(frameName)

	setRenderScriptFunction(frameName, 'renderLoadGame("' .. frameName ..  '")')
    
    -- LEVEL 1
    createSpriteWidget(
		frameName, .520742, .335, .25, .03125, 0, .3125, 0.5, .375,
		'startNewGame("lev/level1_loadscript.lev", 1)')
		
	-- LEVEL 2
    createSpriteWidget(
		frameName, .520742, .37, .25, .03125, 0, .375, 0.5, .4375,
		'startNewGame("lev/level2_loadscript.lev", 1)')
		
	-- LEVEL 3
	createSpriteWidget(
		frameName, .520742, .405, .25, .03125, 0, .4375, 0.5, .5,
		'startNewGame("lev/level3_loadscript.lev", 1)')
		
	-- LEVEL 4
	createSpriteWidget(
		frameName, .520742, .44, .25, .03125, 0, .5, 0.5, .5625,
		'startNewGame("lev/level4_part1_loadscript.lev", 1)')
		
	-- LEVEL 5
	createSpriteWidget(
		frameName, .520742, .475, .25, .03125, 0, .5625, 0.5, .625,
		'startNewGame("lev/level5_loadscript.lev", 1)')
		
	-- LEVEL 6
	createSpriteWidget(
		frameName, .520742, .51, .25, .03125, 0, .625, 0.5, .6875,
		'startNewGame("lev/level6_loadscript.lev", 1)')

	-- LEVEL 7
	createSpriteWidget(
		frameName, .520742, .545, .25, .03125, 0, .6875, 0.5, .75,
		'startNewGame("lev/level7_loadscript.lev", 1)')
		
	-- LEVEL 8
	createSpriteWidget(
		frameName, .520742, .58, .25, .03125, 0, .75, 0.5, .8125,
		'startNewGame("lev/level8_loadscript.lev", 1)')

	-- LEVEL 9
	createSpriteWidget(
		frameName, .520742, .615, .25, .03125, 0, .8125, 0.5, .875,
		'startNewGame("lev/level9_loadscript.lev", 1)')
		
	-- LEVEL 10
	createSpriteWidget(
		frameName, .520742, .65, .25, .03125, 0, .875, 0.5, .9375,
		'startNewGame("lev/level10_loadscript.lev", 1)')

	-- Back
	createSpriteWidget(
		frameName, .520742, .7, .25, .03125, 0, .9375, 0.5, 1,
		'setFrameEnabled("load_game", 0); setFrameEnabled(getMainMenuScreen(), 1)')
    
    playMusic("media/music/IntroMusic.mp3")
    setFrameEnabled(frameName, 0)
end
createLoadGame()

menuLightTime = 0

function renderLoadGame(frameName)
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
		renderQuad(cursorX,cursorX+0.075,cursorY,cursorY-0.1,0,0.95,0.95,0)
	endPass()
	endEffect()
	
end
