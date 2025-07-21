#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

constexpr int NUM_WASP_TRIANGLES = 2;
constexpr int NUM_WASP_VERTS = 3 * NUM_WASP_TRIANGLES;

class Wasp: public Entity
{
public:
	Wasp();
	Wasp(Game* g, Vec2 pos, float orientationDeg);
	void Update(float deltaSeconds) override;
	void Render() const override;
	//void DebugRender() const override;

	bool IsOffscreen()const override;

private:
	void InitializeLocalVerts();

private:
	Vertex_PCU m_localVerts[NUM_WASP_VERTS];
	Rgba8 m_waspColor = Rgba8(255, 255, 0, 255);
};

