#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in VERY few places (and .CPPs only)
#include "Engine/Core/EngineCommon.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Game/App.hpp"


App* g_theApp = nullptr;

//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{


	UNUSED(applicationInstanceHandle);
	UNUSED(commandLineString);

	g_theApp = new App();
	g_theApp->Startup();

	// Program main loop; keep running frames until it's time to quit
	g_theApp->RunMainLoop();

	
	g_theApp->Shutdown();
	delete g_theApp;
	g_theApp = nullptr;

	return 0;
}


