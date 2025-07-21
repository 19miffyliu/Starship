#include "Game/Game.hpp"
#include "Game/App.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"

#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"

#include "Game/PlayerShip.hpp"

Game::Game()
{
	
}

Game::~Game()
{
	delete m_playership;
	m_playership = nullptr;

}

void Game::Startup()
{
	m_gameClock = new Clock(*g_theSysClock);

	//initialize playership
	Vec2 worldCenter = Vec2(WORLD_CENTER_X, WORLD_CENTER_Y);
	m_playership = new PlayerShip(this, worldCenter, 0.f);
	

	//load sounds
	m_BGMAttract = g_theAudio->CreateOrGetSound("Data/Audio/703246__victor_natas__under-the-influence-of-lucifer.wav");
	m_BGMGameplay = g_theAudio->CreateOrGetSound("Data/Audio/623137__victor_natas__music-for-dark-moments.wav");

	m_SFXPlayershipHurtMinimal = g_theAudio->CreateOrGetSound("Data/Audio/Hit_Hurt_Small.wav");
	m_SFXPlayershipHurtCritical = g_theAudio->CreateOrGetSound("Data/Audio/Hit_Hurt_Death.wav");

	m_SFXNextLevel = g_theAudio->CreateOrGetSound("Data/Audio/Next_Level1.wav");
	m_SFXGameWin = g_theAudio->CreateOrGetSound("Data/Audio/624882__Enes_DENIZ__old-video-game-5.wav");
	m_SFXGameLose = g_theAudio->CreateOrGetSound("Data/Audio/660279__irishcoder__game-over.wav");

	m_SFXSelect = g_theAudio->CreateOrGetSound("Data/Audio/Blip_Select.wav");
	m_SFXPickUp = g_theAudio->CreateOrGetSound("Data/Audio/Pickup_Coin.wav");
	m_SFXPowerUp = g_theAudio->CreateOrGetSound("Data/Audio/Powerup.wav");
	m_SFXCountDown = g_theAudio->CreateOrGetSound("Data/Audio/CountDown.wav");

	m_SFXBulletHit = g_theAudio->CreateOrGetSound("Data/Audio/Hit_Hurt_Enemy.wav");
	m_SFXEnemyHurt = g_theAudio->CreateOrGetSound("Data/Audio/Hit_Hurt_Die.wav");
	m_SFXEnemyDeath = g_theAudio->CreateOrGetSound("Data/Audio/Hit_Death_Enemy.wav");


	m_SFXBeetleFireBullet = g_theAudio->CreateOrGetSound("Data/Audio/Laser_Shoot_Enemy.wav");
	m_SFXWaspFireBullet = g_theAudio->CreateOrGetSound("Data/Audio/Laser_Shoot_Enemy2.wav");


	//initialize stars
	InitializeStarVertices();

	EnterAttractMode();

	//console
	EventArgs args;
	args.SetValue("scale", "double");
	g_theEventSystem->SubscribeEventCallbackFunction("timescale", Command_SetTimeScale, args);
	PrintKeysToConsole();

	

}

void Game::Update()
{	

	CheckInput();
	if (m_inAttractMode){
		UpdateAttractMode();
	}
	else {
		UpdateWaves();
		UpdateEntities();
		UpdateCollision();
		DeleteGarbageEntities();
		
		
	}
	//camera should update last
	UpdateCameras();
}

void Game::Render()const
{
	
	if (m_inAttractMode) {
		g_theRenderer->BeginCamera(m_screenCamera);
		RenderAttractMode();
		g_theRenderer->EndCamera(m_screenCamera);

	}
	else {
		//render world size
		g_theRenderer->BeginCamera(m_worldCamera);
		RenderStarryBackground();
		RenderEntities();
		g_theRenderer->EndCamera(m_worldCamera);

		//render screen size
		g_theRenderer->BeginCamera(m_screenCamera);
		RenderLimitedLives();
		RenderShipTemperatureBar();
		RenderMinimap();
		RenderPauseOverlay();
		g_theRenderer->EndCamera(m_screenCamera);

	}
	g_theRenderer->BeginCamera(m_screenCamera);
	//render DevConsole
	g_theDevConsole->Render(AABB2(Vec2::ZERO, Vec2(SCREEN_SIZE_X,SCREEN_SIZE_Y)), nullptr);
	g_theRenderer->EndCamera(m_screenCamera);

}

void Game::Shutdown()
{
	delete m_playership;
	m_playership = nullptr;

	for (int i = 0; i < MAX_BEETLES;i++) {
		delete m_beetles[i];
		m_beetles[i] = nullptr;
	}

	for (int i = 0; i < MAX_WASPS; i++) {
		delete m_wasps[i];
		m_wasps[i] = nullptr;
	}

	for (int i = 0; i < MAX_DEBRIS; i++) {
		delete m_debris[i];
		m_debris[i] = nullptr;
	}
}

void Game::EnterAttractMode()
{
	//if the gameplay music is playing, stop music
	StopSoundAtIndex(m_BGMGameplayPlayIndex);
	m_BGMAttractPlayIndex = g_theAudio->StartSound(m_BGMAttract,true, m_globalVolume * 0.5f, 0.f, m_SFXSpeedMultiplier, m_gameClock->IsPaused());
	//initialize attract mode vertices
	InitializeAttractLocalVerts();
}

void Game::InitializeAttractLocalVerts()
{
	//store ship's verts
	Vertex_PCU verts[NUM_SHIP_VERTS];

	for (int i = 0; i < NUM_SHIP_VERTS;i++) {
		m_attractModeVertices[i] = m_playership->GetVertByIndex(i);

		m_attractModeVertices[i+NUM_SHIP_VERTS] = m_attractModeVertices[i];

	}

	//initialize verts for triangle
	
	m_attractModeVertices[m_attractTriangleIndex].m_position = Vec3(2.f, 0.f, 0.f);
	m_attractModeVertices[m_attractTriangleIndex+1].m_position = Vec3(-1.f, 1.73205f, 0.f);
	m_attractModeVertices[m_attractTriangleIndex+2].m_position = Vec3(-1.f, -1.73205f, 0.f);

	for (int i = m_attractTriangleIndex; i < m_attractTriangleIndex + 3; i++) {
		m_attractModeVertices[i].m_color = m_attractTriangleColor;
	}

}

void Game::InitializeStarVertices()
{

	//initialize 3 vertices for the triangle
	Vec3 topLeft(-1.f, 1.f, 0.f);
	Vec3 bottom(0.f, -1.f, 0.f);
	Vec3 topRight(1.f, 1.f, 0.f);

	//initialize the color and uv
	Rgba8 starColor(255, 255, 255);
	Vec2 textCoords(0.f, 0.f);

	Vertex_PCU m_perStarVertices[NUM_STAR_VERTEX];
	m_perStarVertices[0] = Vertex_PCU(topLeft, starColor, textCoords);
	m_perStarVertices[1] = Vertex_PCU(bottom, starColor, textCoords);
	m_perStarVertices[2] = Vertex_PCU(topRight, starColor, textCoords);


	//place NUM_STARS_IN_BG of stars randomly in the world
	Vec2 starTransformPosition;
	Vertex_PCU tempStarVertices[NUM_STAR_VERTEX];

	for (int i = 0; i < NUM_STARS_IN_BG; i++) {
		starTransformPosition.x = m_randomGen.RollRandomFloatInRange(0.f, WORLD_SIZE_X);
		starTransformPosition.y = m_randomGen.RollRandomFloatInRange(0.f, WORLD_SIZE_Y);

		topLeft.y = m_randomGen.RollRandomFloatInRange(0.f,1.f);
		bottom.x = m_randomGen.RollRandomFloatInRange(-1.f,1.f);
		topRight.y = m_randomGen.RollRandomFloatInRange(0.f, 1.f);

		m_perStarVertices[0].m_position = topLeft;
		m_perStarVertices[1].m_position = bottom;
		m_perStarVertices[2].m_position = topRight;


		tempStarVertices[0] = m_perStarVertices[0];
		tempStarVertices[1] = m_perStarVertices[1];
		tempStarVertices[2] = m_perStarVertices[2];
		TransformVertexArrayXY3D(NUM_STAR_VERTEX, tempStarVertices, STAR_SCALE, 0.f, starTransformPosition);

		m_totalStarVertices[i*3] = tempStarVertices[0];
		m_totalStarVertices[i*3+1] = tempStarVertices[1];
		m_totalStarVertices[i*3+2] = tempStarVertices[2];
	}



}

void Game::UpdateAttractMode()
{
	for (int i = m_attractTriangleIndex; i < m_attractTriangleIndex + 3; i++) {
		m_attractModeVertices[i].m_color.a = static_cast<unsigned char>(sinf(static_cast<float>(GetCurrentTimeSeconds())*2.f)*63.5f + 127.f);
	}

	//make fake playership move back and forth
	Vec2 previousShip1Offset = m_attractShip1Offset;
	Vec2 previousShip2Offset = m_attractShip2Offset;

	float twoPi = 1.f * 3.14159265f; //changed from 2 pi to 1 pi for visual effect
	float toBeX = m_attractShip1Offset.x;
	float toBeX2 = m_attractShip2Offset.x;
	
	
	if (toBeX > twoPi) {
		m_inverseDirection1 = -1.f;
	}
	else if (toBeX < -twoPi) {
		m_inverseDirection1 = 1.f;
	}
	toBeX += static_cast<float>(m_gameClock->GetDeltaSeconds()) * m_inverseDirection1;
	

	m_attractShip1Offset = Vec2(toBeX, 0.5f* cosf(toBeX) * tanf(0.5f * toBeX));
	m_attractShip1OrientationDeg = (m_attractShip1Offset-previousShip1Offset).GetOrientationDegrees();


	if (toBeX2 > twoPi) {
		m_inverseDirection2 = -1.f;
	}
	else if (toBeX2 < -twoPi) {
		m_inverseDirection2 = 1.f;
	}
	toBeX2 += static_cast<float>(m_gameClock->GetDeltaSeconds()) * m_inverseDirection2;
	m_attractShip2Offset = Vec2(toBeX2, 0.5f * cosf(toBeX2) * tanf(0.5f * -toBeX2));
	m_attractShip2OrientationDeg = ( m_attractShip2Offset - previousShip2Offset).GetOrientationDegrees();

}

void Game::SetWindowsDimension(Vec2 const& dimension)
{
	m_windowsDimension = dimension;

	//set up camera viewport
	AABB2 viewport(Vec2::ZERO, dimension);
	m_screenCamera.SetViewport(viewport);
	m_worldCamera.SetViewport(viewport);
	m_worldCamera.SetAspect(2);
}

void Game::PrintKeysToConsole()
{
	std::string fileString;
	std::string fileName("Data/Controls/ControlKeys.txt");
	FileReadToString(fileString, fileName);

	std::string delimiter("Keyboard:\r");
	Strings paragraphs = SplitStringOnString(fileString, delimiter);

	Strings debugControls = SplitStringOnDelimiter(paragraphs[2], '\n', false);
	debugControls[0] = "[Debug Controls]";

	std::string delimiter2("Xbox controller:\r");
	Strings parsedParagraphs = SplitStringOnString(paragraphs[1], delimiter2);

	std::string gameplayControlsParagraph = parsedParagraphs[0];
	Strings gameplayControls = SplitStringOnDelimiter(gameplayControlsParagraph, '\n', false);
	gameplayControls[0] = "[Gameplay Controls]";
	gameplayControls.pop_back();

	Rgba8 color = DevConsole::INFO_HINT;
	g_theDevConsole->AddParagraph(color, gameplayControls);
	g_theDevConsole->AddParagraph(color, debugControls);

}

void Game::SetTimeScale(double timeScale)
{
	m_gameClock->SetTimeScale(timeScale);
}


bool Game::Command_SetTimeScale(EventArgs& args)
{
	std::string timescale = args.GetValue("scale", "NaN");
	double scale = -1.0;
	if (IsNumber(timescale))scale = atof(timescale.c_str());

	//if input is valid double
	if (scale >= 0.0)
	{
		g_theApp->GetGamePtr()->SetTimeScale(scale);
		return true;
	}
	//if input is not valid
	g_theDevConsole->Addline(DevConsole::ERROR_MSG, "scale value is not a valid double. Correct format: timescale scale=0.5");

	return true;
}

void Game::StartGame()
{
	//if the attract music is playing, stop music
	StopSoundAtIndex(m_BGMAttractPlayIndex);
	m_BGMGameplayPlayIndex = g_theAudio->StartSound(m_BGMGameplay, true, m_globalVolume * 0.5f,0.f,m_SFXSpeedMultiplier,m_gameClock->IsPaused());

	InitializeWaves(INITIAL_ASTEROID_NUM, INITIAL_BEETLE_NUM, INITIAL_WASP_NUM);
	SpawnWave(m_currentWave);
}

void Game::ResetGame()
{
	ClearAllEntities();
	m_currentShipLives = PLAYER_SHIP_MAX_HEALTH;
	m_respawns = 0;
	m_currentWave = 0;
	m_gameClock->SetTimeScale(1.0);
	m_gameClock->Reset();
	HandleSlowMo(1.0f);
	m_gameWon = false;
	m_gameLost = false;

	m_debugDraw = false;
	
}

void Game::ClearAllEntities()
{
	//m_playership->Respawn();

	delete m_playership;
	m_playership = new PlayerShip(this,Vec2(WORLD_CENTER_X,WORLD_CENTER_Y),0.f);

	//clear asteroids, bullets, beetles, wasps, debris
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		delete m_asteroids[i];
		m_asteroids[i] = nullptr;

	}
	m_numAsteroids = 0;

	for (int i = 0; i < MAX_BULLETS; i++) {
		delete m_bullets[i];
		m_bullets[i] = nullptr;

	}
	m_numBullets = 0;


	for (int i = 0; i < MAX_DEBRIS; i++) {
			delete m_debris[i];
			m_debris[i] = nullptr;
			
	}
	m_numDebris = 0;


	for (int i = 0; i < MAX_BEETLES; i++) {
			delete m_beetles[i];
			m_beetles[i] = nullptr;
			
	}
	m_numBeetles = 0;

	for (int i = 0; i < MAX_WASPS; i++) {
			delete m_wasps[i];
			m_wasps[i] = nullptr;
			
	}
	m_numWasps = 0;
}

void Game::BackToAttractMode()
{
	//if the gameplay music is playing, stop music
	StopSoundAtIndex(m_BGMGameplayPlayIndex);
	m_BGMAttractPlayIndex = g_theAudio->StartSound(m_BGMAttract, true, m_globalVolume * 0.5f, 0.f, m_SFXSpeedMultiplier, m_gameClock->IsPaused());
	m_inAttractMode = true;
	ResetGame();
}



void Game::UpdateEntities()
{
	//update each of entities
	float deltaSeconds = static_cast<float>(m_gameClock->GetDeltaSeconds());

	m_playership->Update(deltaSeconds);


	for (int i = 0; i < MAX_BULLETS; ++i) {
		if (m_bullets[i] != nullptr)m_bullets[i]->Update(deltaSeconds);
	}

	for (int i = 0; i < MAX_ASTEROIDS; ++i) {
		if (m_asteroids[i] != nullptr)m_asteroids[i]->Update(deltaSeconds);
	}

	for (int i = 0; i < MAX_BEETLES; ++i) {
		if (m_beetles[i] != nullptr)m_beetles[i]->Update(deltaSeconds);
	}

	for (int i = 0; i < MAX_WASPS; ++i) {
		if (m_wasps[i] != nullptr)m_wasps[i]->Update(deltaSeconds);
	}

	for (int i = 0; i < MAX_DEBRIS; ++i) {
		if(m_debris[i]!=nullptr)m_debris[i]->Update(deltaSeconds);
	}


}

//handle entity collision
void Game::UpdateCollision()
{
	

	//playership collision
	UpdatePlayershipCollision();

	//bullet collision
	UpdateBulletCollision();
	
	//asteroid collision
	UpdateAsteroidCollision();
	



}

void Game::UpdatePlayershipCollision()
{
	//if died for 3 seconds, goes back to attract mode
	float timeNow = static_cast<float>(GetCurrentTimeSeconds());
	if (m_currentShipLives == 0) {
		m_timeSincePlayerConsumeAllLives = timeNow - m_timeWhenPlayerConsumesAllLives;
		if (m_timeSincePlayerConsumeAllLives >= 3.f) {
			BackToAttractMode();
			return;
		}
	}

	if (m_gameWon) {
		m_playership->StopThrustingSound();
		m_timeSincePlayerWins = timeNow - m_timeWhenPlayerWins;
		m_timeSincePlayerWins = timeNow - m_timeWhenPlayerWins;
		if (m_timeSincePlayerWins >= 3.f) {
			BackToAttractMode();
		}
	}

	//dies if touches an asteroid
	for (int i = 0; i < MAX_ASTEROIDS; ++i) {
		if (m_asteroids[i] == nullptr)continue;
		bool overlaps = DoDiscsOverlap(m_playership->GetPosition(), PLAYER_SHIP_PHYSICS_RADIUS, m_asteroids[i]->GetPosition(), ASTEROID_PHYSICS_RADIUS);

		if (overlaps && m_playership->IsAlive()&&!m_playership->IsInvincible()) {
			HandlePlayershipDeath();

			m_asteroids[i]->DecreaseHealthByOne();
		}

	}


	//dies if touches an beetle
	for (int i = 0; i < MAX_BEETLES; ++i) {
		if (m_beetles[i] == nullptr)continue;
		bool overlaps = DoDiscsOverlap(m_playership->GetPosition(), PLAYER_SHIP_PHYSICS_RADIUS, m_beetles[i]->GetPosition(), BEETLE_PHYSICS_RADIUS);

		if (overlaps && m_playership->IsAlive() && !m_playership->IsInvincible()) {
			HandlePlayershipDeath();
			m_beetles[i]->DecreaseHealthByOne();
		}

	}


	//dies if touches an wasp
	for (int i = 0; i < MAX_WASPS; ++i) {
		if (m_wasps[i] == nullptr)continue;
		bool overlaps = DoDiscsOverlap(m_playership->GetPosition(), PLAYER_SHIP_PHYSICS_RADIUS, m_wasps[i]->GetPosition(), WASP_PHYSICS_RADIUS);

		if (overlaps && m_playership->IsAlive() && !m_playership->IsInvincible()) {
			HandlePlayershipDeath();
			m_wasps[i]->DecreaseHealthByOne();
		}

	}
}

void Game::UpdateBulletCollision()
{

	//dies if goes offscreen
	for (int i = 0; i < MAX_BULLETS; ++i) {
		if (m_bullets[i] == nullptr)continue;
		if (m_bullets[i]->IsOffscreen()) {
			m_bullets[i]->Die();
			m_bullets[i]->BecomesGarbage();

		}

		//bullet hitting asteroid
		for (int j = 0; j < MAX_ASTEROIDS; ++j) {
			if (m_asteroids[j] == nullptr)continue;

			bool overlaps = DoDiscsOverlap(m_bullets[i]->GetPosition(), BULLET_PHYSICS_RADIUS, m_asteroids[j]->GetPosition(), ASTEROID_PHYSICS_RADIUS);

			if (overlaps) {
				m_bullets[i]->Die();
				m_bullets[i]->BecomesGarbage();
				// spawn 1-3 debris if asteroid is hit by bullet
				SpawnNewDebrisCluster(m_randomGen.RollRandomIntInRange(1, 3),
					m_bullets[i]->GetPosition(),
					-m_bullets[i]->GetVelocity() * 0.2f,
					MAX_DEBRIS_SCATTER_SPEED,
					BULLET_PHYSICS_RADIUS * 1.f,
					m_asteroids[j]->GetColor());
				m_asteroids[j]->DecreaseHealthByOne();
				HandleBulletHit(ASTEROID_HIT_PITCH,ASTEROID_HIT_VOLUME);

				// spawn 3-12 debris if asteroid is dead
				if (!m_asteroids[j]->IsAlive()) {
					SpawnNewDebrisCluster(m_randomGen.RollRandomIntInRange(3, 12),
						m_asteroids[j]->GetPosition(),
						m_asteroids[j]->GetVelocity(),
						MAX_ASTEROID_SCATTER_SPEED,
						ASTEROID_COSMETIC_RADIUS * 0.7f,
						m_asteroids[j]->GetColor());
				}
			}
		}

		//bullet hitting beetle
		for (int j = 0; j < MAX_BEETLES; ++j) {
			if (m_beetles[j] == nullptr)continue;
			bool overlaps = DoDiscsOverlap(m_bullets[i]->GetPosition(), BULLET_PHYSICS_RADIUS, m_beetles[j]->GetPosition(), BEETLE_PHYSICS_RADIUS);

			if (overlaps) {
				m_bullets[i]->Die();
				m_bullets[i]->BecomesGarbage();
				// spawn 1-3 debris if beetle is hit by bullet
				SpawnNewDebrisCluster(m_randomGen.RollRandomIntInRange(1, 3),
					m_bullets[i]->GetPosition(),
					-m_bullets[i]->GetVelocity() * 0.2f,
					MAX_DEBRIS_SCATTER_SPEED,
					BULLET_PHYSICS_RADIUS * 1.f,
					m_beetles[j]->GetColor());
				m_beetles[j]->DecreaseHealthByOne();
				

				// spawn 3-12 debris if beetle is dead
				if (!m_beetles[j]->IsAlive()) {
					SpawnNewDebrisCluster(m_randomGen.RollRandomIntInRange(3, 12),
						m_beetles[j]->GetPosition(),
						m_beetles[j]->GetVelocity(),
						MAX_BEETLE_SCATTER_SPEED,
						BEETLE_COSMETIC_RADIUS * 0.7f,
						m_beetles[j]->GetColor());

					HandleBeetleDeath();
				}
				else {
					HandleBulletHit(BEETLE_HIT_PITCH);
				}
			}
		}





		//bullet hitting wasp

		for (int j = 0; j < MAX_WASPS; ++j) {
			if (m_wasps[j] == nullptr)continue;
			bool overlaps = DoDiscsOverlap(m_bullets[i]->GetPosition(), BULLET_PHYSICS_RADIUS, m_wasps[j]->GetPosition(), WASP_PHYSICS_RADIUS);

			if (overlaps) {
				m_bullets[i]->Die();
				m_bullets[i]->BecomesGarbage();
				// spawn 1-3 debris if wasp is hit by bullet
				SpawnNewDebrisCluster(m_randomGen.RollRandomIntInRange(1, 3),
					m_bullets[i]->GetPosition(),
					-m_bullets[i]->GetVelocity() * 0.2f,
					MAX_DEBRIS_SCATTER_SPEED,
					BULLET_PHYSICS_RADIUS * 1.f,
					m_wasps[j]->GetColor());
				m_wasps[j]->DecreaseHealthByOne();

				// spawn 3-12 debris if wasp is dead
				if (!m_wasps[j]->IsAlive()) {
					SpawnNewDebrisCluster(m_randomGen.RollRandomIntInRange(3, 12),
						m_wasps[j]->GetPosition(),
						m_wasps[j]->GetVelocity(),
						MAX_WASP_SCATTER_SPEED,
						WASP_COSMETIC_RADIUS * 0.7f,
						m_wasps[j]->GetColor());

					HandleWaspDeath();
				}
				else {
					HandleBulletHit(WASP_HIT_PITCH);
				}
			}
		}

	}
}

void Game::UpdateAsteroidCollision()
{
	//wraps around the screen if it goes off screen
	for (int i = 0; i < MAX_ASTEROIDS; ++i) {
		if (m_asteroids[i] == nullptr)continue;
		if (m_asteroids[i]->IsOffscreen()) {
			m_asteroids[i]->WrapAroundWorld();
		}
	}
}

void Game::HandlePlayershipDeath()
{
	m_playership->Die();
	m_currentShipLives--;
	
	int minDebris = 7;
	int maxDebris = 12;
	if (m_currentShipLives == 0) {
		m_timeWhenPlayerConsumesAllLives = static_cast<float>(GetCurrentTimeSeconds());
		m_SFXPlayershipHurtCriticalPlayIndex = g_theAudio->StartSound(m_SFXPlayershipHurtCritical,false, m_globalVolume * 1.f,0.f,m_SFXSpeedMultiplier, m_gameClock->IsPaused());
		minDebris = 20, maxDebris = 30;
		m_traumaLevel += PLAYER_SHIP_DEATH_TRAUMA;

		HandleGameLost();
	}
	else {
		m_SFXPlayershipHurtMinimalPlayIndex = g_theAudio->StartSound(m_SFXPlayershipHurtMinimal, false, m_globalVolume * 1.f, 0.f, m_SFXSpeedMultiplier, m_gameClock->IsPaused());
		m_traumaLevel += GAMEOVER_TRAUMA;

	}

	// spawn 5-30 debris if ship is dead
	SpawnDebrisForShip(minDebris, maxDebris);

	
}

void Game::HandleBulletHit(float pitch, float volume)
{

	float pitchVariation = m_randomGen.RollRandomFloatInRange(0.9f, 1.1f);
	m_SFXBulletHitPlayIndex = g_theAudio->StartSound(m_SFXBulletHit,false,volume * m_globalVolume,0.f,pitch * m_SFXSpeedMultiplier * pitchVariation,m_gameClock->IsPaused());

}

void Game::HandleBeetleDeath()
{
	m_SFXEnemyHurtPlayIndex = g_theAudio->StartSound(m_SFXEnemyHurt, false, m_globalVolume * 1.f, 0.f, m_SFXSpeedMultiplier, m_gameClock->IsPaused());
}

void Game::HandleWaspDeath()
{
	m_SFXEnemyDeathPlayIndex = g_theAudio->StartSound(m_SFXEnemyDeath, false, m_globalVolume * 1.f, 0.f, m_SFXSpeedMultiplier, m_gameClock->IsPaused());

}

void Game::HandlePause(bool isPaused)
{
	m_playership->SetSFXPause(isPaused);
	if(m_BGMGameplayPlayIndex !=-1)g_theAudio->SetSoundPlaybackPause(m_BGMGameplayPlayIndex, isPaused);
	
}

void Game::HandleSlowMo(float speedMultiplier)
{
	m_playership->SetSFXSpeedMultiplier(speedMultiplier);
	m_playership->SetRotationSpeedMultiplier(1.f / speedMultiplier);//player can turn normally in slow-mo
	m_SFXSpeedMultiplier = speedMultiplier;
	if(m_BGMGameplayPlayIndex!=-1)g_theAudio->SetSoundPlaybackSpeed(m_BGMGameplayPlayIndex, speedMultiplier);
}

void Game::HandleGameWin()
{
	m_gameWon = true;
	m_SFXGameWinPlayIndex = g_theAudio->StartSound(m_SFXGameWin, false, m_globalVolume* 1.f, 0.f, m_SFXSpeedMultiplier, m_gameClock->IsPaused());
	StopSoundAtIndex(m_BGMGameplayPlayIndex);
	
}

void Game::HandleGameLost()
{
	m_gameLost = true;
	m_SFXGameLosePlayIndex = g_theAudio->StartSound(m_SFXGameLose, false, m_globalVolume * 1.f, 0.f, m_SFXSpeedMultiplier, m_gameClock->IsPaused());
	StopSoundAtIndex(m_BGMGameplayPlayIndex);
}

void Game::StopSoundAtIndex(SoundPlaybackID& soundPlaybackId)
{
	if (soundPlaybackId != -1) {
		g_theAudio->StopSound(soundPlaybackId);
		soundPlaybackId = static_cast<SoundPlaybackID>(-1);
	}
}

//delete the dead asteroids and bullets
void Game::DeleteGarbageEntities()
{
	
	DeleteGarbageAsteroid();
	DeleteGarbageBullet();
	DeleteGarbageDebris();
	DeleteGarbageBeetles();
	DeleteGarbageWasps();

}

void Game::CheckInput()
{
	XboxController const& controller = g_theInput->GetController(0);
	//pressing Space Bar, the ‘N’ key, or the controller Start button or controller A button
	if (m_inAttractMode) {
		if (g_theInput->WasKeyJustPressed('N') || g_theInput->WasKeyJustPressed(' ')|| controller.IsButtonDown(XBOX_BUTTON_A) || controller.IsButtonDown(XBOX_BUTTON_START)) {
			m_inAttractMode = false;
			StartGame();
		}

		if (g_theInput->WasKeyJustPressed(KEYCODE_ESC)) {
			g_theApp->HandleQuitRequested();
		}

		
	}
	else {
		//if pressing esc, goes back to attract mode
		if (g_theInput->WasKeyJustPressed(KEYCODE_ESC)) {
			BackToAttractMode();
		}
		//if pressing f8, restart game
		if (g_theInput->WasKeyJustPressed(KEYCODE_F8)) {
			ResetGame();
			SpawnWave(m_currentWave);
		}
		


		if (g_theInput->WasKeyJustPressed('T')|| g_theInput->WasKeyJustPressed(KEYCODE_SHIFT)|| controller.WasButtonJustPressed(XBOX_BUTTON_LSHOULDER))
		{
			m_gameClock->SetTimeScale(0.1);
			HandleSlowMo(0.1f);
		}

		if (g_theInput->WasKeyJustReleased('T') || g_theInput->WasKeyJustReleased(KEYCODE_SHIFT) || controller.WasButtonJustReleased(XBOX_BUTTON_LSHOULDER))
		{
			m_gameClock->SetTimeScale(1.0);
			HandleSlowMo(1.f);

		}


		if (g_theInput->WasKeyJustPressed('O'))
		{
			m_gameClock->StepSingleFrame();
		}

		if (g_theInput->WasKeyJustPressed('P'))
		{
			m_gameClock->TogglePause();
			HandlePause(m_gameClock->IsPaused());
		}


		//spawn asteroid if pressing I
		if (g_theInput->WasKeyJustPressed('I')) {
			SpawnRandomAsteroids(1);
		}

		//pressing F1 enable debug render
		if (g_theInput->WasKeyJustPressed(KEYCODE_F1)) {
			m_debugDraw = !m_debugDraw;
		}


	}

	

}



//see if the current wave is over
void Game::UpdateWaves()
{
	// if all enemies have been eliminated
	//if (m_numAsteroids == 0 && m_numBeetles == 0 && m_numWasps == 0 && !m_gameWon) {
	if (m_numBeetles == 0 && m_numWasps == 0 && !m_gameWon) {
		m_currentWave++;
		if (m_currentWave < MAX_WAVES) {
			SpawnWave(m_currentWave);
			m_SFXNextLevelPlayIndex = g_theAudio->StartSound(m_SFXNextLevel, false, 1.f, 0.f, m_SFXSpeedMultiplier, m_gameClock->IsPaused());
			
		}
		else {
			//Game win, back to attract mode
			m_timeWhenPlayerWins = static_cast<float>(GetCurrentTimeSeconds());
			HandleGameWin();

		}

	}
}

void Game::RenderLimitedLives() const
{
	//render ship icons based on current ship lives

	int shipIconNum = PLAYER_SHIP_MAX_HEALTH - m_respawns - 1;
	if (shipIconNum <= 0)return;

	//transform & scale relative to screen camera position
	Vec2 transformForAll = Vec2(SCREEN_SIZE_X / 40.f,SCREEN_SIZE_Y * 0.95f);
	constexpr float XOffsetForEachShip = SCREEN_SIZE_X / 40.f;
	constexpr float scaleForAll = SCREEN_SIZE_X / 200.f;
	

	for (int i = 0; i < shipIconNum;++i) {
		Vertex_PCU iconVerts[NUM_SHIP_VERTS];
		for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex) {
			iconVerts[vertIndex] = m_attractModeVertices[vertIndex];
		}
		TransformVertexArrayXY3D(NUM_SHIP_VERTS, iconVerts, scaleForAll, 90.f, Vec2(transformForAll.x + XOffsetForEachShip *i, transformForAll.y));
		g_theRenderer->DrawVertexArray(NUM_SHIP_VERTS, iconVerts);
	}

}

void Game::RenderShipTemperatureBar() const
{


	//render ship temperature as one debugRect and one debugBox
	constexpr float tempBarLength = SCREEN_SIZE_X / 30.f;
	constexpr float tempBarWidth = SCREEN_SIZE_Y / 60.f;

	Vec2 transformForAll = Vec2(SCREEN_SIZE_X * 0.5f, SCREEN_SIZE_Y * 0.95f);
	constexpr float scaleForAll = SCREEN_SIZE_X * 0.001f;
	float currentTemp = m_playership->GetCurrentTemp();
	float tempPercent = currentTemp * (1.f / PLAYER_SHIP_MAX_TEMPERATURE);

	Vec2 bottomLeft(-tempBarLength, -tempBarWidth);
	Vec2 topRight(tempBarLength, tempBarWidth);
	Vec2 topRightBar(tempBarLength * (tempPercent * 2.f - 1.f),tempBarWidth);

	TransformPosition2D(bottomLeft, scaleForAll, 0.f, transformForAll);
	TransformPosition2D(topRight, scaleForAll, 0.f, transformForAll);
	TransformPosition2D(topRightBar, scaleForAll, 0.f, transformForAll);


	//bar changing from green to red as temperature goes up
	
	int currentRed = currentTemp == 0.f? 0 : RoundDownToInt(tempPercent * 255.f);
	
	Rgba8 barColor(static_cast<unsigned char>(currentRed), static_cast<unsigned char>(255-currentRed),0,255);

	DebugDrawRectangle(bottomLeft, topRightBar, barColor);

	//draw the containing box
	constexpr float boxThickness = SCREEN_SIZE_X * 0.0005f;
	Rgba8 boxColor(255,255,255,255);

	DebugDrawBox(bottomLeft, topRight, boxColor, boxThickness);




}

void Game::RenderMinimap() const
{

	Vec2 transformForAll = Vec2(SCREEN_SIZE_X * 0.9f, SCREEN_SIZE_Y * 0.9f);
	constexpr float scaleForAll = SCREEN_SIZE_Y * 0.00025f;
	//constexpr float scaleForAll = 1.f;
	constexpr float boxThickness = SCREEN_SIZE_Y * 0.002f;

	//draw the world map as a rectangle proportional to screen size

	Rgba8 worldMapColor(255, 255, 255);
	Rgba8 worldMapBGColor(0, 0, 0);

	//float worldScaledX = (WORLD_SIZE_X / SCREEN_SIZE_X) * 0.025f;
	//float worldScaledY = (WORLD_SIZE_Y / SCREEN_SIZE_Y) * 0.025f;

	//choose the smaller scale in the two
	//float generalScale = worldScaledX <= worldScaledY? worldScaledX : worldScaledY;

	Vec2 worldMapBottomLeft = Vec2(-WORLD_SIZE_X * 0.5f, -WORLD_SIZE_Y * 0.5f);// *generalScale;
	Vec2 worldMapTopRight = Vec2(WORLD_SIZE_X * 0.5f, WORLD_SIZE_Y * 0.5f);// *generalScale;


	TransformPosition2D(worldMapBottomLeft, scaleForAll, 0.f, transformForAll);
	TransformPosition2D(worldMapTopRight, scaleForAll, 0.f, transformForAll);

	DebugDrawRectangle(worldMapBottomLeft, worldMapTopRight, worldMapBGColor);
	DebugDrawBox(worldMapBottomLeft, worldMapTopRight, worldMapColor, boxThickness);

	
	//draw world cam as rectangle
	Rgba8 worldCamColor(255, 255, 255);

	Vec2 worldCamBottomLeft = m_worldCamera.GetOrthographicBottomLeft();
	Vec2 wCBLScaled(RangeMapClamped(worldCamBottomLeft.x, 0.f, WORLD_SIZE_X, worldMapBottomLeft.x, worldMapTopRight.x), RangeMapClamped(worldCamBottomLeft.y, 0.f, WORLD_SIZE_Y, worldMapBottomLeft.y, worldMapTopRight.y));

	Vec2 worldCamTopRight = m_worldCamera.GetOrthographicTopRight();
	Vec2 wCTRScaled(RangeMapClamped(worldCamTopRight.x, 0.f, WORLD_SIZE_X, worldMapBottomLeft.x, worldMapTopRight.x), RangeMapClamped(worldCamTopRight.y, 0.f, WORLD_SIZE_Y, worldMapBottomLeft.y, worldMapTopRight.y));

	
	DebugDrawBox(wCBLScaled, wCTRScaled, worldCamColor, boxThickness);


	//draw entities as dots
	Vec2 entityPos;
	Vec2 entityPosRangeMapped;
	//float entityScale = (BEETLE_COSMETIC_RADIUS / SCREEN_SIZE_Y) * 0.5f;
	float entityScale = SCREEN_SIZE_Y * 0.01f;

	//beetles
	for (int i = 0; i < MAX_BEETLES; i++) {
		if (m_beetles[i] == nullptr)continue;
		entityPos = m_beetles[i]->GetPosition();
		entityPosRangeMapped = Vec2(RangeMapClamped(entityPos.x, 0.f, WORLD_SIZE_X, worldMapBottomLeft.x, worldMapTopRight.x), RangeMapClamped(entityPos.y, 0.f, WORLD_SIZE_Y, worldMapBottomLeft.y, worldMapTopRight.y));
		DebugDrawDot(entityPosRangeMapped, entityScale, m_beetles[i]->GetColor());
	}


	//wasps
	for (int i = 0; i < MAX_WASPS; i++) {
		if (m_wasps[i] == nullptr)continue;
		entityPos = m_wasps[i]->GetPosition();
		entityPosRangeMapped = Vec2(RangeMapClamped(entityPos.x, 0.f, WORLD_SIZE_X, worldMapBottomLeft.x, worldMapTopRight.x), RangeMapClamped(entityPos.y, 0.f, WORLD_SIZE_Y, worldMapBottomLeft.y, worldMapTopRight.y));
		DebugDrawDot(entityPosRangeMapped, entityScale, m_wasps[i]->GetColor());
	}


	//asteroids
	for (int i = 0; i < MAX_WASPS; i++) {
		if (m_asteroids[i] == nullptr)continue;
		entityPos = m_asteroids[i]->GetPosition();
		entityPosRangeMapped = Vec2(RangeMapClamped(entityPos.x, 0.f, WORLD_SIZE_X, worldMapBottomLeft.x, worldMapTopRight.x), RangeMapClamped(entityPos.y, 0.f, WORLD_SIZE_Y, worldMapBottomLeft.y, worldMapTopRight.y));
		DebugDrawDot(entityPosRangeMapped, entityScale, m_asteroids[i]->GetColor());
	}


}

void Game::RenderPauseOverlay() const
{
	if (!m_gameClock->IsPaused())return;
	DebugDrawRectangle(Vec2(), m_screenCamOffset * 2.f, Rgba8(0, 0, 0, 127));
}

void Game::RenderEntities()const
{
	//render each of entities
	

	for (int i = 0; i < MAX_BULLETS; ++i) {
		if (m_bullets[i] != nullptr)m_bullets[i]->Render();
	}

	for (int i = 0; i < MAX_ASTEROIDS; ++i) {
		if (m_asteroids[i] != nullptr)m_asteroids[i]->Render();
	}

	for (int i = 0; i < MAX_BEETLES; ++i) {
		if (m_beetles[i] != nullptr)m_beetles[i]->Render();
	}

	for (int i = 0; i < MAX_WASPS; ++i) {
		if (m_wasps[i] != nullptr)m_wasps[i]->Render();
	}

	for (int i = 0; i < MAX_DEBRIS; ++i) {
		if (m_debris[i] != nullptr)m_debris[i]->Render();
	}

	//render player ship last to prevent blocking the ship body
	m_playership->Render();

	//if (m_debugDraw)m_playership->DebugRender();

	if (m_debugDraw)DebugRender();

}

void Game::RenderStarryBackground() const
{
	g_theRenderer->DrawVertexArray(TOTAL_STAR_VERTEX, m_totalStarVertices);
}

void Game::RenderAttractMode() const
{
	//float scaleForAll = 10.f;
	//Vec2 transformForAll(100.f,50.f);

	float scaleForAll = SCREEN_SIZE_X / 20.f;
	Vec2 transformForAll(SCREEN_SIZE_X / 2.f,SCREEN_SIZE_Y/2.f);


	//transform the first ship
	Vertex_PCU firstShipVerts[NUM_SHIP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex) {
		firstShipVerts[vertIndex] = m_attractModeVertices[vertIndex];
	}
	
	Vec2 transformForShip1 = Vec2(transformForAll.x - scaleForAll * 5, transformForAll.y) + m_attractShip1Offset;
	TransformVertexArrayXY3D(NUM_SHIP_VERTS, firstShipVerts, scaleForAll, m_attractShip1OrientationDeg, transformForShip1);
	

	//transform & rotate the second ship
	Vertex_PCU secondShipVerts[NUM_SHIP_VERTS];
	for (int vertIndex = NUM_SHIP_VERTS; vertIndex < NUM_SHIP_VERTS*2; ++vertIndex) {
		secondShipVerts[vertIndex-NUM_SHIP_VERTS] = m_attractModeVertices[vertIndex];
	}
	Vec2 transformForShip2 = Vec2(transformForAll.x + scaleForAll * 5, transformForAll.y) + m_attractShip2Offset;

	TransformVertexArrayXY3D(NUM_SHIP_VERTS, secondShipVerts, scaleForAll, m_attractShip2OrientationDeg, transformForShip2);


	//transform the triangle
	Vertex_PCU TriangleVerts[3];
	for (int vertIndex = NUM_SHIP_VERTS*2; vertIndex < (NUM_SHIP_VERTS * 2)+3; ++vertIndex) {
		TriangleVerts[vertIndex- (NUM_SHIP_VERTS * 2)] = m_attractModeVertices[vertIndex];
	}

	TransformVertexArrayXY3D(3, TriangleVerts, scaleForAll, 0.f, transformForAll);

	//combine all vertices to one array
	Vertex_PCU tempModeWorldVerts[ATTRACT_MODE_VERTS];

	for (int i = 0; i < ATTRACT_MODE_VERTS;i++) {
		if (i < NUM_SHIP_VERTS) {
			tempModeWorldVerts[i] = firstShipVerts[i];
		}
		else if (i< NUM_SHIP_VERTS*2) {
			tempModeWorldVerts[i] = secondShipVerts[i-NUM_SHIP_VERTS];
		}
		else {
			tempModeWorldVerts[i] = TriangleVerts[i - (NUM_SHIP_VERTS*2)];
		}
	}

	g_theRenderer->DrawVertexArray(ATTRACT_MODE_VERTS, tempModeWorldVerts);
}

void Game::DebugRender() const
{
	//draw a dark gray line from playership to center of each entity
	Rgba8 connectionLineCol(50,50,50);
	float thickness = DEBUG_THICKNESS;

	for (int i = 0; i < MAX_BULLETS; ++i) {
		if (m_bullets[i] == nullptr)continue;
		DebugDrawLine(m_bullets[i]->GetPosition(), m_playership->GetPosition(), connectionLineCol, thickness);
		m_bullets[i]->DebugRender();
	}

	for (int i = 0; i < MAX_ASTEROIDS; ++i) {
		if (m_asteroids[i] == nullptr)continue;
		DebugDrawLine(m_asteroids[i]->GetPosition(), m_playership->GetPosition(), connectionLineCol, thickness);
		m_asteroids[i]->DebugRender();
	}

	for (int i = 0; i < MAX_BEETLES; ++i) {
		if (m_beetles[i]==nullptr)continue;
		DebugDrawLine(m_beetles[i]->GetPosition(), m_playership->GetPosition(), connectionLineCol, thickness);
		m_beetles[i]->DebugRender();
	}

	for (int i = 0; i < MAX_DEBRIS; ++i) {
		if (m_debris[i] == nullptr)continue;
		DebugDrawLine(m_debris[i]->GetPosition(), m_playership->GetPosition(), connectionLineCol, thickness);
		m_debris[i]->DebugRender();
	}

	for (int i = 0; i < MAX_WASPS; ++i) {
		if (m_wasps[i] == nullptr)continue;
		DebugDrawLine(m_wasps[i]->GetPosition(), m_playership->GetPosition(), connectionLineCol, thickness);
		m_wasps[i]->DebugRender();
	}



	//render player ship last to prevent blocking the ship body
	m_playership->DebugRender();
}

void Game::UpdateCameras()
{
	m_screenCamera.SetOrthographicView(Vec2(0.f,0.f), Vec2(SCREEN_SIZE_X,SCREEN_SIZE_Y));

	UpdateCamFollowPlayer();

	//world camera applying current level of camera shake
	//trauma = [0,1]
	m_traumaLevel = GetClampedZeroToOne(m_traumaLevel);
	float camShakeAmount = m_traumaLevel * MAX_CAM_SHAKE_AMT;
	
	
	float worldOffsetX = m_randomGen.RollRandomFloatInRange(-camShakeAmount, camShakeAmount);
	float worldOffsetY = m_randomGen.RollRandomFloatInRange(-camShakeAmount, camShakeAmount);

	Vec2 displacement = Vec2(worldOffsetX, worldOffsetY);
	m_worldCamera.Translate2D(displacement);

	//camera shake decrease over time in a decreasing rate
	//drop drastically at high trauma, but decrease slowly at low trauma
	//based on delta seconds
	if (m_traumaLevel > 0.f) {
		m_traumaLevel -= m_traumaLevel * static_cast<float>(m_gameClock->GetDeltaSeconds()) * 2;
		//m_traumaLevel -= m_traumaLevel * 0.07f;

	}
	

}

void Game::UpdateCamFollowPlayer()
{
	Vec2 playerPos = m_playership->GetPosition();


	//set the view of the world camera based on player world position
	Vec2 worldBottomLeft = playerPos - m_worldCamOffset;
	Vec2 worldTopRight = playerPos + m_worldCamOffset;
	
	//clamp the camera location if the player goes to the edge of the world

	if (playerPos.x < 0.f + WORLD_CAM_SIZE_X * 0.5) {//if player get close to the left edge
		worldBottomLeft.x = 0.f;
		worldTopRight.x = WORLD_CAM_SIZE_X;
	}
	if (playerPos.y < 0.f + WORLD_CAM_SIZE_Y * 0.5) {//if player get close to the bottom edge
		worldBottomLeft.y = 0.f;
		worldTopRight.y = WORLD_CAM_SIZE_Y;
	}
	if (playerPos.x > WORLD_SIZE_X - WORLD_CAM_SIZE_X * 0.5) {//if player get close to the right edge
		worldBottomLeft.x = WORLD_SIZE_X - WORLD_CAM_SIZE_X;
		worldTopRight.x = WORLD_SIZE_X;
	}
	if (playerPos.y > WORLD_SIZE_Y - WORLD_CAM_SIZE_Y * 0.5) {//if player get close to the top edge
		worldBottomLeft.y = WORLD_SIZE_Y - WORLD_CAM_SIZE_Y;
		worldTopRight.y = WORLD_SIZE_Y;
	}


	//set world camera
	m_worldCamera.SetOrthographicView(worldBottomLeft, worldTopRight);

	
}

void Game::MoveCamTo(Camera& cam, Vec2 position)
{
	Vec2 camOffset((cam.GetOrthographicTopRight().x - cam.GetOrthographicBottomLeft().x)*0.5f, (cam.GetOrthographicTopRight().y - cam.GetOrthographicBottomLeft().y)*0.5f);

	cam.SetOrthographicView(position-camOffset,position+camOffset);

}

void Game::InitializeWaves(int numA, int numB, int numW)
{
	m_waves[0].m_numAsteroidSpawn = numA;
	m_waves[0].m_numBeetleSpawn = numB;
	m_waves[0].m_numWaspSpawn = numW;

	m_currentWave = 0;


	for (int i = 1; i < MAX_WAVES;++i) {
		

		//incremental
		m_waves[i].m_numAsteroidSpawn = GetTopClampedInt(numA + static_cast<int>(WAVE_MULTIPLIER * i), MAX_ASTEROIDS);
		m_waves[i].m_numBeetleSpawn = GetTopClampedInt(numB + static_cast<int>(WAVE_MULTIPLIER * i),MAX_BEETLES);
		m_waves[i].m_numWaspSpawn = GetTopClampedInt(numW + static_cast<int>(WAVE_MULTIPLIER * i),MAX_WASPS);
		

	}

}

void Game::SpawnWave(int waveNum)
{
	SpawnRandomAsteroids(m_waves[waveNum].m_numAsteroidSpawn);
	SpawnBeetles(m_waves[waveNum].m_numBeetleSpawn);
	SpawnWasps(m_waves[waveNum].m_numWaspSpawn);
}


void Game::SpawnAsteroid(Vec2 pos, float orientationDeg)
{
	if (m_numAsteroids == MAX_ASTEROIDS) {
		ERROR_RECOVERABLE("Cannot spawn more asteroids. All slots are full.");
		return;
	}
	

	for (int i = 0; i < MAX_ASTEROIDS; ++i) {
		//spawn at random positions along the edge of the world, just outside the world

		if (m_asteroids[i] == nullptr) {
			m_asteroids[i] = new Asteroid(this, pos, orientationDeg);
			++m_numAsteroids;
			break;
		}
	}
}

void Game::SpawnRandomAsteroids(int numAst)
{

	if (m_numAsteroids == MAX_ASTEROIDS || m_numAsteroids + numAst > MAX_ASTEROIDS) {
		ERROR_RECOVERABLE("Cannot spawn more asteroids. All slots are full.");
		return;
	}


	Vec2 position;
	float orientationDeg;
	int count = 0;

	for (int i = 0; i < MAX_ASTEROIDS && count < numAst; ++i) {
		//spawn at random positions along the edge of the world, just outside the world

		position = GetRandomPosOffscreen(ASTEROID_COSMETIC_RADIUS);
		orientationDeg = m_randomGen.RollRandomFloatInRange(0, 360);

		if (m_asteroids[i] == nullptr) {
			m_asteroids[i] = new Asteroid(this, position, orientationDeg);
			count++;
		}
	}
	m_numAsteroids += numAst;
}

void Game::SpawnBullet(Vec2 pos, float orientationDeg)
{
	if (m_numBullets == MAX_BULLETS) {
		ERROR_RECOVERABLE("Cannot spawn more bullets. All slots are full.");
		return;
	}

	for (int i = 0; i < MAX_BULLETS; ++i) {

		if (m_bullets[i] == nullptr) {
			m_bullets[i] = new Bullet(this, pos, orientationDeg);
			++m_numBullets;
			break;
		}
	}
}

void Game::SpawnBeetles(int numB)
{
	if (m_numBeetles == MAX_BEETLES || m_numBeetles + numB > MAX_BEETLES) {
		ERROR_RECOVERABLE("Cannot spawn more beetles. Not enough slots for beetles.");
		return;
	}


	Vec2 position;
	float orientationDeg;
	int count = 0;

	for (int i = 0; i < MAX_BEETLES && count < numB; ++i) {
		//spawn at random positions along the edge of the world, just outside the world

		position = GetRandomPosOffscreen(BEETLE_COSMETIC_RADIUS);
		orientationDeg = 0.f;

		if (m_beetles[i] == nullptr) {
			m_beetles[i] = new Beetle(this, position, orientationDeg);
			count++;
		}
	}
	m_numBeetles += numB;
}

void Game::SpawnWasps(int numW)
{
	if (m_numWasps == MAX_WASPS || m_numWasps + numW > MAX_WASPS) {
		ERROR_RECOVERABLE("Cannot spawn more wasps. Not enough slots for wasps.");
		return;
	}


	Vec2 position;
	float orientationDeg;
	int count = 0;

	for (int i = 0; i < MAX_WASPS && count < numW; ++i) {
		//spawn at random positions along the edge of the world, just outside the world

		position = GetRandomPosOffscreen(WASP_COSMETIC_RADIUS);

		orientationDeg = 0.f;
		if (m_wasps[i] == nullptr) {
			m_wasps[i] = new Wasp(this, position, orientationDeg);
			count++;
		}
	}
	m_numWasps += numW;
}



void Game::SpawnNewDebrisCluster(int numD, Vec2 const& position, Vec2 const& avgVelocity, float maxScatterSpeed, float avgRadius, Rgba8 const& color)
{

	if (m_numDebris == MAX_DEBRIS || m_numDebris + numD > MAX_DEBRIS) {
		ERROR_RECOVERABLE("Cannot spawn more debris. Not enough slots for debris.");
		return;
	}


	float thetaDeg;
	float speed;
	Vec2 scatterVel;
	Vec2 velocity;
	float orientationDeg;
	int count = 0;

	for (int i = 0; i < MAX_DEBRIS && count < numD;++i) {
		if (m_debris[i] == nullptr) {
			thetaDeg = m_randomGen.RollRandomFloatInRange(0.f, 360.f);
			speed = m_randomGen.RollRandomFloatInRange(0.f, maxScatterSpeed);
			scatterVel = Vec2::MakeFromPolarDegrees(thetaDeg, speed);
			velocity = avgVelocity + scatterVel;
			orientationDeg = m_randomGen.RollRandomFloatInRange(0.f, 360.f);
			m_debris[i] = new Debris(this,position, orientationDeg, velocity, avgRadius, color);
			count++;
		}
	}
}

void Game::SpawnDebrisForShip(int minD, int maxD)
{

	SpawnNewDebrisCluster(m_randomGen.RollRandomIntInRange(minD, maxD),
		m_playership->GetPosition(),
		m_playership->GetVelocity(),
		MAX_DEBRIS_SCATTER_SPEED,
		PLAYER_SHIP_COSMETIC_RADIUS * 0.5f,
		m_playership->GetColor());
}



void Game::DeleteGarbageAsteroid()
{
	for (int i = 0; i < MAX_ASTEROIDS; i++) {
		if (m_asteroids[i] != nullptr && m_asteroids[i]->IsGarbage()) {
			// If dead asteroid found in array 
			delete m_asteroids[i];
			m_asteroids[i] = nullptr;
			m_numAsteroids--;
		}
	}
}

void Game::DeleteGarbageBullet()
{
	for (int i = 0; i < MAX_BULLETS; i++) {
		if (m_bullets[i] != nullptr && m_bullets[i]->IsGarbage()) {
			// If dead bullet found in array 
			delete m_bullets[i];
			m_bullets[i] = nullptr;
			m_numBullets--;
		}
	}
}

void Game::DeleteGarbageDebris()
{
	for (int i = 0; i < MAX_DEBRIS; i++) {
		if (m_debris[i]!=nullptr && m_debris[i]->IsGarbage()) {
			// If dead debris found in array 
			delete m_debris[i];
			m_debris[i] = nullptr;
			m_numDebris--;
		}
	}
}

void Game::DeleteGarbageBeetles()
{
	for (int i = 0; i < MAX_BEETLES; i++) {
		if (m_beetles[i] != nullptr && m_beetles[i]->IsGarbage()) {
			// If dead debris found in array 
			delete m_beetles[i];
			m_beetles[i] = nullptr;
			m_numBeetles--;
		}
	}
}

void Game::DeleteGarbageWasps()
{
	for (int i = 0; i < MAX_WASPS; i++) {
		if (m_wasps[i] != nullptr && m_wasps[i]->IsGarbage()) {
			// If dead debris found in array 
			delete m_wasps[i];
			m_wasps[i] = nullptr;
			m_numWasps--;
		}
	}
}

Vec2 Game::GetRandomPosOffscreen(float radius)const
{

	RandomNumberGenerator randGen;
	float offsetX, offsetY;
	int X = randGen.RollRandomIntInRange(0,1);
	if (X == 0)offsetX = 0.f - radius;
	else offsetX = WORLD_SIZE_X + radius;

	int Y = randGen.RollRandomIntInRange(0, 1);
	if (Y == 0)offsetY = 0.f - radius;
	else offsetY = WORLD_SIZE_Y + radius;

	int disableRandXOrY = randGen.RollRandomIntInRange(0, 1);
	if (disableRandXOrY == 0) {
		//randomize x location, keep y as offsetY
		//appear from top/bottom of screen
		float locX = randGen.RollRandomFloatInRange(0.f-radius, WORLD_SIZE_X+radius);
		return Vec2(locX,offsetY);
	}
	else {
		//appear from left/right of screen
		float locY = randGen.RollRandomFloatInRange(0.f - radius, WORLD_SIZE_Y + radius);
		return Vec2(offsetX, locY);
	}



}

Vec2 Game::GetShipPos() const
{
	return m_playership->GetPosition();
}

bool Game::GetShipAlive() const
{
	return m_playership->IsAlive();
}

bool Game::IsInAttractMode() const
{
	return m_inAttractMode;
}

int Game::GetPlayerLives() const
{
	return m_currentShipLives;
}

float Game::GetGlobalVolume() const
{
	return m_globalVolume;
}

void Game::IncreaseRespawnNum()
{
	m_respawns++;
}

void Game::SetGlobalVolume(float volume)
{
	m_globalVolume = volume;
}
