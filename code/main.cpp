#include <windows.h>
#include "gameapp.hpp"
#include "scripting.hpp"
#include "common.hpp"

int WINAPI WinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nShowCmd )
{
	gameApp = GameAppPtr(new GameApp());
	if (!gameApp->initialize(
			"Restless Moon II", 
			"scripts/init.lua", 
			hInstance, 
			hPrevInstance, 
			lpCmdLine, 
			nShowCmd))
	{
		log("Couldn't initialize the game!");
		return -1;
	}

	gameApp->eventLoop();

	return 0;
}
