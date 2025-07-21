#include "Debris.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"

Debris::Debris()
{
}

Debris::Debris(Game* g, Vec2 pos, float orientationDeg) : Entity(g, pos, orientationDeg)
{

	m_health = 3;
	m_physicsRadius = DEBRIS_PHYSICS_RADIUS;
	m_cosmeticRadius = DEBRIS_COSMETIC_RADIUS;
	InitializeLocalVerts();


	RandomNumberGenerator randomGen;
	float randomVeloDirection = randomGen.RollRandomFloatInRange(0, 360);
	m_velocity = Vec2::MakeFromPolarDegrees(randomVeloDirection) * randomGen.RollRandomFloatInRange(0.f, MAX_DEBRIS_SCATTER_SPEED);

	m_angularVelocity = randomGen.RollRandomFloatInRange(-400, 400);

}

Debris::Debris(Game* g, Vec2 pos, float orientationDeg, Vec2 const& velocity, float avgRadius, Rgba8 color) : Entity(g, pos, orientationDeg)
{
	m_health = 3;
	//m_physicsRadius = DEBRIS_PHYSICS_RADIUS;
	//m_cosmeticRadius = DEBRIS_COSMETIC_RADIUS;

	/*
	m_physicsRadius = GetClamped(radius * 0.3f, 0.05f, radius * 0.3f);
	m_cosmeticRadius = GetClamped(radius * 1.5f, 0.1f , radius * 1.5f);
	*/
	RandomNumberGenerator randGen;
	m_physicsRadius = randGen.RollRandomFloatInRange(avgRadius * 0.3f, avgRadius * 0.7f);
	m_cosmeticRadius = randGen.RollRandomFloatInRange(avgRadius, avgRadius* 1.2f);

	m_color = color;
	InitializeLocalVerts();

	m_velocity = velocity;

	RandomNumberGenerator randomGen;
	m_angularVelocity = randomGen.RollRandomFloatInRange(-400, 400);

}

void Debris::Update(float deltaSeconds)
{
	m_position += m_velocity * deltaSeconds;
	m_orientationDegrees += m_angularVelocity * deltaSeconds;
	m_ageInSeconds += deltaSeconds;
	m_color.a = static_cast<unsigned char>(RangeMapClamped(m_ageInSeconds, 0.f, 2.f, 127.f, 0.f));
	ReRenderColors(m_color);
	if (m_ageInSeconds >= 2.f) {
		Die();
		BecomesGarbage();
	}
}

void Debris::Render() const
{
	if (m_isDead)return;

	Vertex_PCU tempDebrisWorldVerts[NUM_DEBRIS_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_DEBRIS_VERTS; ++vertIndex) {
		tempDebrisWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_DEBRIS_VERTS, tempDebrisWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_DEBRIS_VERTS, tempDebrisWorldVerts);
}

void Debris::ReRenderColors(Rgba8 const& color)
{
	for (Vertex_PCU & vert : m_localVerts)
	{
		vert.m_color = color;
	}
}

bool Debris::IsOffscreen() const
{
	bool overX = m_position.x + DEBRIS_COSMETIC_RADIUS < 0.f || m_position.x - DEBRIS_COSMETIC_RADIUS > WORLD_SIZE_X;
	bool overY = m_position.y + DEBRIS_COSMETIC_RADIUS < 0.f || m_position.y - DEBRIS_COSMETIC_RADIUS > WORLD_SIZE_Y;

	return overX || overY;
}

void Debris::InitializeLocalVerts()
{
	Vec2 texCoord = Vec2(0.f, 0.f);
	Rgba8 debrisColor = m_color;

	constexpr float DEGREES_PER_SIDE = 360.f / static_cast<float>(NUM_DEBRIS_TRIANGLES);

	RandomNumberGenerator randomGen;
	float currentRadius;
	float nextRadius = randomGen.RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius);

	float initialRadius = nextRadius;

	for (int triNum = 0; triNum < NUM_DEBRIS_TRIANGLES; ++triNum) {
		float startDeg = DEGREES_PER_SIDE * static_cast<float>(triNum);
		float endDeg = DEGREES_PER_SIDE * static_cast<float>(triNum + 1);
		float cosStart = CosDegrees(startDeg);
		float sinStart = SinDegrees(startDeg);
		float cosEnd = CosDegrees(endDeg);
		float sinEnd = SinDegrees(endDeg);

		//randomize the radius of the debris to value in range (physic radius, cosmetic radius) inclusive
		currentRadius = nextRadius;
		nextRadius = randomGen.RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius);
		if (triNum == NUM_DEBRIS_TRIANGLES - 1)nextRadius = initialRadius;//set the final triangle's end radius the same as the first radius

		//triangle in index [triNum] of 16 triangles
		/*
		//relative

		//center point
		m_localVerts[triNum*3] = Vertex_PCU(Vec3(m_position.x, m_position.y, 0.f), asteroidColor, texCoord);

		//former point
		m_localVerts[triNum*3+1] = Vertex_PCU(Vec3(m_position.x + currentRadius * cosStart, m_position.y + currentRadius * sinStart, 0.f), asteroidColor, texCoord);

		//next point
		m_localVerts[triNum * 3+2] = Vertex_PCU(Vec3(m_position.x + nextRadius * cosEnd, m_position.y + nextRadius * sinEnd, 0.f), asteroidColor, texCoord);
*/
		//local
		//center point
		m_localVerts[triNum * 3] = Vertex_PCU(Vec3(0.f, 0.f, 0.f), debrisColor, texCoord);

		//former point
		m_localVerts[triNum * 3 + 1] = Vertex_PCU(Vec3(currentRadius * cosStart, currentRadius * sinStart, 0.f), debrisColor, texCoord);

		//next point
		m_localVerts[triNum * 3 + 2] = Vertex_PCU(Vec3(nextRadius * cosEnd, nextRadius * sinEnd, 0.f), debrisColor, texCoord);
	}
}
