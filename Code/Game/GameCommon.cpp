#include "GameCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/App.hpp"

//Renderer* g_theRenderer;



void DebugDrawRing(Vec2 const& center, float radius, float thickness, Rgba8 const& color)
{
	constexpr int NUM_SIDES = 12;
	constexpr int NUM_TRIS = 2*NUM_SIDES;
	constexpr int NUM_VERTS = 3*NUM_TRIS;
	constexpr float DEGREES_PER_SIDE = 360.f / static_cast<float>(NUM_SIDES);

	float halfThickness = 0.5f * thickness;
	float innerRadius = radius - halfThickness;
	float outerRadius = radius + halfThickness;

	Vertex_PCU verts[NUM_VERTS];


	for (int sideNum = 0; sideNum < NUM_SIDES; ++sideNum) {
		float startDeg = DEGREES_PER_SIDE * static_cast<float>(sideNum);
		float endDeg = DEGREES_PER_SIDE * static_cast<float>(sideNum+1);
		float cosStart = CosDegrees(startDeg);
		float sinStart = SinDegrees(startDeg);
		float cosEnd = CosDegrees(endDeg);
		float sinEnd = SinDegrees(endDeg);

		Vec3 innerStartPos = Vec3(center.x + innerRadius * cosStart, center.y + innerRadius * sinStart,  0.f);
		Vec3 outerStartPos = Vec3(center.x + outerRadius * cosStart, center.y + outerRadius * sinStart,0.f);

		Vec3 innerEndPos = Vec3(center.x + innerRadius * cosEnd, center.y + innerRadius * sinEnd, 0.f);
		Vec3 outerEndPos = Vec3(center.x + outerRadius * cosEnd, center.y + outerRadius * sinEnd, 0.f);

		//trapezoid are made of 2 triangles
		// first: IE, IS, OS  second: IE, OS, OE

		Vec3 temp;
		for (int index = 0; index < 6; ++index) {
			switch (index) {
			case 0:
				temp = innerEndPos;
				break;
			case 1:
				temp = innerStartPos;
				break;
			case 2:
				temp = outerStartPos;

				break;
			case 3:
				temp = innerEndPos;

				break;
			case 4:
				temp = outerStartPos;
				break;
			case 5:
				temp = outerEndPos;
				break;
			}

			verts[6 * sideNum + index].m_position = temp;
			verts[6*sideNum + index].m_color = color;

		}
		
		g_theRenderer->DrawVertexArray(NUM_VERTS, &verts[0]);

	}

}

void DebugDrawLine(Vec2 const& start, Vec2 const& end, Rgba8 color, float thickness)
{
	Vec2 SE = end - start;
	Vec2 normalizedSE = SE.GetNormalized();

	float h = thickness * 0.5f;
	Vec2 stepF = h * normalizedSE;
	Vec2 stepL = stepF.GetRotated90Degrees();

	Vec2 SL = start - stepF + stepL;
	Vec2 SR = start - stepF - stepL;
	Vec2 EL = end + stepF + stepL;
	Vec2 ER = end + stepF - stepL;

	constexpr int NUM_VERTS = 6;
	Vertex_PCU verts[NUM_VERTS];

	//draw the left part of the line
	verts[0].m_position = GetFromVec2(EL);
	verts[1].m_position = GetFromVec2(SL);
	verts[2].m_position = GetFromVec2(SR);

	//draw the left part of the line
	verts[3].m_position = GetFromVec2(EL);
	verts[4].m_position = GetFromVec2(SR);
	verts[5].m_position = GetFromVec2(ER);

	for (int i = 0; i < NUM_VERTS; ++i) {
		verts[i].m_color = color;
	}

	g_theRenderer->DrawVertexArray(NUM_VERTS, &verts[0]);

}

void DebugDrawDot(Vec2 const& center, float radius, Rgba8 color)
{
	//draw a equilateral triangle at the center
	Vec2 topPoint = Vec2(0.f, 1.f) * radius + center;
	Vec2 leftPoint = Vec2(-sqrt3by2, -0.5f) * radius + center;
	Vec2 rightPoint = Vec2(sqrt3by2, -0.5f) * radius + center;

	Vertex_PCU verts[3];

	verts[0].m_position = GetFromVec2(topPoint);
	verts[1].m_position = GetFromVec2(leftPoint);
	verts[2].m_position = GetFromVec2(rightPoint);


	for (int i = 0; i < 3; ++i) {
		verts[i].m_color = color;
	}

	g_theRenderer->DrawVertexArray(3, &verts[0]);
}

void DebugDrawRectangle(Vec2 const& bottomLeft, Vec2 const& topRight, Rgba8 color)
{
	Vec2 topLeft = Vec2(bottomLeft.x,topRight.y);
	Vec2 bottomRight = Vec2(topRight.x,bottomLeft.y);

	constexpr int NUM_VERTS = 6;
	Vertex_PCU verts[NUM_VERTS];


	//draw top left of rectangle as triangle
	verts[0].m_position = GetFromVec2(topLeft);
	verts[1].m_position = GetFromVec2(bottomLeft);
	verts[2].m_position = GetFromVec2(topRight);


	//draw bottom right of rectangle as triangle
	verts[3].m_position = GetFromVec2(topRight);
	verts[4].m_position = GetFromVec2(bottomLeft);
	verts[5].m_position = GetFromVec2(bottomRight);

	for (int i = 0; i < NUM_VERTS; ++i) {
		verts[i].m_color = color;
	}

	g_theRenderer->DrawVertexArray(NUM_VERTS, &verts[0]);

}

void DebugDrawBox(Vec2 const& bottomLeft, Vec2 const& topRight, Rgba8 color, float thickness)
{
	Vec2 topLeft = Vec2(bottomLeft.x, topRight.y);
	Vec2 bottomRight = Vec2(topRight.x, bottomLeft.y);

	//draw 4 debugline with color and thickness
	DebugDrawLine(topLeft, topRight, color, thickness);
	DebugDrawLine(topLeft, bottomLeft, color, thickness);
	DebugDrawLine(topRight, bottomRight, color, thickness);
	DebugDrawLine(bottomLeft, bottomRight, color, thickness);


}
