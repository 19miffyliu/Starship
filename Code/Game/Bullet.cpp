#include "Bullet.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"

Bullet::Bullet()
{
}

Bullet::Bullet(Game* g, Vec2 pos, float orientationDeg) : Entity(g, pos, orientationDeg)
{
	m_physicsRadius = BULLET_PHYSICS_RADIUS;
	m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
	InitializeLocalVerts();

	m_velocity = GetForwardNormal() * BULLET_SPEED;
}

void Bullet::Update(float deltaSeconds)
{
	m_position += m_velocity * deltaSeconds;
}

void Bullet::Render() const
{
	if (m_isDead)return;

	Vertex_PCU tempBulletWorldVerts[NUM_BULLETS_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_BULLETS_VERTS; ++vertIndex) {
		tempBulletWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_BULLETS_VERTS, tempBulletWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_BULLETS_VERTS, tempBulletWorldVerts);
}
/*
void Bullet::DebugRender() const
{
}
*/
bool Bullet::IsOffscreen() const
{
	bool overX = m_position.x + BULLET_COSMETIC_RADIUS < 0.f || m_position.x - BULLET_COSMETIC_RADIUS > WORLD_SIZE_X;
	bool overY = m_position.y + BULLET_COSMETIC_RADIUS < 0.f || m_position.y - BULLET_COSMETIC_RADIUS > WORLD_SIZE_Y;

	return overX || overY;
}

//initialize the bullet verts based on relative position
void Bullet::InitializeLocalVerts()
{
	Vec2 texCoord = Vec2(0.f, 0.f);
	Rgba8 noseColor = Rgba8(255,255,0, 255);
	Rgba8 fireColor = Rgba8(255, 0, 0, 255);
	Rgba8 tailColor = Rgba8(255, 0, 0, 0);

	/*
	//relative
	//triangle A
	m_localVerts[0] = Vertex_PCU(Vec3(m_position.x + 0.f, m_position.y + 5.f, 0.f), noseColor, texCoord);
	m_localVerts[1] = Vertex_PCU(Vec3(m_position.x + 0.f, m_position.y - 5.f, 0.f), noseColor, texCoord);
	m_localVerts[2] = Vertex_PCU(Vec3(m_position.x + 5.f, m_position.y + 0.f, 0.f), noseColor, texCoord);


	//triangle B
	m_localVerts[3] = Vertex_PCU(Vec3(m_position.x + 0.f, m_position.y + 5.f, 0.f), fireColor, texCoord);
	m_localVerts[4] = Vertex_PCU(Vec3(m_position.x - 2.f, m_position.y + 0.f, 0.f), tailColor, texCoord);
	m_localVerts[5] = Vertex_PCU(Vec3(m_position.x + 0.f, m_position.y - 5.f, 0.f), fireColor, texCoord);
*/

	//local
	//triangle A
	m_localVerts[0] = Vertex_PCU(Vec3(0.f,0.5f, 0.f), noseColor, texCoord);
	m_localVerts[1] = Vertex_PCU(Vec3( 0.f,-0.5f, 0.f), noseColor, texCoord);
	m_localVerts[2] = Vertex_PCU(Vec3(0.5f, 0.f, 0.f), noseColor, texCoord);


	//triangle B
	m_localVerts[3] = Vertex_PCU(Vec3(0.f,  0.5f, 0.f), fireColor, texCoord);
	m_localVerts[4] = Vertex_PCU(Vec3(-2.f,  0.f, 0.f), tailColor, texCoord);
	m_localVerts[5] = Vertex_PCU(Vec3(0.f, -0.5f, 0.f), fireColor, texCoord);

}
