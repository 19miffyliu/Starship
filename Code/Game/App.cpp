#include "Game/App.hpp"
#include "Game/Game.hpp"

#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"

Renderer* g_theRenderer = nullptr;
InputSystem* g_theInput = nullptr;
AudioSystem* g_theAudio = nullptr;
Window* g_theWindow = nullptr;
BitmapFont* g_theFont = nullptr;
Clock* g_theSysClock = nullptr;

NamedStrings g_gameconfigBlackBoard;


App::App()
{
}

App::~App()
{
}

void App::Startup()
{
	EventSystemConfig eventConfig;
	g_theEventSystem = new EventSystem(eventConfig);


	InputConfig inputconfig;
	g_theInput = new InputSystem(inputconfig);
	

	WindowConfig windowConfig;
	windowConfig.m_aspectRatio = 2.f;
	windowConfig.m_inputSystem = g_theInput;
	windowConfig.m_windowtitle = "SD1-A4: Starship Gold";
	g_theWindow = new Window(windowConfig);

	RendererConfig rendererConfig;
	rendererConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(rendererConfig);

	g_theSysClock = new Clock();

	DevConsoleConfig devConsoleConfig;
	devConsoleConfig.m_fontName = "Data/Fonts/SquirrelFixedFont";
	devConsoleConfig.m_renderer = g_theRenderer;
	g_theDevConsole = new DevConsole(devConsoleConfig);

	AudioConfig audioConfig;
	g_theAudio = new AudioSystem(audioConfig);

	//start up functions
	g_theEventSystem->Startup();
	g_theWindow->Startup();
	g_theRenderer->Startup();
	g_theDevConsole->Startup();
	g_theInput->Startup();
	g_theAudio->Startup();

	g_theEventSystem->SubscribeEventCallbackFunction("quit", Event_Quit);


	m_theGame = new Game();
	m_theGame->Startup();
	m_theGame->SetWindowsDimension(GetFromIntVec2(g_theWindow->GetClientDimensions()));
}



void App::Shutdown()
{
	delete m_theGame;
	m_theGame = nullptr;


	//shut down the engine
	g_theAudio->Shutdown();
	g_theDevConsole->Shutdown();
	g_theRenderer->Shutdown();
	g_theWindow->Shutdown();
	g_theInput->Shutdown();
	g_theEventSystem->Shutdown();
	


	//delete engine
	
	delete g_theAudio;
	g_theAudio = nullptr;
	delete g_theDevConsole;
	g_theDevConsole = nullptr;
	delete g_theRenderer;
	g_theRenderer = nullptr;
	delete g_theWindow;
	g_theWindow = nullptr;
	delete g_theInput;
	g_theInput = nullptr;
	delete g_theEventSystem;
	g_theEventSystem = nullptr;


	delete g_theSysClock;
	g_theSysClock = nullptr;
	
	
}

void App::RunFrame()
{
	BeginFrame();
	Update();
	Render();		
	EndFrame();		
}

void App::RunMainLoop()
{
	while (!IsQuitting())
	{
		RunFrame(); 
	}
}

bool App::IsQuitting() const
{
	return m_isQuitting;
}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;
	return true;
}

void App::BeginFrame()
{
	g_theSysClock->TickSystemClock();

	g_theEventSystem->BeginFrame();
	g_theInput->BeginFrame();
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theDevConsole->BeginFrame();
	g_theAudio->BeginFrame();
}

void App::Update()
{


	m_theGame->Update();

	
}

void App::Render() const
{

	//clear screen
	unsigned char r = 0, g = 0, b = 0, a = 255;
	g_theRenderer->ClearScreen(Rgba8(r, g, b, a));


	//render game
	m_theGame->Render();


}

void App::EndFrame()
{	
	g_theAudio->EndFrame();
	g_theDevConsole->EndFrame();
	g_theRenderer->EndFrame();
	g_theWindow->EndFrame();
	g_theInput->EndFrame();
	g_theEventSystem->EndFrame();
}

void App::RestartGame()
{
	delete m_theGame;
	m_theGame = nullptr;

	m_theGame = new Game();
	m_theGame->Startup();
}

bool App::Event_Quit(EventArgs& args)
{
	UNUSED(args);
	return g_theApp->HandleQuitRequested();
}

Game* App::GetGamePtr() const
{
	return m_theGame;
}


