#ifndef GAMEAPP_HPP
#define GAMEAPP_HPP

//
// Encapsulates all hardware related and OS-specific functionality.
//

#include <windows.h>
#include <boost/shared_ptr.hpp>
#include <string>

class GameApp;
typedef boost::shared_ptr<GameApp> GameAppPtr;

class GameApp
{
public:
	GameApp();
	~GameApp();

	bool initialize(
		const std::string &appName, 
		const std::string &initFilename, 
		HINSTANCE hInstance,
		HINSTANCE hPrevInstance, 
		LPSTR lpCmdLine, 
		int nShowCmd);

	void shutdown();

	int eventLoop();
	void quit();

private:
	struct
	{
		bool windowed;
		float screenWidth;
		float screenHeight;
		std::string cursor;
	} _options;

	void loadOptions(const std::string &initFilename);
	void render();
	void timepass();

	void printDiagnostics();

	HWND createWindow(
		HINSTANCE hInstance,
		HINSTANCE hPrevInstance, 
		LPSTR lpCmdLine, 
		int nShowCmd, 
		int width, 
		int height);

	std::string _appName;
	HWND        _hwnd;
};

extern GameAppPtr gameApp;

#endif
