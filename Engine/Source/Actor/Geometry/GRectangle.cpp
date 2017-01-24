#include "Actor/Geometry/GRectangle.h"
#include "Core/Primitive/PTriangle.h"
#include "Math/Vector3f.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "FileIO/InputPacket.h"

#include <iostream>

namespace ph
{

GRectangle::GRectangle(const float32 width, const float32 height) :
	m_width(width), m_height(height)
{

}

GRectangle::GRectangle(const InputPacket& packet) : 
	Geometry(packet)
{
	m_width  = packet.getReal("width",  1.0f);
	m_height = packet.getReal("height", 1.0f);
}

GRectangle::~GRectangle() = default;

void GRectangle::discretize(std::vector<std::unique_ptr<Primitive>>* const out_primitives, const PrimitiveMetadata& metadata) const
{
	if(m_width <= 0.0f || m_height <= 0.0f)
	{
		std::cerr << "warning: GRectangle's dimension is zero or negative" << std::endl;
	}

	const float32 halfWidth = m_width * 0.5f;
	const float32 halfHeight = m_height * 0.5f;

	const Vector3f vA(-halfWidth,  halfHeight, 0.0f);// quadrant II
	const Vector3f vB(-halfWidth, -halfHeight, 0.0f);// quadrant III
	const Vector3f vC( halfWidth, -halfHeight, 0.0f);// quadrant IV
	const Vector3f vD( halfWidth,  halfHeight, 0.0f);// quadrant I

	const Vector3f tA(0.0f, 1.0f, 0.0f);// quadrant II
	const Vector3f tB(0.0f, 0.0f, 0.0f);// quadrant III
	const Vector3f tC(1.0f, 0.0f, 0.0f);// quadrant IV
	const Vector3f tD(1.0f, 1.0f, 0.0f);// quadrant I

	PTriangle tri1(&metadata, vA, vB, vD);
	tri1.setUVWa(tA);
	tri1.setUVWb(tB);
	tri1.setUVWc(tD);

	PTriangle tri2(&metadata, vB, vC, vD);
	tri2.setUVWa(tB);
	tri2.setUVWb(tC);
	tri2.setUVWc(tD);

	// 2 triangles for a rectangle (both CCW)
	out_primitives->push_back(std::make_unique<PTriangle>(tri1));
	out_primitives->push_back(std::make_unique<PTriangle>(tri2));
}

}// end namespace ph