#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

constexpr int NUM_ASTEROIDS_TRIANGLES = 16;
constexpr int NUM_ASTEROIDS_VERTS = 3* NUM_ASTEROIDS_TRIANGLES;

class Asteroid : public Entity
{
public:
	Asteroid();
	Asteroid(Game* g, Vec2 pos, float orientationDeg);
	void Update(float deltaSeconds) override;
	void Render() const override;
	

	void WrapAroundWorld();

	bool IsOffscreen()const override;


private:
	void InitializeLocalVerts();


private:
	Vertex_PCU m_localVerts[NUM_ASTEROIDS_VERTS];

};

