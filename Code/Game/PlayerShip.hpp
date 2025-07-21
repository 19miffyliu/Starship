#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Audio/AudioSystem.hpp"


constexpr int NUM_SHIP_TRIANGLES = 6;
constexpr int NUM_SHIP_VERTS = 3 * NUM_SHIP_TRIANGLES;

//weapon type related
constexpr int SHIP_DELTASECOND_BEFORE_NEXT_BULLET_NORMAL = 180;
constexpr int SHIP_DELTASECOND_BEFORE_NEXT_BULLET_LONG = 240;//laser
constexpr int SHIP_DELTASECOND_BEFORE_NEXT_BULLET_WIDE = 360;//shotgun

constexpr float TIME_BEFORE_START_RAPIDFIRE = 0.5f;

constexpr float BULLET_NORMAL_TEMPERATURE = 3.f;
constexpr float BULLET_LONG_TEMPERATURE = 30.f;
constexpr float BULLET_WIDE_TEMPERATURE = 50.f;

constexpr float BULLET_TEMPERATURE_DECAY_RATE = 0.003f;

constexpr float SHIP_MAX_PROJECTILE_DEVIATION_DEG = 30.f;
constexpr float MAX_RAPIDFIRE_ALLOWED_TEMP_PERCENT = 0.98f;

constexpr int NUM_BULLETS_PER_STARBURST = 16;
constexpr float MAX_STARTBURST_ALLOWED_TEMP_PERCENT = 0.6f;



class PlayerShip : public Entity
{
public:
	PlayerShip(Game* owner, Vec2 pos, float orientationDeg);
	void Update(float deltaSeconds) override;
	void Render() const override;
	void UpdateThrustFlame();

	//get function
	bool IsOffscreen()const override;
	Vertex_PCU* GetLocalVerts()const;
	Vertex_PCU GetVertByIndex(int index)const;
	int GetVertexNum()const;
	float GetCurrentTemp()const;
	bool IsInvincible()const;

	//set functions
	void SetSFXSpeedMultiplier(float multiplier);
	void SetRotationSpeedMultiplier(float multiplier);
	void SetSFXPause(bool isPaused);
	void SetInvincible(bool invincible);

	//util functions
	void Respawn();
	void SpawnBullet(float orientationDeg);

	void HandleFireBulletSFXs(int weaponType);
	void HandleFireBullets(int weaponType, float projectileDeviation, float deltaSeconds);
	void StopThrustingSound();

private:
	void InitializeLocalVerts();
	void UpdateFromKeyboard(float deltaSeconds);
	void BounceOffWalls();

	void UpdateFromController(float deltaSeconds);
	

private:
	Vec2 m_velocityOffsetKeyboard;//velocity change from keyboard
	Vec2 m_velocityOffsetController;//velocity change from controller
	float m_thrustFraction;

	Vertex_PCU m_localVerts[NUM_SHIP_VERTS];
	Rgba8 m_shipColor = Rgba8(102, 153, 204, 255);
	Vec2 m_bulletSpawnPoint;
	float m_thrustFlameLength = -4.f;
	Rgba8 m_thrustFlameColor = Rgba8(255,0,0,255);


	//SFXs
	SoundID m_SFXPlayershipFireBulletShort = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXPlayershipFireBulletShortPlayIndex = static_cast<SoundPlaybackID>(-1);
	SoundID m_SFXPlayershipFireBulletLong = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXPlayershipFireBulletLongPlayIndex = static_cast<SoundPlaybackID>(-1);
	SoundID m_SFXPlayershipFireBulletWide = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXPlayershipFireBulletWidePlayIndex = static_cast<SoundPlaybackID>(-1);

	SoundID m_SFXPlayershipThrust = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXPlayershipThrustPlayIndex = static_cast<SoundPlaybackID>(-1);

	SoundID m_SFXPlayershipRespawn = static_cast<SoundID>(-1);
	SoundPlaybackID m_SFXPlayershipRespawnPlayIndex = static_cast<SoundPlaybackID>(-1);

	//speed & time related
	float m_SFXSpeedMultiplier = 1.f;
	float m_rotationSpeedMultiplier = 1.f;
	bool m_isPaused = false;


	//weapon related
	float m_currentTemperature = 0.f;


	float m_timeWhenHoldingFireButtonNormal = 0.f;
	float m_timeWhenHoldingFireButtonLong = 0.f;
	float m_timeWhenHoldingFireButtonWide = 0.f;

	float m_currentDeltasecondsPassedNormal = 0.f;
	float m_currentDeltasecondsPassedLong = 0.f;
	float m_currentDeltasecondsPassedWide = 0.f;

	//control related
	bool m_controlledByKeyboard = false;

	//invincibility related
	bool m_isInvincible = false;

	float m_timeWhenActivateInvincibility = 0.f;


};

