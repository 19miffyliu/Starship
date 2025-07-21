#include "Wasp.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Time.hpp"


Wasp::Wasp()
{
}

Wasp::Wasp(Game* g, Vec2 pos, float orientationDeg) : Entity(g, pos, orientationDeg)
{
	m_health = WASP_MAX_HEALTH;
	m_physicsRadius = WASP_PHYSICS_RADIUS;
	m_cosmeticRadius = WASP_COSMETIC_RADIUS;
	InitializeLocalVerts();

}

void Wasp::Update(float deltaSeconds)
{
	Vec2 dispVector = m_game->GetShipPos() - m_position;
	m_orientationDegrees = Atan2Degrees(dispVector.y, dispVector.x);
	Vec2 fowardNormal = GetForwardNormal();
	m_velocity += fowardNormal * PLAYER_SHIP_ACCELERATION * deltaSeconds;
	m_velocity.ClampLength(WASP_MAX_SPEED);
	m_position += m_velocity * deltaSeconds;

	// if got hurt, flash red for 0.3s
	Rgba8 currentCol = m_waspColor;
	float timeSinceGotHurt = static_cast<float>(GetCurrentTimeSeconds()) - m_timeWhenGotHurt;
	if (m_timeWhenGotHurt != 0.f && timeSinceGotHurt < 0.3f) {
		currentCol = Rgba8(255, 0, 0);

	}
	for (int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; ++vertIndex) {
		m_localVerts[vertIndex].m_color = currentCol;
	}
}

void Wasp::Render() const
{
	if (m_isDead)return;

	Vertex_PCU tempBulletWorldVerts[NUM_WASP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_WASP_VERTS; ++vertIndex) {
		tempBulletWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_WASP_VERTS, tempBulletWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_WASP_VERTS, tempBulletWorldVerts);

	RenderHealthBar(WASP_MAX_HEALTH, WASP_COSMETIC_RADIUS);
}

bool Wasp::IsOffscreen() const
{
	return false;
}

void Wasp::InitializeLocalVerts()
{
	//initialize the coords to yellow
	Vec2 texCoord = Vec2(0.f, 0.f);
	
	m_color = m_waspColor;

	//local
	//triangle head
	m_localVerts[0] = Vertex_PCU(Vec3(0.f,2.f, 0.f), m_color, texCoord);
	m_localVerts[1] = Vertex_PCU(Vec3(0.f,-2.f, 0.f), m_color, texCoord);
	m_localVerts[2] = Vertex_PCU(Vec3(2.f,0.f, 0.f), m_color, texCoord);


	//triangle tail
	m_localVerts[3] = Vertex_PCU(Vec3(0.f,1.f, 0.f), m_color, texCoord);
	m_localVerts[4] = Vertex_PCU(Vec3(-2.f,0.f, 0.f), m_color, texCoord);
	m_localVerts[5] = Vertex_PCU(Vec3(0.f,-1.f, 0.f), m_color, texCoord);
}
