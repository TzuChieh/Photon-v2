#include <Engine/SDL/sdl_helpers.h>
#include <Engine/SDL/TSdl.h>
#include <Engine/Actor/Actor.h>
#include <Engine/Actor/Geometry/GCuboid.h>
#include <Engine/Actor/Material/MatteOpaque.h>

#include <gtest/gtest.h>

#include <typeinfo>

using namespace ph;

namespace
{

struct DummyStruct
{};

}

TEST(SdlHelpersTest, Concepts)
{
	static_assert(CSdlResource<DummyStruct> == false);
	static_assert(CSdlResource<Actor> == true);

	static_assert(CHasStaticSdlCategoryInfo<DummyStruct> == false);
	static_assert(CHasStaticSdlCategoryInfo<Actor> == true);

	static_assert(sdl::category_of<DummyStruct>() == ESdlTypeCategory::Unspecified);
	static_assert(sdl::category_of<Actor>() != ESdlTypeCategory::Unspecified);
}

TEST(SdlHelpersTest, CastToPointer)
{
	auto cuboid = TSdl<GCuboid>::makeResource();

	EXPECT_EQ(sdl::cast_to<GCuboid>(cuboid.get()), cuboid.get());
	EXPECT_EQ(sdl::cast_to<Geometry>(cuboid.get()), cuboid.get());
	EXPECT_EQ(sdl::cast_to<Material>(cuboid.get()), nullptr);

	const ISdlResource* emptyResource = nullptr;
	EXPECT_EQ(sdl::cast_to<const Geometry>(emptyResource), nullptr);
}

TEST(SdlHelpersTest, CastToReference)
{
	auto cuboid = TSdl<GCuboid>::makeResource();

	EXPECT_EQ(&sdl::cast_to<GCuboid>(*cuboid), cuboid.get());
	EXPECT_EQ(&sdl::cast_to<Geometry>(*cuboid), cuboid.get());
	EXPECT_THROW(sdl::cast_to<Material>(*cuboid), std::bad_cast);
}

TEST(SdlHelpersTest, VisitExactResourceType)
{
	auto cuboid = TSdl<GCuboid>::makeResource();
	GCuboid* visitedCuboid = nullptr;

	const bool isVisited = sdl::visit(cuboid.get(),
		[&visitedCuboid](GCuboid& visited)
		{
			visitedCuboid = &visited;
		});

	EXPECT_TRUE(isVisited);
	EXPECT_EQ(visitedCuboid, cuboid.get());
}

TEST(SdlHelpersTest, VisitByBasePtrToDerivedRef)
{
	auto cuboid = TSdl<GCuboid>::makeResource();
	const ISdlResource* resource = cuboid.get();
	const Geometry* visitedGeometry = nullptr;

	const bool isVisited = sdl::visit(resource,
		[&visitedGeometry](const Geometry& visited)
		{
			visitedGeometry = &visited;
		});

	EXPECT_TRUE(isVisited);
	EXPECT_EQ(visitedGeometry, cuboid.get());
}

TEST(SdlHelpersTest, VisitByBaseRefToDerivedRef)
{
	auto cuboid = TSdl<GCuboid>::makeResource();
	const ISdlResource& resource = *cuboid;
	const Geometry* visitedGeometry = nullptr;

	const bool isVisited = sdl::visit(resource,
		[&visitedGeometry](const Geometry& visited)
		{
			visitedGeometry = &visited;
		});

	EXPECT_TRUE(isVisited);
	EXPECT_EQ(visitedGeometry, cuboid.get());
}

TEST(SdlHelpersTest, VisitByBaseRefToDerivedPtr)
{
	auto cuboid = TSdl<GCuboid>::makeResource();
	const ISdlResource& resource = *cuboid;
	const Geometry* visitedGeometry = nullptr;

	const bool isVisited = sdl::visit(resource,
		[&visitedGeometry](const Geometry* visited)
		{
			visitedGeometry = visited;
		});

	EXPECT_TRUE(isVisited);
	EXPECT_EQ(visitedGeometry, cuboid.get());
}

TEST(SdlHelpersTest, VisitByBasePtrToDerivedPtr)
{
	auto cuboid = TSdl<GCuboid>::makeResource();
	const ISdlResource* resource = cuboid.get();
	const Geometry* visitedGeometry = nullptr;

	const bool isVisited = sdl::visit(resource,
		[&visitedGeometry](const Geometry* visited)
		{
			visitedGeometry = visited;
		});

	EXPECT_TRUE(isVisited);
	EXPECT_EQ(visitedGeometry, cuboid.get());
}

TEST(SdlHelpersTest, VisitReturnsFalseOnNoMatch)
{
	auto material = TSdl<MatteOpaque>::makeResource();
	bool isGeometryVisited = false;

	const bool isVisited = sdl::visit(material.get(),
		[&isGeometryVisited](const Geometry& /* visited */)
		{
			isGeometryVisited = true;
		});

	EXPECT_FALSE(isVisited);
	EXPECT_FALSE(isGeometryVisited);
}

TEST(SdlHelpersTest, VisitOnlyFirstMatch)
{
	auto cuboid = TSdl<GCuboid>::makeResource();
	int visitCount = 0;
	const Geometry* visitedGeometry = nullptr;
	const GCuboid* visitedCuboid = nullptr;

	const bool isVisited = sdl::visit(cuboid.get(),
		[&visitCount](const Material& /* visited */)
		{
			++visitCount;
		},
		[&visitCount, &visitedGeometry](const Geometry& visited)
		{
			++visitCount;
			visitedGeometry = &visited;
		},
		[&visitCount, &visitedCuboid](const GCuboid& visited)
		{
			++visitCount;
			visitedCuboid = &visited;
		});

	EXPECT_TRUE(isVisited);
	EXPECT_EQ(visitCount, 1);
	EXPECT_EQ(visitedGeometry, cuboid.get());
	EXPECT_EQ(visitedCuboid, nullptr);
}
