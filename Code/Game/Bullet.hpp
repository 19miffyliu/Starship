#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

constexpr int NUM_BULLETS_VERTS = 6;
class Bullet : public Entity
{
public:
	Bullet();
	Bullet(Game* owner, Vec2 pos, float orientationDeg);
	void Update(float deltaSeconds) override;
	void Render() const override;
	//void DebugRender() const override;

	bool IsOffscreen()const override;

private:
	void InitializeLocalVerts(); 

private:
	Vertex_PCU m_localVerts[NUM_BULLETS_VERTS];
};

