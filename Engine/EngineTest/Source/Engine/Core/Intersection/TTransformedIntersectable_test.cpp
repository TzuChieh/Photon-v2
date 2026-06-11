#include <Engine/Core/Intersection/PEmpty.h>
#include <Engine/Core/Intersection/TTransformedIntersectable.h>
#include <Engine/Core/Transform/StaticAffineTransform.h>

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
