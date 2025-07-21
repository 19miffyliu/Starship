#include "PlayerShip.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/Bullet.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"



PlayerShip::PlayerShip(Game* g, Vec2 pos, float orientationDeg): Entity(g, pos, orientationDeg)
{
	m_physicsRadius = PLAYER_SHIP_PHYSICS_RADIUS;
	m_cosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
	InitializeLocalVerts();

	m_SFXPlayershipFireBulletShort = g_theAudio->CreateOrGetSound("Data/Audio/Laser_Shoot_Short.wav");
	m_SFXPlayershipFireBulletLong = g_theAudio->CreateOrGetSound("Data/Audio/Laser_Shoot_Long.wav");
	m_SFXPlayershipFireBulletWide = g_theAudio->CreateOrGetSound("Data/Audio/Laser_Shoot_Wide.wav");
	m_SFXPlayershipThrust = g_theAudio->CreateOrGetSound("Data/Audio/Thrust_Player_1.wav");
	m_SFXPlayershipRespawn = g_theAudio->CreateOrGetSound("Data/Audio/Respawn.wav");
}

void PlayerShip::Update(float deltaSeconds)
{
	

	UpdateFromKeyboard(deltaSeconds);
	UpdateFromController(deltaSeconds);

	UpdateThrustFlame();

	
	Vec2 combinedVelocityOffset = m_velocityOffsetKeyboard + m_velocityOffsetController;

	float radius = combinedVelocityOffset.GetLength();
	float orieDeg = Atan2Degrees(combinedVelocityOffset.y, combinedVelocityOffset.x);

	float ClampedR = GetClamped(radius, radius, PLAYER_SHIP_ACCELERATION*deltaSeconds);

	m_velocity += Vec2::MakeFromPolarDegrees(orieDeg, ClampedR);

	BounceOffWalls();

	m_position += m_velocity*deltaSeconds;

	// if just respawned, get invincibility for 3s
	Rgba8 currentCol = m_shipColor;
	float timeSinceActiviateInvincibility = static_cast<float>(GetCurrentTimeSeconds()) - m_timeWhenActivateInvincibility;
	if (m_isInvincible && timeSinceActiviateInvincibility < 3.f) {
		float invincibleColorOpacityPercent = (3.f - timeSinceActiviateInvincibility ) / 3.f;


		int r = static_cast<int>(m_shipColor.r);
		unsigned char currentR = static_cast<unsigned char>(invincibleColorOpacityPercent * (255-r) + r);
		int g = static_cast<int>(m_shipColor.g);
		unsigned char currentG = static_cast<unsigned char>(invincibleColorOpacityPercent * (255 - g) + g);
		int b = static_cast<int>(m_shipColor.b);
		unsigned char currentB = static_cast<unsigned char>(invincibleColorOpacityPercent * (255 - b) + b);

		//dereasing from white to normal color
		currentCol = Rgba8(currentR, currentG, currentB);

	}
	else {
		m_isInvincible = false;
	}

	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS-3; ++vertIndex) {
		m_localVerts[vertIndex].m_color = currentCol;
	}


}

//transform the local playership verts to world verts, then render
void PlayerShip::Render() const
{
	if (m_isDead)return;

	Vertex_PCU tempShipWorldVerts[NUM_SHIP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; ++vertIndex) {
		tempShipWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_SHIP_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_SHIP_VERTS, tempShipWorldVerts);
}

void PlayerShip::UpdateThrustFlame()
{
	RandomNumberGenerator randomGen;
	m_thrustFlameLength = randomGen.RollRandomFloatInRange(0.f, m_thrustFraction*2.f);
	m_localVerts[16].m_position = Vec3(-4.f-m_thrustFlameLength, 0.f, 0.f);

	int fireOpacity = RoundDownToInt(255.f * m_thrustFraction);
	int inverseOpacity = 255 - fireOpacity;

	Rgba8 fireColor = Rgba8(static_cast<unsigned char>(inverseOpacity), 0, static_cast<unsigned char>(fireOpacity), static_cast<unsigned char>(fireOpacity));
	m_localVerts[15].m_color = fireColor;
	m_localVerts[17].m_color = fireColor;

	m_thrustFlameColor.r = static_cast<unsigned char>(fireOpacity);
	m_thrustFlameColor.a = static_cast<unsigned char>(fireOpacity);
	m_localVerts[16].m_color = m_thrustFlameColor;
}

bool PlayerShip::IsOffscreen() const
{
	bool overX = m_position.x + PLAYER_SHIP_COSMETIC_RADIUS < 0.f || m_position.x - PLAYER_SHIP_COSMETIC_RADIUS > WORLD_SIZE_X;
	bool overY = m_position.y + PLAYER_SHIP_COSMETIC_RADIUS < 0.f || m_position.y - PLAYER_SHIP_COSMETIC_RADIUS > WORLD_SIZE_Y;
		
	return overX || overY;
}

Vertex_PCU* PlayerShip::GetLocalVerts() const
{
	static Vertex_PCU temp[NUM_SHIP_VERTS];
	for (int i = 0; i < NUM_SHIP_VERTS; ++i) {
		temp[i] = m_localVerts[i];
	}
	return temp;
}

Vertex_PCU PlayerShip::GetVertByIndex(int index) const
{
	if (index >= NUM_SHIP_VERTS) {
		ERROR_RECOVERABLE("Cannot get vert, index out of bounds.");
		return Vertex_PCU();
	}
	return m_localVerts[index];
}

int PlayerShip::GetVertexNum() const
{
	return NUM_SHIP_VERTS;
}

float PlayerShip::GetCurrentTemp() const
{
	return m_currentTemperature;
}

bool PlayerShip::IsInvincible() const
{
	return m_isInvincible;
}



//initialize the player ship verts based on relative position
void PlayerShip::InitializeLocalVerts()
{
	Vec2 texCoord = Vec2(0.f, 0.f);
	
	m_color = m_shipColor;



	//local

	//triangle A
	m_localVerts[0].m_position = Vec3(0.f, 2.f, 0.f);
	m_localVerts[1].m_position = Vec3(-2.f, 1.f, 0.f);
	m_localVerts[2].m_position = Vec3(2.f, 1.f, 0.f);

	//triangle B
	m_localVerts[3].m_position = Vec3(-2.f, 1.f, 0.f);
	m_localVerts[4].m_position = Vec3(-2.f, -1.f, 0.f);
	m_localVerts[5].m_position = Vec3(0.f, 1.f, 0.f);

	//triangle C
	m_localVerts[6].m_position = Vec3(0.f, 1.f, 0.f);
	m_localVerts[7].m_position = Vec3(-2.f, -1.f, 0.f);
	m_localVerts[8].m_position = Vec3(0.f, -1.f, 0.f);

	//triangle D
	m_localVerts[9].m_position = Vec3(0.f, 1.f, 0.f);
	m_localVerts[10].m_position = Vec3(0.f, -1.f, 0.f);
	m_localVerts[11].m_position = Vec3(1.f, 0.f, 0.f);
	m_bulletSpawnPoint = Vec2(m_localVerts[11].m_position.x, m_localVerts[11].m_position.y);

	//triangle E
	m_localVerts[12].m_position = Vec3(-2.f, -1.f, 0.f);
	m_localVerts[13].m_position = Vec3(0.f, -2.f, 0.f);
	m_localVerts[14].m_position = Vec3(2.f, -1.f, 0.f);

	//triangle F: thrust flame
	m_localVerts[15].m_position = Vec3(-2.f, 1.f, 0.f);
	m_localVerts[17].m_position = Vec3(-2.f, 0.f, 0.f);
	m_localVerts[17].m_position = Vec3(-2.f, -1.f, 0.f);
	

	//color
	for (int i = 0; i < NUM_SHIP_VERTS; ++i) {
		m_localVerts[i].m_color = m_color;
	}
	
	

}

void PlayerShip::UpdateFromKeyboard(float deltaSeconds)
{
	m_velocityOffsetKeyboard = Vec2();
	if (m_isDead) {
		//Respawns (at screen center, facing right/east, with zero velocity) if N is pressed while dead
		if (g_theInput->WasKeyJustPressed('N') && m_game->GetPlayerLives() > 0) {
			Respawn();
			m_game->IncreaseRespawnNum();
			m_SFXPlayershipRespawnPlayIndex = g_theAudio->StartSound(m_SFXPlayershipRespawn, false, m_game->GetGlobalVolume(), 0.f, m_SFXSpeedMultiplier, m_isPaused);

			
		}
		else {
			//stop thrusting sound
			StopThrustingSound();
			return;
		}


	}

	//Turns (at 300 degrees per second) while S or F key is held (but not if both held)
	if (g_theInput->IsKeyDown('S')) {
		m_orientationDegrees += (PLAYER_SHIP_TURN_SPEED * m_rotationSpeedMultiplier * deltaSeconds);
	}
	if (g_theInput->IsKeyDown('F')) {
		m_orientationDegrees -= (PLAYER_SHIP_TURN_SPEED * m_rotationSpeedMultiplier * deltaSeconds);
	}
	
	//Accelerates (30 world units/second/second) in its forward direction while E key is held
	if (g_theInput->IsKeyDown('E')) {
		Vec2 fowardNormal = GetForwardNormal();
		m_velocityOffsetKeyboard = fowardNormal * PLAYER_SHIP_ACCELERATION * deltaSeconds;
		m_thrustFraction = 1.f;
		if (m_SFXPlayershipThrustPlayIndex == -1) {
			m_SFXPlayershipThrustPlayIndex = g_theAudio->StartSound(m_SFXPlayershipThrust, true, m_game->GetGlobalVolume() * m_thrustFraction * 0.3f, 0.f, m_SFXSpeedMultiplier * m_thrustFraction * 0.5f, m_isPaused);
		}
		else {
			g_theAudio->SetSoundPlaybackVolume(m_SFXPlayershipThrustPlayIndex, m_game->GetGlobalVolume() * m_thrustFraction * 0.3f);
			g_theAudio->SetSoundPlaybackSpeed(m_SFXPlayershipThrustPlayIndex, m_SFXSpeedMultiplier * m_thrustFraction * 0.5f);
			g_theAudio->SetSoundPlaybackPause(m_SFXPlayershipThrustPlayIndex, false);
		}
		m_controlledByKeyboard = true;
	}
	else {
		m_thrustFraction = 0.f;
		if (m_SFXPlayershipThrustPlayIndex != -1) {
			g_theAudio->SetSoundPlaybackPause(m_SFXPlayershipThrustPlayIndex,true);

		}
		m_controlledByKeyboard = false;

	}
	

	//Fires a bullet (at its “nose”, facing/moving in its forward direction) when space is pressed
	// does NOT fire multiple bullets if space is held down
	if (g_theInput->WasKeyJustPressed(' ')) {
		HandleFireBulletSFXs(0);
		SpawnBullet(m_orientationDegrees);
		m_timeWhenHoldingFireButtonNormal = static_cast<float>(GetCurrentTimeSeconds());
		
	}
	else if (g_theInput->IsKeyDown(' ') && static_cast<float>(GetCurrentTimeSeconds()) - m_timeWhenHoldingFireButtonNormal > TIME_BEFORE_START_RAPIDFIRE && m_currentTemperature < PLAYER_SHIP_MAX_TEMPERATURE * MAX_RAPIDFIRE_ALLOWED_TEMP_PERCENT) {//if holding button for a period of time
		
		float currentProjectileDeviation = m_currentTemperature * (1 / PLAYER_SHIP_MAX_TEMPERATURE) * SHIP_MAX_PROJECTILE_DEVIATION_DEG;
		

		HandleFireBullets(0, currentProjectileDeviation, deltaSeconds);
	}
	else if (g_theInput->WasKeyJustPressed('R') && m_currentTemperature < PLAYER_SHIP_MAX_TEMPERATURE * MAX_STARTBURST_ALLOWED_TEMP_PERCENT) {
		HandleFireBulletSFXs(2);
		HandleFireBullets(2, 0.f, deltaSeconds);
		m_timeWhenHoldingFireButtonWide = static_cast<float>(GetCurrentTimeSeconds());

	}
	else {//if not holding button
		//decreasing faster if temperature is high
		//float tempPercent = m_currentTemperature / PLAYER_SHIP_MAX_TEMPERATURE;
		//m_currentTemperature -= BULLET_TEMPERATURE_DECAY_RATE * tempPercent;

		m_currentTemperature -= BULLET_TEMPERATURE_DECAY_RATE;
		m_currentTemperature = GetClamped(m_currentTemperature, 0.f, PLAYER_SHIP_MAX_TEMPERATURE);
	}





	






}

// physic disc is snapped in-bounds, the x(or y) velocity is reversed
void PlayerShip::BounceOffWalls()
{
	if (m_position.x - m_physicsRadius < 0.f) {
		m_position.x = m_physicsRadius;

		m_velocity.x *= -1.f;
	}

	if (m_position.x + m_physicsRadius > WORLD_SIZE_X) {
		m_position.x = WORLD_SIZE_X - m_physicsRadius;

		m_velocity.x *= -1.f;
	}


	if (m_position.y - m_physicsRadius < 0.f) {
		m_position.y = m_physicsRadius;

		m_velocity.y *= -1.f;
	}

	if (m_position.y + m_physicsRadius > WORLD_SIZE_Y) {
		m_position.y = WORLD_SIZE_Y - m_physicsRadius;

		m_velocity.y *= -1.f;
	}

}

void PlayerShip::UpdateFromController(float deltaSeconds)
{
	XboxController const& controller = g_theInput->GetController(0);
	m_velocityOffsetController = Vec2();
	//respawn
	if (m_isDead) {
		//Respawns (at screen center, facing right/east, with zero velocity) if N is pressed while dead
		if (controller.WasButtonJustPressed(XBOX_BUTTON_START)&&m_game->GetPlayerLives()>0) {
			Respawn();
			m_game->IncreaseRespawnNum();
		}
		else {
			return;
		}


	}

	//move
	
	float leftStickMagnitude = controller.GetLeftStick().GetMagnitude();
	if (leftStickMagnitude > 0.f) {
		m_thrustFraction = leftStickMagnitude;
		Vec2 fowardNormal = GetForwardNormal();
		m_velocityOffsetController = fowardNormal * PLAYER_SHIP_ACCELERATION * m_thrustFraction * deltaSeconds;
		m_orientationDegrees = controller.GetLeftStick().GetOrientationDegrees();


		if (m_SFXPlayershipThrustPlayIndex == -1) {
			m_SFXPlayershipThrustPlayIndex = g_theAudio->StartSound(m_SFXPlayershipThrust, true, m_game->GetGlobalVolume() * m_thrustFraction * 0.3f, 0.f, m_SFXSpeedMultiplier * m_thrustFraction * 0.5f, m_isPaused);
		}
		else {
			g_theAudio->SetSoundPlaybackVolume(m_SFXPlayershipThrustPlayIndex, m_game->GetGlobalVolume() * m_thrustFraction * 0.3f);
			g_theAudio->SetSoundPlaybackSpeed(m_SFXPlayershipThrustPlayIndex, m_SFXSpeedMultiplier * m_thrustFraction * 0.5f);
			g_theAudio->SetSoundPlaybackPause(m_SFXPlayershipThrustPlayIndex, false);
		}



	}else {
		//m_thrustFraction = 0.f;
		if (m_SFXPlayershipThrustPlayIndex != -1 && !m_controlledByKeyboard) {
			g_theAudio->SetSoundPlaybackPause(m_SFXPlayershipThrustPlayIndex, true);

		}
	}

	//shoot
	if (controller.WasButtonJustPressed(XBOX_BUTTON_A)) {
		HandleFireBulletSFXs(0);
		SpawnBullet(m_orientationDegrees);
		m_timeWhenHoldingFireButtonNormal = static_cast<float>(GetCurrentTimeSeconds());

	}
	else if (controller.IsButtonDown(XBOX_BUTTON_A) && static_cast<float>(GetCurrentTimeSeconds()) - m_timeWhenHoldingFireButtonNormal > TIME_BEFORE_START_RAPIDFIRE && m_currentTemperature < PLAYER_SHIP_MAX_TEMPERATURE * MAX_RAPIDFIRE_ALLOWED_TEMP_PERCENT) {//if holding button for a period of time

		float currentProjectileDeviation = m_currentTemperature * (1 / PLAYER_SHIP_MAX_TEMPERATURE) * SHIP_MAX_PROJECTILE_DEVIATION_DEG;


		HandleFireBullets(0, currentProjectileDeviation, deltaSeconds);
	}
	else if (controller.IsButtonDown(XBOX_BUTTON_X) && m_currentTemperature < PLAYER_SHIP_MAX_TEMPERATURE * MAX_STARTBURST_ALLOWED_TEMP_PERCENT) {
		HandleFireBulletSFXs(2);
		HandleFireBullets(2, 0.f, deltaSeconds);
		m_timeWhenHoldingFireButtonWide = static_cast<float>(GetCurrentTimeSeconds());

	}
	else {//if not holding button
		//decreasing faster if temperature is high
		//float tempPercent = m_currentTemperature / PLAYER_SHIP_MAX_TEMPERATURE;
		//m_currentTemperature -= BULLET_TEMPERATURE_DECAY_RATE * tempPercent;

		m_currentTemperature -= BULLET_TEMPERATURE_DECAY_RATE;
		m_currentTemperature = GetClamped(m_currentTemperature, 0.f, PLAYER_SHIP_MAX_TEMPERATURE);
	}
}

void PlayerShip::Respawn()
{
	m_isDead = false;
	m_position.x = WORLD_CENTER_X;
	m_position.y = WORLD_CENTER_Y;
	m_orientationDegrees = 0.f;
	m_velocity = Vec2();

	m_velocityOffsetKeyboard = Vec2();
	m_velocityOffsetController = Vec2();

	m_thrustFlameLength = -4.f;
	m_thrustFlameColor = Rgba8(255, 0, 0, 255);

	m_SFXSpeedMultiplier = 1.f;
	m_isPaused = false;
	m_currentTemperature = 0.f;

	m_currentDeltasecondsPassedNormal = 0.f;
	m_currentDeltasecondsPassedLong = 0.f;
	m_currentDeltasecondsPassedWide = 0.f;

	m_controlledByKeyboard = false;

	m_isInvincible = true;
	m_timeWhenActivateInvincibility = static_cast<float>(GetCurrentTimeSeconds());
}

void PlayerShip::SpawnBullet(float orientationDeg)
{
	Vec2 bulletPoint = m_bulletSpawnPoint.GetRotatedDegrees(orientationDeg);
	m_game->SpawnBullet(m_position + bulletPoint, orientationDeg);
}

void PlayerShip::HandleFireBulletSFXs(int weaponType)
{

	RandomNumberGenerator randomGen;
	float pitchVariation = randomGen.RollRandomFloatInRange(0.9f, 1.1f);

	float globalVolume = m_game->GetGlobalVolume();

	switch (weaponType) {
	case 0:
		m_SFXPlayershipFireBulletShortPlayIndex = g_theAudio->StartSound(m_SFXPlayershipFireBulletShort,false, globalVolume,0.f,m_SFXSpeedMultiplier * pitchVariation, m_isPaused);
		break;
	case 1://long
		m_SFXPlayershipFireBulletLongPlayIndex = g_theAudio->StartSound(m_SFXPlayershipFireBulletLong, false, globalVolume, 0.f, m_SFXSpeedMultiplier * pitchVariation, m_isPaused);
		break;
	case 2://wide
		m_SFXPlayershipFireBulletWidePlayIndex = g_theAudio->StartSound(m_SFXPlayershipFireBulletWide, false, globalVolume, 0.f, m_SFXSpeedMultiplier * pitchVariation, m_isPaused);
		break;
	default:
		break;
	}
	
	

}

void PlayerShip::HandleFireBullets(int weaponType, float projectileDeviation, float deltaSeconds)
{
	UNUSED(deltaSeconds);
	RandomNumberGenerator randGen;
	if (weaponType == 0) {//rapidfire normal
		
		m_currentDeltasecondsPassedNormal += 1;
		if (m_currentDeltasecondsPassedNormal < SHIP_DELTASECOND_BEFORE_NEXT_BULLET_NORMAL)return;

		float orientationDeg = randGen.RollRandomFloatInRange(m_orientationDegrees - projectileDeviation, m_orientationDegrees + projectileDeviation);

		SpawnBullet(orientationDeg);
		m_currentTemperature += BULLET_NORMAL_TEMPERATURE;
		m_currentTemperature = GetClamped(m_currentTemperature, 0.f, PLAYER_SHIP_MAX_TEMPERATURE);
		
		m_currentDeltasecondsPassedNormal = 0.f;
	}
	else if (weaponType == 1) {//laser - todo



		m_currentTemperature += BULLET_LONG_TEMPERATURE;
		m_currentTemperature = GetClamped(m_currentTemperature, 0.f, PLAYER_SHIP_MAX_TEMPERATURE);

	}
	else if (weaponType == 2) {//starburst
		float orientationDeg = 0.f;
		for (int i = 0; i < NUM_BULLETS_PER_STARBURST;i++) {

			orientationDeg = static_cast<float>(i * (360 / NUM_BULLETS_PER_STARBURST));

			SpawnBullet(orientationDeg);
		}

		m_currentTemperature += BULLET_WIDE_TEMPERATURE;
		m_currentTemperature = GetClamped(m_currentTemperature, 0.f, PLAYER_SHIP_MAX_TEMPERATURE);
	}






}

void PlayerShip::StopThrustingSound()
{
	//stop thrusting sound
	if (m_SFXPlayershipThrustPlayIndex != -1) {
		g_theAudio->StopSound(m_SFXPlayershipThrustPlayIndex);
		m_SFXPlayershipThrustPlayIndex = static_cast<SoundPlaybackID>(-1);
	}
}

void PlayerShip::SetSFXSpeedMultiplier(float multiplier)
{
	m_SFXSpeedMultiplier = multiplier;
}

void PlayerShip::SetRotationSpeedMultiplier(float multiplier)
{
	m_rotationSpeedMultiplier = multiplier;
}

void PlayerShip::SetSFXPause(bool isPaused)
{
	m_isPaused = isPaused;
}

void PlayerShip::SetInvincible(bool invincible)
{
	m_isInvincible = invincible;
}

