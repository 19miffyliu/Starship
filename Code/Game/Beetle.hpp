#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

constexpr int NUM_BEETLE_TRIANGLES = 2;
constexpr int NUM_BEETLE_VERTS = 3 * NUM_BEETLE_TRIANGLES;

class Beetle: public Entity
{
public:
	Beetle();
	Beetle(Game* g, Vec2 pos, float orientationDeg);
	void Update(float deltaSeconds) override;
	void Render() const override;
	//void DebugRender() const override;

	bool IsOffscreen()const override;

private:
	void InitializeLocalVerts();

private:
	Vertex_PCU m_localVerts[NUM_BEETLE_VERTS];
	Rgba8 m_beetleColor = Rgba8(0, 255, 0, 255);
};

