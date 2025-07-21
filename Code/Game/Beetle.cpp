#include "Game/Beetle.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Time.hpp"


Beetle::Beetle()
{
}

Beetle::Beetle(Game* g, Vec2 pos, float orientationDeg) : Entity(g, pos, orientationDeg)
{
	m_health = BEETLE_MAX_HEALTH;
	m_physicsRadius = BEETLE_PHYSICS_RADIUS;
	m_cosmeticRadius = BEETLE_COSMETIC_RADIUS;
	InitializeLocalVerts();

	//m_velocity = GetForwardNormal() * BEETLE_SPEED;
}

void Beetle::Update(float deltaSeconds)
{
	if (m_game->GetShipAlive()) {
		Vec2 dispVector = m_game->GetShipPos() - m_position;
		m_orientationDegrees = Atan2Degrees(dispVector.y, dispVector.x);
		m_velocity = GetForwardNormal() * BEETLE_SPEED;
	}
	m_position += m_velocity * deltaSeconds;

	// if got hurt, flash red for 0.3s
	Rgba8 currentCol = m_beetleColor;
	float timeSinceGotHurt = static_cast<float>(GetCurrentTimeSeconds()) - m_timeWhenGotHurt;
	if (m_timeWhenGotHurt!=0.f && timeSinceGotHurt < 0.3f) {
		currentCol = Rgba8(255, 0, 0);
		
	}
	for (int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; ++vertIndex) {
		m_localVerts[vertIndex].m_color = currentCol;
	}
}

void Beetle::Render() const
{
	if (m_isDead)return;

	Vertex_PCU tempBulletWorldVerts[NUM_BEETLE_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; ++vertIndex) {
		tempBulletWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}
	

	TransformVertexArrayXY3D(NUM_BEETLE_VERTS, tempBulletWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_BEETLE_VERTS, tempBulletWorldVerts);

	RenderHealthBar(BEETLE_MAX_HEALTH, BEETLE_COSMETIC_RADIUS);
}

bool Beetle::IsOffscreen() const
{
	bool overX = m_position.x + BEETLE_COSMETIC_RADIUS < 0.f || m_position.x - BEETLE_COSMETIC_RADIUS > WORLD_SIZE_X;
	bool overY = m_position.y + BEETLE_COSMETIC_RADIUS < 0.f || m_position.y - BEETLE_COSMETIC_RADIUS > WORLD_SIZE_Y;

	return overX || overY;
}

void Beetle::InitializeLocalVerts()
{
	//initialize the coords to green
	Vec2 texCoord = Vec2(0.f, 0.f);
	
	m_color = m_beetleColor;

	//local
	//triangle A
	m_localVerts[0] = Vertex_PCU(Vec3(-1.28558f, 1.53208f, 0.f), m_color, texCoord);
	m_localVerts[1] = Vertex_PCU(Vec3(-1.28558f, -1.53208f, 0.f), m_color, texCoord);
	m_localVerts[2] = Vertex_PCU(Vec3(1.74f, 0.9861f, 0.f), m_color, texCoord);


	//triangle B
	m_localVerts[3] = Vertex_PCU(Vec3(-1.28558f, -1.53208f, 0.f), m_color, texCoord);
	m_localVerts[4] = Vertex_PCU(Vec3(1.74f, -0.9861f, 0.f), m_color, texCoord);
	m_localVerts[5] = Vertex_PCU(Vec3(1.74f, 0.9861f, 0.f), m_color, texCoord);
}
