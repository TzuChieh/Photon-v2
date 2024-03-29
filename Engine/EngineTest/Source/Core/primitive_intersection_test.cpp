#include <Core/Intersection/PLatLong01Sphere.h>
#include <Core/Ray.h>
#include <Core/Intersection/PrimitiveMetadata.h>

#include <gtest/gtest.h>

#include <limits>
#include <memory>

using namespace ph;
using namespace ph::math;

TEST(PrimitiveIntersectionTest, RaySphereIntersection)
{
	std::unique_ptr<Intersectable> unitSphere = std::make_unique<PLatLong01Sphere>(1.0_r);

	Ray longXAxisRay(
		Vector3R(-100000.0_r, 0, 0), 
		Vector3R(1, 0, 0), 
		0, 
		std::numeric_limits<real>::max());
	EXPECT_TRUE(unitSphere->isOccluding(longXAxisRay));

	Ray shortXAxisRay(
		Vector3R(-100000.0_r, 0, 0),
		Vector3R(1, 0, 0), 
		0, 
		1);
	EXPECT_FALSE(unitSphere->isOccluding(shortXAxisRay));

	Ray insideUnitSphereRay(
		Vector3R(0, 0, 0), 
		Vector3R(1, 0, 0), 
		0, 
		0.1_r);
	EXPECT_FALSE(unitSphere->isOccluding(insideUnitSphereRay));

	Ray fromInsideToOutsideUnitSphereRay(
		Vector3R(0, 0, 0), 
		Vector3R(1, 0, 0), 
		0, 
		std::numeric_limits<real>::max());
	EXPECT_TRUE(unitSphere->isOccluding(fromInsideToOutsideUnitSphereRay));
}
