#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EngineCommon.hpp"


class Game;

class App
{
public:
	App();
	~App();
	void Startup();
	void Shutdown();
	void RunFrame();

	void RunMainLoop();
	
	bool IsQuitting() const;
	bool HandleQuitRequested();
	void RestartGame();

	static bool Event_Quit(EventArgs& args);
	Game* GetGamePtr()const;

private:
	void BeginFrame();
	void Update();
	void Render() const;
	void EndFrame();

private:
	
	bool m_isQuitting = false;

	Game* m_theGame = nullptr;
};

