#include <Common/exceptions.h>

#include <Engine/Core/Intersection/DataStructure/IndexedAttributeBuffer.h>
#include <Engine/Math/Geometry/TSphere.h>
#include <Engine/Math/math.h>

#include <gtest/gtest.h>

#include <array>
#include <limits>
#include <vector>

using namespace ph;
using namespace ph::math;

TEST(IndexedAttributeBufferTest, BasicBufferStates)
{
	{
		IndexedAttributeBuffer buffer;
		EXPECT_FALSE(buffer.hasAttribute(EPrimitiveAttribute::Position_0));
	}

	{
		IndexedAttributeBuffer buffer;
		buffer.declareAttribute(EPrimitiveAttribute::Position_0, EAttributeDomain::Vertex, EAttributeElement::Float32, 3);
		
		const auto writer = buffer.allocate(100);
		EXPECT_EQ(writer.numVertices(), 100);
		EXPECT_GT(writer.memoryUsage(), 100 * 32 * 3 / 8);
	}

	{
		IndexedAttributeBuffer buffer;
		buffer.declareAttribute(EPrimitiveAttribute::Position_0, EAttributeDomain::Vertex, EAttributeElement::Float16, 3);
		buffer.declareAttribute(EPrimitiveAttribute::Normal_0, EAttributeDomain::Vertex, EAttributeElement::OctahedralUnitVec3_24, 3);
		buffer.declareAttribute(EPrimitiveAttribute::TexCoord_0, EAttributeDomain::Vertex, EAttributeElement::Int16, 2);
		
		const auto writer = buffer.allocate(100);
		EXPECT_EQ(writer.numVertices(), 100);
		EXPECT_GT(writer.memoryUsage(), 100 * (16 * 3 + 24 + 16 * 2) / 8);
	}
}

TEST(IndexedAttributeBufferTest, RejectsMixedLayouts)
{
	IndexedAttributeBuffer buffer;
	buffer.declareAttribute(EPrimitiveAttribute::Position_0, EAttributeDomain::Vertex, EAttributeElement::Float32, 3);
	buffer.declareAttribute(EPrimitiveAttribute::Normal_0, EAttributeDomain::Vertex, EAttributeElement::Float32, 3, 0, 12);

	EXPECT_THROW((void)buffer.allocate(1), InvalidArgumentException);
}

TEST(IndexedAttributeBufferTest, BufferIOFloatTypes)
{
	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-6_r;

		IndexedAttributeBuffer buffer;
		buffer.declareAttribute(EPrimitiveAttribute::Position_0, EAttributeDomain::Vertex, EAttributeElement::Float32, 3);

		auto writer = buffer.allocate(1);
		writer.setAttribute(EPrimitiveAttribute::Position_0, 0, {-1, -2, -3});
		EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::Position_0, 0).isNear({-1, -2, -3}, MAX_ALLOWED_ABS_ERROR));
	}

	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-6_r;

		IndexedAttributeBuffer buffer;
		buffer.declareAttribute(EPrimitiveAttribute::TexCoord_0, EAttributeDomain::Vertex, EAttributeElement::Float32, 2);
		
		auto writer = buffer.allocate(3);
		writer.setAttribute(EPrimitiveAttribute::TexCoord_0, 0, {-1, -2});
		writer.setAttribute(EPrimitiveAttribute::TexCoord_0, 1, {0, 1});
		writer.setAttribute(EPrimitiveAttribute::TexCoord_0, 2, {2, 3});
		EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::TexCoord_0, 0).isNear({-1, -2, 0}, MAX_ALLOWED_ABS_ERROR));
		EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::TexCoord_0, 1).isNear({0, 1, 0}, MAX_ALLOWED_ABS_ERROR));
		EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::TexCoord_0, 2).isNear({2, 3, 0}, MAX_ALLOWED_ABS_ERROR));
	}

	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-6_r;

		IndexedAttributeBuffer buffer;
		buffer.declareAttribute(EPrimitiveAttribute::Normal_0, EAttributeDomain::Vertex, EAttributeElement::Float32, 3);
		
		auto writer = buffer.allocate(1000);

		for(std::size_t i = 0; i < writer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i);
			writer.setAttribute(EPrimitiveAttribute::Normal_0, i, {val, val + 1.0_r, -val});
		}

		for(std::size_t i = 0; i < writer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i);
			EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::Normal_0, i).isNear({val, val + 1.0_r, -val}, MAX_ALLOWED_ABS_ERROR));
		}
	}

	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-6_r;

		IndexedAttributeBuffer buffer;
		buffer.declareAttribute(EPrimitiveAttribute::Position_0, EAttributeDomain::Vertex, EAttributeElement::Float16, 3);
		
		auto writer = buffer.allocate(1000);

		for(std::size_t i = 0; i < writer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i);
			writer.setAttribute(EPrimitiveAttribute::Position_0, i, {val, val + 1.0_r, -val});
		}

		for(std::size_t i = 0; i < writer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i);
			EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::Position_0, i).isNear({val, val + 1.0_r, -val}, MAX_ALLOWED_ABS_ERROR));
		}
	}
}

TEST(IndexedAttributeBufferTest, BufferIOIntegerTypes)
{
	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-5_r;

		IndexedAttributeBuffer buffer;
		buffer.declareAttribute(EPrimitiveAttribute::Position_0, EAttributeDomain::Vertex, EAttributeElement::Int32, 3);
		
		auto writer = buffer.allocate(1000);

		for(std::size_t i = 0; i < writer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i);
			writer.setAttribute(EPrimitiveAttribute::Position_0, i, {val, val + 1.0_r, -val * val});
		}

		for(std::size_t i = 0; i < writer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i);
			EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::Position_0, i).isNear({val, val + 1.0_r, -val * val}, MAX_ALLOWED_ABS_ERROR));
		}
	}

	// Normalized int32
	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-5_r;

		IndexedAttributeBuffer buffer;
		buffer.declareAttribute(EPrimitiveAttribute::Position_0, EAttributeDomain::Vertex, EAttributeElement::Int32, 3, true);
		
		auto writer = buffer.allocate(1000);

		for(std::size_t i = 0; i < writer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i) / 1000.0_r;
			writer.setAttribute(EPrimitiveAttribute::Position_0, i, {val, val * val, -val * val});
		}

		for(std::size_t i = 0; i < writer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i) / 1000.0_r;
			EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::Position_0, i).isNear({val, val * val, -val * val}, MAX_ALLOWED_ABS_ERROR));
		}
	}
}

TEST(IndexedAttributeBufferTest, BufferIOOctahedronNormalEncoding)
{
	const TSphere<real> unitSphere(1);

	// Normal vectors generated from a unit sphere, on each lat-long degree
	std::vector<Vector3R> normalVectors;
	for(int thetaDegrees = 0; thetaDegrees <= 180; ++thetaDegrees)
	{
		for(int phiDegrees = 0; phiDegrees <= 360; ++phiDegrees)
		{
			normalVectors.push_back(unitSphere.phiThetaToSurface(
				{to_radians(static_cast<real>(phiDegrees)), to_radians(static_cast<real>(thetaDegrees))}));
		}
	}

	auto makeNormalBuffer = [&normalVectors](const EAttributeElement element)
	{
		IndexedAttributeBuffer buffer;
		buffer.declareAttribute(EPrimitiveAttribute::Normal_0, EAttributeDomain::Vertex, element, 3);

		auto writer = buffer.allocate(normalVectors.size());
		for(std::size_t i = 0; i < normalVectors.size(); ++i)
		{
			writer.setAttribute(EPrimitiveAttribute::Normal_0, i, normalVectors[i]);
		}

		return buffer;
	};

	// Baseline: float32
	{
		// Sould really have much smaller error than 1e-6 (re-normalization error only)
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-6_r;

		const auto buffer = makeNormalBuffer(EAttributeElement::Float32);
		for(std::size_t i = 0; i < normalVectors.size(); ++i)
		{
			EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::Normal_0, i).isNear(normalVectors[i], MAX_ALLOWED_ABS_ERROR));
		}
	}

	// 32-bit encoding
	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-4_r;

		const auto buffer = makeNormalBuffer(EAttributeElement::OctahedralUnitVec3_32);
		for(std::size_t i = 0; i < normalVectors.size(); ++i)
		{
			EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::Normal_0, i).isNear(normalVectors[i], MAX_ALLOWED_ABS_ERROR));
		}
	}

	// 24-bit encoding
	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-3_r;

		const auto buffer = makeNormalBuffer(EAttributeElement::OctahedralUnitVec3_24);
		for(std::size_t i = 0; i < normalVectors.size(); ++i)
		{
			EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::Normal_0, i).isNear(normalVectors[i], MAX_ALLOWED_ABS_ERROR));
		}
	}
}

TEST(IndexedAttributeBufferTest, BufferIOMixedAttributes)
{
	// Mixed attributes with default AoS layout
	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-6_r;

		IndexedAttributeBuffer buffer;
		buffer.declareAttribute(EPrimitiveAttribute::TexCoord_0, EAttributeDomain::Vertex, EAttributeElement::Float32, 2);
		buffer.declareAttribute(EPrimitiveAttribute::Position_0, EAttributeDomain::Vertex, EAttributeElement::Float32, 3);
		buffer.declareAttribute(EPrimitiveAttribute::Custom_0, EAttributeDomain::Face, EAttributeElement::Float32, 1);
		
		auto writer = buffer.allocate(3, 1);

		writer.setAttribute(EPrimitiveAttribute::TexCoord_0, 0, {-1, -2});
		writer.setAttribute(EPrimitiveAttribute::Position_0, 0, {-3, -4, -5});

		writer.setAttribute(EPrimitiveAttribute::TexCoord_0, 1, {6, 7});
		writer.setAttribute(EPrimitiveAttribute::Position_0, 1, {-8, -9, -10});

		writer.setAttribute(EPrimitiveAttribute::TexCoord_0, 2, {-11, -12});
		writer.setAttribute(EPrimitiveAttribute::Position_0, 2, {13, 14, 15});

		writer.setAttribute(EPrimitiveAttribute::Custom_0, 0, 16.0_r);

		EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::TexCoord_0, 0).isNear({-1, -2, 0}, MAX_ALLOWED_ABS_ERROR));
		EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::Position_0, 0).isNear({-3, -4, -5}, MAX_ALLOWED_ABS_ERROR));

		EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::TexCoord_0, 1).isNear({6, 7, 0}, MAX_ALLOWED_ABS_ERROR));
		EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::Position_0, 1).isNear({-8, -9, -10}, MAX_ALLOWED_ABS_ERROR));

		EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::TexCoord_0, 2).isNear({-11, -12, 0}, MAX_ALLOWED_ABS_ERROR));
		EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::Position_0, 2).isNear({13, 14, 15}, MAX_ALLOWED_ABS_ERROR));

		EXPECT_EQ(buffer.getAttribute(EPrimitiveAttribute::Custom_0, 0), Vector3R(16, 0, 0));
	}

	// Mixed attributes with custom SoA layout
	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-6_r;

		std::size_t numVertices = 10;
		std::size_t numFaces = 2;

		IndexedAttributeBuffer buffer;
		buffer.declareAttribute(EPrimitiveAttribute::Normal_0, EAttributeDomain::Vertex, EAttributeElement::Float16, 3, 0, 2*3);
		buffer.declareAttribute(EPrimitiveAttribute::Position_0, EAttributeDomain::Vertex, EAttributeElement::Float32, 3, 2*3*numVertices, 4*3);
		buffer.declareAttribute(EPrimitiveAttribute::Custom_0, EAttributeDomain::Face, EAttributeElement::Float32, 1, (2*3 + 4*3)*numVertices, 4);
		
		auto writer = buffer.allocate(numVertices, numFaces);

		// Setting values
		for(std::size_t vi = 0; vi < numVertices; ++vi)
		{
			const auto value = static_cast<real>(vi);
			writer.setAttribute(EPrimitiveAttribute::Normal_0, vi, {-value, -value, value});
			writer.setAttribute(EPrimitiveAttribute::Position_0, vi, {value, value, value});
		}
		writer.setAttribute(EPrimitiveAttribute::Custom_0, 0, 10.0_r);
		writer.setAttribute(EPrimitiveAttribute::Custom_0, 1, 20.0_r);

		// Testing values
		for(std::size_t vi = 0; vi < numVertices; ++vi)
		{
			const auto value = static_cast<real>(vi);
			EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::Normal_0, vi).isNear({-value, -value, value}, MAX_ALLOWED_ABS_ERROR));
			EXPECT_TRUE(buffer.getAttribute(EPrimitiveAttribute::Position_0, vi).isNear({value, value, value}, MAX_ALLOWED_ABS_ERROR));
		}
		EXPECT_EQ(buffer.getAttribute(EPrimitiveAttribute::Custom_0, 0), Vector3R(10, 0, 0));
		EXPECT_EQ(buffer.getAttribute(EPrimitiveAttribute::Custom_0, 1), Vector3R(20, 0, 0));
	}
}

TEST(IndexedAttributeBufferTest, GetAttributeBatch)
{
	IndexedAttributeBuffer buffer;
	buffer.declareAttribute(EPrimitiveAttribute::Position_0, EAttributeDomain::Vertex, EAttributeElement::Float32, 3);
	buffer.declareAttribute(EPrimitiveAttribute::TexCoord_0, EAttributeDomain::Vertex, EAttributeElement::Float16, 2);

	auto writer = buffer.allocate(2);
	writer.setAttribute(EPrimitiveAttribute::Position_0, 0, Vector3R(1, 2, 3));
	writer.setAttribute(EPrimitiveAttribute::Position_0, 1, Vector3R(4, 5, 6));
	writer.setAttribute(EPrimitiveAttribute::TexCoord_0, 0, Vector3R(0.5_r, -0.5_r, 0));
	writer.setAttribute(EPrimitiveAttribute::TexCoord_0, 1, Vector3R(1.5_r, 2.5_r, 0));

	const std::array<uint32, 3> indices = {1, 0, 1};
	const std::array<Vector3R, 3> expectedPositions = {
		Vector3R(4, 5, 6),
		Vector3R(1, 2, 3),
		Vector3R(4, 5, 6)};
	const std::array<Vector3R, 3> expectedTexCoords = {
		Vector3R(1.5_r, 2.5_r, 0),
		Vector3R(0.5_r, -0.5_r, 0),
		Vector3R(1.5_r, 2.5_r, 0)};
	const std::array<Vector3R, 3> expectedMissingAttributes = {
		Vector3R(0),
		Vector3R(0),
		Vector3R(0)};

	EXPECT_EQ(
		buffer.getAttribute(EPrimitiveAttribute::Position_0, indices),
		expectedPositions);
	EXPECT_EQ(
		buffer.getAttribute(EPrimitiveAttribute::TexCoord_0, indices),
		expectedTexCoords);
	EXPECT_EQ(
		buffer.getAttribute(EPrimitiveAttribute::Color_0, indices),
		expectedMissingAttributes);
}
