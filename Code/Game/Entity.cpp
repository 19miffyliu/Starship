#include "Entity.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"



Entity::Entity()
{
}

Entity::Entity(Game* g, Vec2 pos, float orientationDeg)
{
	m_game = g;
	m_position = pos;
	m_orientationDegrees = orientationDeg;
}

Entity::~Entity()
{
	m_game = nullptr;
}




void Entity::RenderHealthBar(float maxHealth, float cosmeticRadius) const
{
	//render health bar as one debugRect and one debugBox
	constexpr float healthBarLength = SCREEN_SIZE_X * 0.001f;
	constexpr float healthBarWidth = SCREEN_SIZE_Y * 0.0005f;

	//Vec2 transformForAll(SCREEN_SIZE_X * 0.5f, SCREEN_SIZE_Y * 0.95f);
	Vec2 transformForAll(m_position.x, m_position.y + cosmeticRadius);
	constexpr float scaleForAll = SCREEN_SIZE_X * 0.0005f;

	
	float healthPercent = m_health * (1.f / maxHealth);

	Vec2 bottomLeft(-healthBarLength, -healthBarWidth);
	Vec2 topRight(healthBarLength, healthBarWidth);
	Vec2 topRightBar(healthBarLength * (healthPercent * 2.f - 1.f), healthBarWidth);

	TransformPosition2D(bottomLeft, scaleForAll, 0.f, transformForAll);
	TransformPosition2D(topRight, scaleForAll, 0.f, transformForAll);
	TransformPosition2D(topRightBar, scaleForAll, 0.f, transformForAll);

	Rgba8 barColor(255, 0, 0, 255);

	DebugDrawRectangle(bottomLeft, topRightBar, barColor);

	//draw the containing box
	constexpr float boxThickness = healthBarWidth * 0.2f;
	Rgba8 boxColor(255, 255, 255, 255);

	DebugDrawBox(bottomLeft, topRight, boxColor, boxThickness);


}

void Entity::DebugRender() const
{

	float thickness = DEBUG_THICKNESS;
	//foward vector
	Vec2 fwdVec = m_position + GetForwardNormal() * m_cosmeticRadius;
	DebugDrawLine(m_position, fwdVec, Rgba8(255, 0, 0), thickness);

	//relative left vector
	Vec2 leftVec = m_position + GetForwardNormal().GetRotated90Degrees() * m_cosmeticRadius;
	DebugDrawLine(m_position, leftVec, Rgba8(0, 255, 0), thickness);

	//cosmetic radius
	DebugDrawRing(m_position, m_cosmeticRadius, thickness, Rgba8(255, 0, 255));

	//physics radius
	DebugDrawRing(m_position, m_physicsRadius, thickness, Rgba8(0, 255, 255));


	//current velocity
	DebugDrawLine(m_position, m_position + m_velocity, Rgba8(255, 255, 0), thickness);
}

void Entity::Die()
{
	m_isDead = true;
}

void Entity::BecomesGarbage()
{
	m_isGarbage = true;
}

void Entity::SetHealth(int h)
{
	m_health = h;
	if (m_health == 0) {
		Die();
	}
}

void Entity::DecreaseHealthByOne()
{
	m_health--;
	if (m_health == 0) {
		Die();
		BecomesGarbage();
	}
	m_timeWhenGotHurt = static_cast<float>(GetCurrentTimeSeconds());


}

void Entity::SetPosition(Vec2 const& pos)
{
	m_position = pos;
}

void Entity::SetOrientationDeg(float deg)
{
	m_orientationDegrees = deg;
}



//return true if the center point is out of the screen
bool Entity::IsOffscreen()const
{
	return (m_position.x > WORLD_SIZE_X || m_position.y > WORLD_SIZE_Y);
}

Vec2 Entity::GetForwardNormal()const
{
	return Vec2::MakeFromPolarDegrees(m_orientationDegrees);
}

Vec2 Entity::GetVelocity() const
{
	return m_velocity;
}

Rgba8 Entity::GetColor() const
{
	return m_color;
}

bool Entity::IsAlive()const
{
	return !m_isDead;
}

bool Entity::IsGarbage() const
{
	return m_isGarbage;
}

Vec2 Entity::GetPosition()const
{
	return m_position;
}

float Entity::GetHealth()const
{
	return static_cast<float>(m_health);
}
