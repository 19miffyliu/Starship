#include "Game/Asteroid.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"

Asteroid::Asteroid()
{
}

Asteroid::Asteroid(Game* g, Vec2 pos, float orientationDeg) : Entity(g, pos, orientationDeg)
{

	m_health = ASTEROID_MAX_HEALTH;
	m_physicsRadius = ASTEROID_PHYSICS_RADIUS;
	m_cosmeticRadius = ASTEROID_COSMETIC_RADIUS;
	InitializeLocalVerts();

	//m_velocity = GetForwardNormal() * ASTEROID_SPEED;

	//randomize velocity by changing the forward direction
	RandomNumberGenerator randomGen;
	float randomVeloDirection = randomGen.RollRandomFloatInRange(0, 360);
	m_velocity = Vec2::MakeFromPolarDegrees(randomVeloDirection) * ASTEROID_SPEED;

	m_angularVelocity = randomGen.RollRandomFloatInRange(-200,200);

}

void Asteroid::Update(float deltaSeconds)
{
	m_position += m_velocity * deltaSeconds;
	m_orientationDegrees += m_angularVelocity * deltaSeconds;
	
}

void Asteroid::Render() const
{
	if (m_isDead)return;

	Vertex_PCU tempAsteroidWorldVerts[NUM_ASTEROIDS_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_ASTEROIDS_VERTS; ++vertIndex) {
		tempAsteroidWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_ASTEROIDS_VERTS, tempAsteroidWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_ASTEROIDS_VERTS, tempAsteroidWorldVerts);
}
void Asteroid::WrapAroundWorld()
{
	//if x is over edge, wrap around edge
	if (m_position.x + ASTEROID_COSMETIC_RADIUS < 0.f) {
		m_position.x = WORLD_SIZE_X + ASTEROID_COSMETIC_RADIUS;
	}
	else if (m_position.x - ASTEROID_COSMETIC_RADIUS > WORLD_SIZE_X) {
		m_position.x = 0.f-ASTEROID_COSMETIC_RADIUS;
	}

	//if y is over edge, wrap around edge
	if (m_position.y + ASTEROID_COSMETIC_RADIUS < 0.f) {
		m_position.y = WORLD_SIZE_Y + ASTEROID_COSMETIC_RADIUS;
	}
	else if (m_position.y - ASTEROID_COSMETIC_RADIUS > WORLD_SIZE_Y) {
		m_position.y = 0.f - ASTEROID_COSMETIC_RADIUS;
	}
}
/*
void Asteroid::DebugRender() const
{
}
*/
//return true if the asteroid's cosmetic radius is offscreen
bool Asteroid::IsOffscreen() const
{
	bool overX = m_position.x + ASTEROID_COSMETIC_RADIUS < 0.f || m_position.x - ASTEROID_COSMETIC_RADIUS > WORLD_SIZE_X;
	bool overY = m_position.y + ASTEROID_COSMETIC_RADIUS < 0.f || m_position.y - ASTEROID_COSMETIC_RADIUS > WORLD_SIZE_Y;

	return overX || overY;
}

void Asteroid::InitializeLocalVerts()
{

	Vec2 texCoord = Vec2(0.f, 0.f);
	Rgba8 asteroidColor = Rgba8(100,100,100, 255);
	m_color = asteroidColor;

	constexpr float DEGREES_PER_SIDE = 360.f / static_cast<float>(NUM_ASTEROIDS_TRIANGLES);

	RandomNumberGenerator randomGen;
	float currentRadius;
	float nextRadius = randomGen.RollRandomFloatInRange(ASTEROID_PHYSICS_RADIUS, ASTEROID_COSMETIC_RADIUS);

	float initialRadius = nextRadius;

	for (int triNum = 0; triNum < NUM_ASTEROIDS_TRIANGLES; ++triNum) {
		float startDeg = DEGREES_PER_SIDE * static_cast<float>(triNum);
		float endDeg = DEGREES_PER_SIDE * static_cast<float>(triNum + 1);
		float cosStart = CosDegrees(startDeg);
		float sinStart = SinDegrees(startDeg);
		float cosEnd = CosDegrees(endDeg);
		float sinEnd = SinDegrees(endDeg);

		//randomize the radius of the asteroid to value in range (physic radius, cosmetic radius) inclusive
		currentRadius = nextRadius;
		nextRadius = randomGen.RollRandomFloatInRange(ASTEROID_PHYSICS_RADIUS, ASTEROID_COSMETIC_RADIUS);
		if (triNum == NUM_ASTEROIDS_TRIANGLES-1)nextRadius = initialRadius;//set the final triangle's end radius the same as the first radius

		//trangle in index [triNum] of 16 triangles
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
		m_localVerts[triNum * 3] = Vertex_PCU(Vec3(0.f, 0.f, 0.f), asteroidColor, texCoord);

		//former point
		m_localVerts[triNum * 3 + 1] = Vertex_PCU(Vec3(currentRadius * cosStart, currentRadius * sinStart, 0.f), asteroidColor, texCoord);

		//next point
		m_localVerts[triNum * 3 + 2] = Vertex_PCU(Vec3(nextRadius * cosEnd, nextRadius * sinEnd, 0.f), asteroidColor, texCoord);


	}



}

