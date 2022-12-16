#include <Core/Intersectable/DataStructure/IndexedVertexBuffer.h>
#include <Math/Geometry/TSphere.h>
#include <Math/math.h>

#include <gtest/gtest.h>

#include <limits>
#include <vector>

using namespace ph;
using namespace ph::math;

TEST(IndexedVertexBufferTest, BasicBufferStates)
{
	{
		IndexedVertexBuffer buffer;
		EXPECT_FALSE(buffer.isAllocated());
		EXPECT_EQ(buffer.numVertices(), 0);
		EXPECT_GT(buffer.estimateMemoryUsage(), 0);
	}

	{
		IndexedVertexBuffer buffer;
		buffer.declareEntry(EVertexAttribute::Position_0, EVertexElement::Float32, 3);
		buffer.allocate(100);
		EXPECT_TRUE(buffer.isAllocated());
		EXPECT_EQ(buffer.numVertices(), 100);
		EXPECT_GT(buffer.estimateMemoryUsage(), 100 * 32 * 3 / 8);
	}

	{
		IndexedVertexBuffer buffer;
		buffer.declareEntry(EVertexAttribute::Position_0, EVertexElement::Float16, 3);
		buffer.declareEntry(EVertexAttribute::Normal_0, EVertexElement::OctahedralUnitVec3_24, 3);
		buffer.declareEntry(EVertexAttribute::TexCoord_0, EVertexElement::Int16, 2);
		buffer.allocate(100);
		EXPECT_TRUE(buffer.isAllocated());
		EXPECT_EQ(buffer.numVertices(), 100);
		EXPECT_GT(buffer.estimateMemoryUsage(), 100 * (16 * 3 + 24 + 16 * 2) / 8);
	}
}

TEST(IndexedVertexBufferTest, BufferIOFloatTypes)
{
	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-6_r;

		IndexedVertexBuffer buffer;
		buffer.declareEntry(EVertexAttribute::Position_0, EVertexElement::Float32, 3);
		buffer.allocate(1);

		buffer.setAttribute(EVertexAttribute::Position_0, 0, {-1, -2, -3});
		EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::Position_0, 0).isNear({-1, -2, -3}, MAX_ALLOWED_ABS_ERROR));
	}

	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-6_r;

		IndexedVertexBuffer buffer;
		buffer.declareEntry(EVertexAttribute::TexCoord_0, EVertexElement::Float32, 2);
		buffer.allocate(3);

		buffer.setAttribute(EVertexAttribute::TexCoord_0, 0, {-1, -2});
		buffer.setAttribute(EVertexAttribute::TexCoord_0, 1, {0, 1});
		buffer.setAttribute(EVertexAttribute::TexCoord_0, 2, {2, 3});
		EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::TexCoord_0, 0).isNear({-1, -2, 0}, MAX_ALLOWED_ABS_ERROR));
		EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::TexCoord_0, 1).isNear({0, 1, 0}, MAX_ALLOWED_ABS_ERROR));
		EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::TexCoord_0, 2).isNear({2, 3, 0}, MAX_ALLOWED_ABS_ERROR));
	}

	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-6_r;

		IndexedVertexBuffer buffer;
		buffer.declareEntry(EVertexAttribute::Normal_0, EVertexElement::Float32, 3);
		buffer.allocate(1000);

		for(std::size_t i = 0; i < buffer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i);
			buffer.setAttribute(EVertexAttribute::Normal_0, i, {val, val + 1.0_r, -val});
		}

		for(std::size_t i = 0; i < buffer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i);
			EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::Normal_0, i).isNear({val, val + 1.0_r, -val}, MAX_ALLOWED_ABS_ERROR));
		}
	}

	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-6_r;

		IndexedVertexBuffer buffer;
		buffer.declareEntry(EVertexAttribute::Position_0, EVertexElement::Float16, 3);
		buffer.allocate(1000);

		for(std::size_t i = 0; i < buffer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i);
			buffer.setAttribute(EVertexAttribute::Position_0, i, {val, val + 1.0_r, -val});
		}

		for(std::size_t i = 0; i < buffer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i);
			EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::Position_0, i).isNear({val, val + 1.0_r, -val}, MAX_ALLOWED_ABS_ERROR));
		}
	}
}

TEST(IndexedVertexBufferTest, BufferIOIntegerTypes)
{
	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-5_r;

		IndexedVertexBuffer buffer;
		buffer.declareEntry(EVertexAttribute::Position_0, EVertexElement::Int32, 3);
		buffer.allocate(1000);

		for(std::size_t i = 0; i < buffer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i);
			buffer.setAttribute(EVertexAttribute::Position_0, i, {val, val + 1.0_r, -val * val});
		}

		for(std::size_t i = 0; i < buffer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i);
			EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::Position_0, i).isNear({val, val + 1.0_r, -val * val}, MAX_ALLOWED_ABS_ERROR));
		}
	}

	// Normalized int32
	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-5_r;

		IndexedVertexBuffer buffer;
		buffer.declareEntry(EVertexAttribute::Position_0, EVertexElement::Int32, 3, true);
		buffer.allocate(1000);

		for(std::size_t i = 0; i < buffer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i) / 1000.0_r;
			buffer.setAttribute(EVertexAttribute::Position_0, i, {val, val * val, -val * val});
		}

		for(std::size_t i = 0; i < buffer.numVertices(); ++i)
		{
			const auto val = static_cast<real>(i) / 1000.0_r;
			EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::Position_0, i).isNear({val, val * val, -val * val}, MAX_ALLOWED_ABS_ERROR));
		}
	}
}

TEST(IndexedVertexBufferTest, BufferIOOctahedronNormalEncoding)
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

	auto makeNormalBuffer = [&normalVectors](const EVertexElement element)
	{
		IndexedVertexBuffer buffer;
		buffer.declareEntry(EVertexAttribute::Normal_0, EVertexElement::Float32, 3);
		buffer.allocate(normalVectors.size());
		for(std::size_t i = 0; i < buffer.numVertices(); ++i)
		{
			buffer.setAttribute(EVertexAttribute::Normal_0, i, normalVectors[i]);
		}

		return buffer;
	};

	// Baseline: float32 
	{
		// Sould really have much smaller error than 1e-6 (re-normalization error only)
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-6_r;

		const auto buffer = makeNormalBuffer(EVertexElement::Float32);
		for(std::size_t i = 0; i < buffer.numVertices(); ++i)
		{
			EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::Normal_0, i).isNear(normalVectors[i], MAX_ALLOWED_ABS_ERROR));
		}
	}

	// 32-bit encoding
	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-4_r;

		const auto buffer = makeNormalBuffer(EVertexElement::OctahedralUnitVec3_32);
		for(std::size_t i = 0; i < buffer.numVertices(); ++i)
		{
			EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::Normal_0, i).isNear(normalVectors[i], MAX_ALLOWED_ABS_ERROR));
		}
	}

	// 24-bit encoding
	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-3_r;

		const auto buffer = makeNormalBuffer(EVertexElement::OctahedralUnitVec3_24);
		for(std::size_t i = 0; i < buffer.numVertices(); ++i)
		{
			EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::Normal_0, i).isNear(normalVectors[i], MAX_ALLOWED_ABS_ERROR));
		}
	}
}

TEST(IndexedVertexBufferTest, BufferIOMixedAttributes)
{
	// Mixed attributes with default AoS layout
	{
		constexpr auto MAX_ALLOWED_ABS_ERROR = 1e-6_r;

		IndexedVertexBuffer buffer;
		buffer.declareEntry(EVertexAttribute::TexCoord_0, EVertexElement::Float32, 2);
		buffer.declareEntry(EVertexAttribute::Position_0, EVertexElement::Float32, 3);
		buffer.allocate(3);

		buffer.setAttribute(EVertexAttribute::TexCoord_0, 0, {-1, -2});
		buffer.setAttribute(EVertexAttribute::Position_0, 0, {-3, -4, -5});

		buffer.setAttribute(EVertexAttribute::TexCoord_0, 1, {6, 7});
		buffer.setAttribute(EVertexAttribute::Position_0, 1, {-8, -9, -10});

		buffer.setAttribute(EVertexAttribute::TexCoord_0, 2, {-11, -12});
		buffer.setAttribute(EVertexAttribute::Position_0, 2, {13, 14, 15});

		EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::TexCoord_0, 0).isNear({-1, -2, 0}, MAX_ALLOWED_ABS_ERROR));
		EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::Position_0, 0).isNear({-3, -4, -5}, MAX_ALLOWED_ABS_ERROR));
		
		EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::TexCoord_0, 1).isNear({6, 7, 0}, MAX_ALLOWED_ABS_ERROR));
		EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::Position_0, 1).isNear({-8, -9, -10}, MAX_ALLOWED_ABS_ERROR));
	
		EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::TexCoord_0, 2).isNear({-11, -12, 0}, MAX_ALLOWED_ABS_ERROR));
		EXPECT_TRUE(buffer.getAttribute(EVertexAttribute::Position_0, 2).isNear({13, 14, 15}, MAX_ALLOWED_ABS_ERROR));
	}

	// TODO: SoA
}
