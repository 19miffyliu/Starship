#pragma once
#include "Game/PlayerShip.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Bullet.hpp"
#include "Game/Debris.hpp"
#include "Game/Beetle.hpp"
#include "Game/Wasp.hpp"
#include "Game/Wave.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/EngineCommon.hpp"

//SFX credits: Victor Natas, Isish coder, Enes DENIZ

constexpr int ATTRACT_MODE_VERTS = NUM_SHIP_VERTS * 2 + 3;
class Clock;

class Game
{
public:
	Game();
	~Game();

	void Startup();
	void Update();
	void Render() const;
	void Shutdown();

	//attract mode related
	void EnterAttractMode();
	void InitializeAttractLocalVerts();
	void InitializeStarVertices();

	//gameplay related
	void StartGame();
	void ResetGame();
	void ClearAllEntities();
	void BackToAttractMode();

	//update functions
	void UpdateEntities();
	void UpdateCollision();

	void UpdatePlayershipCollision();
	void UpdateBulletCollision();
	void UpdateAsteroidCollision();

	void UpdateCameras();
	void UpdateCamFollowPlayer();
	void MoveCamTo(Camera& cam, Vec2 position);

	//UI related
	void UpdateAttractMode();
	void SetWindowsDimension(Vec2 const& dimension);
	
	//DevConsole related
	void PrintKeysToConsole();
	void SetTimeScale(double timeScale);
	static bool Command_SetTimeScale(EventArgs& args);

	//handle decrease health + SFX
	void HandlePlayershipDeath();
	void HandleBulletHit(float pitch, float volume = 0.8f);
	void HandleBeetleDeath();
	void HandleWaspDeath();

	void HandlePause(bool isPaused);
	void HandleSlowMo(float speedMultiplier);

	void HandleGameWin();
	void HandleGameLost();

	void StopSoundAtIndex(SoundPlaybackID& soundId);

	void DeleteGarbageEntities();
	void CheckInput();
	void UpdateWaves();

	//render function
	void RenderLimitedLives()const;
	void RenderShipTemperatureBar()const;
	void RenderMinimap()const;
	void RenderPauseOverlay()const;

	void RenderEntities() const;
	void RenderStarryBackground() const;
	void RenderAttractMode()const;
	void DebugRender()const;

	//wave related
	void InitializeWaves(int numAs,int numBeetles,int numWasps);
	void SpawnWave(int waveNum);


	//spawning entities
	void SpawnAsteroid(Vec2 pos, float orientationDeg);
	void SpawnRandomAsteroids(int numAst);
	void SpawnBullet(Vec2 pos, float orientationDeg);
	void SpawnBeetles(int numB);
	void SpawnWasps(int numW);

	//debris related
	void SpawnNewDebrisCluster(int numDebris, Vec2 const& position, Vec2 const& avgVelocity, float maxScatterSpeed, float avgRadius, Rgba8 const& color);
	void SpawnDebrisForShip(int minD, int maxD);


	//garbage collection
	void DeleteGarbageAsteroid();
	void DeleteGarbageBullet();
	void DeleteGarbageDebris();
	void DeleteGarbageBeetles();
	void DeleteGarbageWasps();

	//get function
	Vec2 GetRandomPosOffscreen(float radius)const;
	Vec2 GetShipPos()const;
	bool GetShipAlive()const;
	bool IsInAttractMode()const;
	int GetPlayerLives()const;
	float GetGlobalVolume()const;
	

	//set function
	void IncreaseRespawnNum();
	void SetGlobalVolume(float volume);

private:
	//utility
	RandomNumberGenerator m_randomGen;
	
	//attract mode
	bool m_inAttractMode = true;
	int m_attractTriangleIndex = NUM_SHIP_VERTS * 2 ;
	Rgba8 m_attractTriangleColor = Rgba8(102, 204, 153, 255);
	Vertex_PCU m_attractModeVertices[ATTRACT_MODE_VERTS];
	Vec2 m_attractShip1Offset;
	float m_attractShip1OrientationDeg = 0.f;
	Vec2 m_attractShip2Offset;
	float m_attractShip2OrientationDeg = 180.f;
	float m_inverseDirection1 = 1.f;
	float m_inverseDirection2 = 1.f;

	//UI element display
	bool m_gameWon = false;
	bool m_gameLost = false;

	//playership
	int m_currentShipLives = PLAYER_SHIP_MAX_HEALTH;
	int m_respawns = 0;
	float m_timeWhenPlayerConsumesAllLives = 0;
	float m_timeSincePlayerConsumeAllLives = 0;
	float m_timeWhenPlayerWins = 0;
	float m_timeSincePlayerWins = 0;
	PlayerShip* m_playership = nullptr;

	//time management
	Clock* m_gameClock = nullptr;
	float m_SFXSpeedMultiplier = 1.f;
	bool m_debugDraw = false;

	//current number of entities
	int m_numBullets = 0;
	int m_numAsteroids = 0;
	int m_numDebris = 0;
	int m_numBeetles = 0;
	int m_numWasps = 0;


	//array of pointers
	Asteroid* m_asteroids[MAX_ASTEROIDS] = {};
	Bullet* m_bullets[MAX_BULLETS] = {};
	Beetle* m_beetles[MAX_BEETLES] = {};
	Wasp* m_wasps[MAX_WASPS] = {};
	Debris* m_debris[MAX_DEBRIS] = {};

	//wave
	Wave m_waves[MAX_WAVES] = {};
	int m_currentWave = 0;

	//camera
	Vec2 m_windowsDimension;
	Camera m_worldCamera;
	Vec2 m_worldCamOffset = Vec2(WORLD_CAM_SIZE_X * 0.5, WORLD_CAM_SIZE_Y * 0.5);

	Camera m_screenCamera;
	Vec2 m_screenCamOffset = Vec2(SCREEN_SIZE_X * 0.5, SCREEN_SIZE_Y * 0.5);
	float m_traumaLevel = 0.f;


	//sound
	float m_globalVolume = 1.f;

	SoundID m_BGMAttract = static_cast<SoundID>(-1);
	SoundPlaybackID m_BGMAttractPlayIndex = static_cast<SoundPlaybackID>(-1);
	SoundID m_BGMGameplay = static_cast<SoundID>(-1);
	SoundPlaybackID m_BGMGameplayPlayIndex = static_cast<SoundPlaybackID>(-1);

	SoundID m_SFXPlayershipHurtMinimal = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXPlayershipHurtMinimalPlayIndex = static_cast<SoundPlaybackID>(-1);
	SoundID m_SFXPlayershipHurtCritical = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXPlayershipHurtCriticalPlayIndex = static_cast<SoundPlaybackID>(-1);

	SoundID m_SFXNextLevel = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXNextLevelPlayIndex = static_cast<SoundPlaybackID>(-1);
	SoundID m_SFXGameWin = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXGameWinPlayIndex = static_cast<SoundPlaybackID>(-1);
	SoundID m_SFXGameLose = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXGameLosePlayIndex = static_cast<SoundPlaybackID>(-1);

	SoundID m_SFXSelect = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXSelectPlayIndex = static_cast<SoundPlaybackID>(-1);
	SoundID m_SFXPickUp = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXPickUpPlayIndex = static_cast<SoundPlaybackID>(-1);
	SoundID m_SFXPowerUp = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXPowerUpPlayIndex = static_cast<SoundPlaybackID>(-1);
	SoundID m_SFXCountDown = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXCountDownPlayIndex = static_cast<SoundPlaybackID>(-1);

	SoundID m_SFXBulletHit = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXBulletHitPlayIndex = static_cast<SoundPlaybackID>(-1);
	SoundID m_SFXEnemyHurt = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXEnemyHurtPlayIndex = static_cast<SoundPlaybackID>(-1);
	SoundID m_SFXEnemyDeath = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXEnemyDeathPlayIndex = static_cast<SoundPlaybackID>(-1);

	SoundID m_SFXBeetleFireBullet = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXBeetleFireBulletPlayIndex = static_cast<SoundPlaybackID>(-1);
	SoundID m_SFXWaspFireBullet = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXWaspFireBulletPlayIndex = static_cast<SoundPlaybackID>(-1);


	//stars

	Vertex_PCU m_totalStarVertices[TOTAL_STAR_VERTEX];
};

