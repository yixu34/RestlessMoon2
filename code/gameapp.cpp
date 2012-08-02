#include "common.hpp"
#include "gameapp.hpp"
#include "scripting.hpp"
#include "eventmanager.hpp"
#include "graphics.hpp"
#include "framemanager.hpp"
#include "frame.hpp"
#include "widget.hpp"
#include "util.hpp"
#include "input.hpp"
#include "timer.hpp"
#include "audio.hpp"
#include "processmanager.hpp"
#include "text.hpp"

#include "sound.hpp"
#include "music.hpp"

#include "mesh.hpp"
#include "camera.hpp"
#include "inputhandler.hpp"
#include "renderer.hpp"
#include "process.hpp"
#include "console.hpp"
#include "command.hpp"
#include "game.hpp"
#include "cvar.hpp"
#include "bindings.hpp"
#include "speech.hpp"

namespace 
{
	bool quitting = false;
	bool hasFocus = true;
	keymod mod    = 0;

	CVar<float> showfps("showfps", 0);
	CVar<float> showpos("showpos", 0);
}

//
// Windows message handling function.
//
LRESULT CALLBACK WndProc(
	HWND hwnd, 
	UINT msg, 
	WPARAM wParam, 
	LPARAM lParam)
{
	switch(msg)
	{
		case WM_QUIT: {
			quitting = true;
			break;
		}

		case WM_SYSCOMMAND: {
			if (wParam == SC_CLOSE)
			{
				gameApp->shutdown();
				DestroyWindow(hwnd);
				PostQuitMessage(0);
			}
			break;
		}

		case WM_ACTIVATE:
			if (wParam == WA_INACTIVE)
				hasFocus = false;
			else
				hasFocus = true;
			break;

		case WM_CHAR: {
			frameManager->keyDown(wParam, mod);
			break;
		}

		//case WM_KEYDOWN: {
		//	//mod = Input::updateKeymods(mod);

		//	if (!frameManager->getActiveFrame()->listensToChars())
		//		frameManager->keyDown(wParam, mod);

		//	input->keyDown(wParam, mod);
		//	break;
  //      }

		//case WM_KEYUP: {
		//	//mod = Input::updateKeymods(mod);

		//	if (!frameManager->getActiveFrame()->listensToChars())
		//		frameManager->keyUp(wParam, mod);

		//	input->keyUp(wParam, mod);
		//	break;
  //      }

		// Left mouse DOWN
		case WM_LBUTTONDOWN: {
			float mouseX, mouseY;
			input->getMousePosition(mouseX, mouseY);
			frameManager->mouseDown(
				button_left, 
				mouseX, 
				mouseY);

			break;
		}

		// Left mouse UP
		case WM_LBUTTONUP: {
			float mouseX, mouseY;
			input->getMousePosition(mouseX, mouseY);
			frameManager->mouseUp(
				button_left, 
				mouseX, 
				mouseY);

			break;
		}

		// Right mouse DOWN
		case WM_RBUTTONDOWN: {
			float mouseX, mouseY;
			input->getMousePosition(mouseX, mouseY);
			frameManager->mouseDown(
				button_right, 
				mouseX, 
				mouseY);

			break;
		}

		// Right mouse UP
		case WM_RBUTTONUP: {
			float mouseX, mouseY;
			input->getMousePosition(mouseX, mouseY);
			frameManager->mouseUp(
				button_right, 
				mouseX, 
				mouseY);

			break;
		}

	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

GameAppPtr gameApp;

GameApp::GameApp()
{
	eventManager   = EventManagerPtr(new EventManager());
	scripting      = ScriptingPtr(new Scripting());
	frameManager   = FrameManagerPtr(new FrameManager());
	processManager = ProcessManagerPtr(new ProcessManager());
	graphics       = GraphicsPtr(new Graphics());
	camera         = CameraPtr(new Camera());
	renderer       = RendererPtr(new Renderer());
	input          = InputPtr(new Input());
	audio          = AudioPtr(new Audio());
	console        = ConsolePtr(new Console());
	game           = GamePtr(new Game());

	HWND _hwnd = 0;
}

GameApp::~GameApp()
{
	shutdownLog();
}

bool GameApp::initialize(
	const std::string &appName, 
	const std::string &initFilename, 
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nShowCmd)
{
	_appName = appName;
	initLog("logfile.log");

	scripting->initialize();

	// Register all scripting functions.
	registerScriptingFunctions();

	// Load up startup options from the initialization file.
	loadOptions(initFilename);

	_hwnd = createWindow(
		hInstance, 
		hPrevInstance, 
		lpCmdLine, 
		nShowCmd, 
		_options.screenWidth, 
		_options.screenHeight);

	if (!graphics->initialize(
		_hwnd, 
		D3DDEVTYPE_HAL, 
		_options.screenWidth, 
		_options.screenHeight, 
		_options.windowed))
	{
		log("Couldn't initialize graphics.");
		return false;
	}

	if (!input->initialize(hInstance, _hwnd))
	{
		log("Couldn't initialize input.");
		return false;
	}

	if (!audio->initialize())
	{
		log("Couldn't initialize audio.");
		return false;
	}

	if (!console->initialize())
	{
		log("Couldn't initialize console.");
		return false;
	}

	if (!game->initialize())
	{
		log("Couldn't initialize game.");
		return false;
	}

	if (!renderer->initialize())
	{
		log("Couldn't initialize renderer.");
		return false;
	}

	registerEvents();

	// All post-initialization settings get read in now.
	scripting->executeFile("scripts/settings.lua");

	return true;
}

void GameApp::shutdown()
{
	static bool alreadyShutDown = false;
	if (!alreadyShutDown)
	{
		shutdownLog();

		//
		// Shut the subsystems down in reverse order.
		//
		soundCache.clear();
		musicCache.clear();
		spriteCache.clear();
		textureCache.clear();
		cubeTextureCache.clear();
		dynamicTextureCache.clear();
		staticMeshCache.clear();
		skinnedMeshCache.clear();
		effectCache.clear();
		meshCache.clear();

		input->shutdown();
		console->shutdown();
		audio->shutdown();
		//input->saveBindingsToFile("scripts/bindings.lua");
		graphics->shutdown();
		scripting->shutdown();

		delete cvarPool;

		alreadyShutDown = true;
	}

	quitting = true;
}

void GameApp::loadOptions(const std::string &initFilename)
{
	// Read the initialization file.
	scripting->executeFile(initFilename);

	memset(&_options, 0, sizeof(_options));
	_options.screenWidth  = scripting->getGlobal<int>("screen_width");
	_options.screenHeight = scripting->getGlobal<int>("screen_height");
	_options.windowed     = scripting->getGlobal<int>("windowed");
	_options.cursor       = scripting->getGlobal<std::string>("cursor");
}

int GameApp::eventLoop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while(!quitting)
	{
		updateTime();
		render();
		timepass();

		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

void GameApp::render()
{
	graphics->beginDraw();

	camera->timepass();
	graphics->getDevice()->SetTransform(D3DTS_VIEW, &camera->getViewMatrix());

	frameManager->render();
	printDiagnostics();
	drawCurrentSubtitle();

	graphics->endDraw();
}

void GameApp::timepass()
{
	frameManager->timepass();
	processManager->timepass();
	renderer->timepass();

	// If we're in non-exclusive mode for the mouse, then
	// force the Windows mouse cursor to be at the same 
	// position as the one that -we- maintain.
	input->timepass();
	if (!input->isMouseExclusive() && hasFocus)
	{
		float cursorX, cursorY;
		input->getMousePosition(cursorX, cursorY);
		SetCursorPos(static_cast<int>(cursorX), static_cast<int>(cursorY));
	}

	bindings.timepass();
	audio->timepass();
	eventManager->timepass();
	updateSpeechSequence();
}

void GameApp::printDiagnostics()
{
	if (showfps)
	{
		std::stringstream fpsStream;
		fpsStream<<"FPS:  "<<getFrameRate();
		std::string fps = fpsStream.str();
		graphics->drawTextAbs(100, 100, fps, false);
	}

	if(showpos && game->isGameInProgress())
	{
		D3DXVECTOR3 p = game->player->getEntity()->getPos();
		std::stringstream posStream;
		posStream<<"Pos:  "<<static_cast<int>(p.x)<<std::endl
		                   <<static_cast<int>(p.y)<<std::endl
						   <<static_cast<int>(p.z)<<std::endl;
		std::string pos = posStream.str();
		graphics->drawTextAbs(100, 200, pos, false);

	}

	if(game->isGameInProgress() && game->isVisible())
	{
		graphics->drawTextRel(0.21f, 0.957f, util::toString(std::ceil(game->player->getEntity()->getHealth())), false, fontWhite);
        //graphics->drawTextAbs(900, 640, game->player->getEntity()->getWeapon()->weaponName);
        graphics->drawTextRel(0.91796875f, 0.859f, util::toString(game->player->getEntity()->getNumBullets()), false, fontWhite);
        graphics->drawTextRel(0.91796875f, 0.885417f, util::toString(game->player->getEntity()->getTotalBullets()), false, fontWhite);
        graphics->drawTextRel(0.785f, 0.930f, util::toString(game->player->getEntity()->getNumGrenades()), false, fontWhite);
	}
}

HWND GameApp::createWindow(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nShowCmd, 
	int width, 
	int height)
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = static_cast<WNDPROC>(WndProc); 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.lpszMenuName  = 0;
	wc.lpszClassName = _appName.c_str();

	if (FAILED(RegisterClass(&wc)))
	{
		ERRORMSG("Register class failed!");
		return 0;
	}

	HWND hwnd = CreateWindow(
					_appName.c_str(), 
					_appName.c_str(), 
					WS_EX_TOPMOST,
					0, 
					0, 
					width, 
					height,
					0, 
					0, 
					hInstance, 
					0); 

	MYASSERT(hwnd);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	ShowCursor(false);

	return hwnd;
}

void GameApp::quit()
{
	PostMessage(_hwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
}
