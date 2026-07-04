#include <Engine/Core/Intersection/PEmpty.h>
#include <Engine/Core/Intersection/PLatLong01Sphere.h>
#include <Engine/Core/Intersection/TTransformedIntersectable.h>
#include <Engine/Core/Transform/StaticAffineTransform.h>
#include <Engine/Core/Transform/DynamicLinearTranslation.h>

#include <gtest/gtest.h>

#include <type_traits>

using namespace ph;

TEST(TTransformedIntersectableTest, UsesReferencedGetter)
{
	static_assert(std::is_constructible_v<
		TTransformedIntersectable<TReferencedIntersectableGetter<Intersectable>>,
		TReferencedIntersectableGetter<Intersectable>,
		const Transform*,
		const Transform*>);

	PEmpty primitive;
	StaticAffineTransform localToWorld;
	StaticAffineTransform worldToLocal;

	TTransformedIntersectable<TReferencedIntersectableGetter<Intersectable>> transformedIntersectable(
		TReferencedIntersectableGetter<Intersectable>(&primitive),
		&localToWorld,
		&worldToLocal);

	EXPECT_FALSE(transformedIntersectable.mayOverlapVolume(math::AABB3D(math::Vector3R(0))));
}

TEST(TTransformedIntersectableTest, LinearTranslationAabbBoundsCookedStep)
{
	PLatLong01Sphere sphere(1);
	DynamicLinearTranslation localToWorld({-2, 0, 0}, {3, 0, 0});
	DynamicLinearTranslation worldToLocal = localToWorld.makeInversed();

	TTransformedIntersectable<TReferencedIntersectableGetter<Intersectable>> transformedIntersectable(
		TReferencedIntersectableGetter<Intersectable>(&sphere),
		&localToWorld,
		&worldToLocal);

	const math::AABB3D aabb = transformedIntersectable.calcAABB();

	EXPECT_LE(aabb.getMinVertex().x(), -3.0_r);
	EXPECT_LE(aabb.getMinVertex().y(), -1.0_r);
	EXPECT_LE(aabb.getMinVertex().z(), -1.0_r);
	EXPECT_GE(aabb.getMaxVertex().x(),  4.0_r);
	EXPECT_GE(aabb.getMaxVertex().y(),  1.0_r);
	EXPECT_GE(aabb.getMaxVertex().z(),  1.0_r);
	EXPECT_TRUE(transformedIntersectable.mayOverlapVolume(
		math::AABB3D(math::Vector3R(3.5_r, 0, 0))));
}
