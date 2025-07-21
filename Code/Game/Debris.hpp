#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

constexpr int NUM_DEBRIS_TRIANGLES = 8;
constexpr int NUM_DEBRIS_VERTS = 3 * NUM_DEBRIS_TRIANGLES;

class Debris : public Entity
{
public:
	Debris();
	Debris(Game* g, Vec2 pos, float orientationDeg);
	Debris(Game* g, Vec2 pos, float orientationDeg, Vec2 const& velocity, float avgRadius, Rgba8 color);
	void Update(float deltaSeconds) override;
	void Render() const override;

	void ReRenderColors(Rgba8 const& color);

	bool IsOffscreen()const override;

private:
	void InitializeLocalVerts();

private:
	Vertex_PCU m_localVerts[NUM_DEBRIS_VERTS];

};
