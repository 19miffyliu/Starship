#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"

class Game;
struct Vec2;


class Entity
{
public:
	Entity();
	Entity(Game* g, Vec2 pos, float orientationDeg);
	virtual ~Entity();

	virtual void Update(float deltaSeconds) = 0;
	virtual void Render() const = 0;
	virtual void RenderHealthBar(float maxHealth, float cosmeticRadius) const;

	void DebugRender() const;

	//set function
	virtual void Die();
	void BecomesGarbage();
	void SetHealth(int h);
	void DecreaseHealthByOne();
	
	void SetPosition(Vec2 const& pos);
	void SetOrientationDeg(float deg);


	//get function
	virtual bool IsOffscreen() const;
	Vec2 GetForwardNormal() const;
	Vec2 GetVelocity()const;
	Rgba8 GetColor()const;

	bool IsAlive()const;
	bool IsGarbage()const;
	Vec2 GetPosition()const;
	float GetHealth()const;

	
protected:
	Game* m_game = nullptr;

	Vec2 m_position; // in world space
	Vec2 m_velocity; // velocity in world unit/s
	float m_orientationDegrees = 0.f;// forward angle in degrees, counterclockwise from +x
	float m_angularVelocity = 0.f; //signed angular velocity in degees/s
	float m_physicsRadius = 5.f; // inner disc-radius for physics purpose
	float m_cosmeticRadius = 10.f; // outer disc-radius for cosmetic purpose, enclosinig all vertices

	int m_health = 1;
	Rgba8 m_color = Rgba8(0,0,255,255);
	float m_ageInSeconds = 0.f;

	bool m_isDead = false;
	bool m_isGarbage = false;


	float m_timeWhenGotHurt = 0.f;

};

