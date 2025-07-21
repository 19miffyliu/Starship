#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Rgba8.hpp"


//extern variables
class Renderer;
extern Renderer* g_theRenderer;
class App;
extern App* g_theApp;
class AudioSystem;
extern AudioSystem* g_theAudio;
class Window;
extern Window* g_theWindow;


//screen camera
constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 800.f;

//world size
constexpr float WORLD_SIZE_X = 1000.f;
constexpr float WORLD_SIZE_Y = 500.f;

//world camera size
constexpr float WORLD_CAM_SIZE_X = 200.f;
constexpr float WORLD_CAM_SIZE_Y = 100.f;

constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;


//asteroids
constexpr int MAX_ASTEROIDS = 100;
constexpr float ASTEROID_SPEED = 10.f;
constexpr float ASTEROID_PHYSICS_RADIUS = 1.6f;
constexpr float ASTEROID_COSMETIC_RADIUS = 2.0f;
constexpr float MAX_ASTEROID_SCATTER_SPEED = 20.f;
constexpr float ASTEROID_HIT_PITCH = 0.5f;
constexpr float ASTEROID_HIT_VOLUME = 0.4f;
constexpr int ASTEROID_MAX_HEALTH = 3;


//bullets
constexpr int MAX_BULLETS = 200;
constexpr float BULLET_LIFETIME_SECONDS = 2.0f;
constexpr float BULLET_SPEED = 50.f;
constexpr float BULLET_PHYSICS_RADIUS = 0.5f;
constexpr float BULLET_COSMETIC_RADIUS = 2.0f;

//beetle
constexpr float BEETLE_SPEED = 11.f;
constexpr float BEETLE_PHYSICS_RADIUS = 1.4f;
constexpr float BEETLE_COSMETIC_RADIUS = 2.0f;
constexpr int BEETLE_MAX_HEALTH = 3;
constexpr int MAX_BEETLES = 50;
constexpr float MAX_BEETLE_SCATTER_SPEED = 30.f;
constexpr float BEETLE_HIT_PITCH = 0.7f;


//wasp
constexpr float WASP_ACCELERATION = 10.f;
constexpr float WASP_MAX_SPEED = 50.f;
constexpr float WASP_PHYSICS_RADIUS = 1.6f;
constexpr float WASP_COSMETIC_RADIUS = 2.0f;
constexpr int WASP_MAX_HEALTH = 3;
constexpr int MAX_WASPS = 30;
constexpr float MAX_WASP_SCATTER_SPEED = 50.f;
constexpr float WASP_HIT_PITCH = 1.0f;


//debris
constexpr int MAX_DEBRIS = 300;
constexpr float MAX_DEBRIS_SCATTER_SPEED = 20.f;
constexpr float DEBRIS_PHYSICS_RADIUS = 1.0f;
constexpr float DEBRIS_COSMETIC_RADIUS = 1.5f;

//player ship
constexpr float PLAYER_SHIP_ACCELERATION = 30.f;
constexpr float PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float PLAYER_SHIP_PHYSICS_RADIUS = 1.75f;
constexpr float PLAYER_SHIP_COSMETIC_RADIUS = 2.25f;
constexpr int PLAYER_SHIP_MAX_HEALTH = 4;
constexpr float PLAYER_SHIP_MAX_TEMPERATURE = 100.f;

constexpr float PLAYER_SHIP_DEATH_TRAUMA = 0.8f;
constexpr float GAMEOVER_TRAUMA = 1.f;
constexpr float MAX_CAM_SHAKE_AMT = WORLD_CAM_SIZE_X / 30.f;

//waves
constexpr int MAX_WAVES = 5;
constexpr float WAVE_MULTIPLIER = 1.5f;
constexpr int INITIAL_ASTEROID_NUM = 5;
constexpr int INITIAL_BEETLE_NUM = 3;
constexpr int INITIAL_WASP_NUM = 2;

//debug drawing
constexpr float DEBUG_THICKNESS = 0.2f;

//starry background variables
constexpr int NUM_STARS_IN_BG = static_cast<int>(WORLD_SIZE_X * 0.3f);
constexpr int NUM_STAR_VERTEX = 3;
constexpr int TOTAL_STAR_VERTEX = NUM_STAR_VERTEX * NUM_STARS_IN_BG;
constexpr float STAR_SCALE = 0.3f;


//drawing dot variables
constexpr float sqrt3by2 = 0.86602540378f;

void DebugDrawRing(Vec2 const& center, float radius, float thickness, Rgba8 const& color);
void DebugDrawLine(Vec2 const& start, Vec2 const& end, Rgba8 color, float thickness);
void DebugDrawDot(Vec2 const& center, float radius, Rgba8 color);//draw a triangle
void DebugDrawRectangle(Vec2 const& bottomLeft, Vec2 const& topRight, Rgba8 color);
void DebugDrawBox(Vec2 const& bottomLeft, Vec2 const& topRight, Rgba8 color, float thickness);
