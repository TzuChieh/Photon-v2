#include <Core/Intersectable/PSphere.h>
#include <Core/Ray.h>
#include <Core/Intersectable/PrimitiveMetadata.h>

#include <gtest/gtest.h>

#include <limits>
#include <memory>

TEST(PrimitiveIntersectionTest, RaySphereIntersection)
{
	using namespace ph;

	PrimitiveMetadata metadata;
	std::unique_ptr<Intersectable> unitSphere = std::make_unique<PSphere>(&metadata, 1.0_r);

	Ray longXAxisRay(
		Vector3R(-1000.0_r, 0, 0), 
		Vector3R(1, 0, 0), 
		0, 
		std::numeric_limits<real>::max());
	EXPECT_TRUE(unitSphere->isIntersecting(longXAxisRay));

	Ray shortXAxisRay(
		Vector3R(-std::numeric_limits<real>::max(), 0, 0), 
		Vector3R(1, 0, 0), 
		0, 
		1);
	EXPECT_FALSE(unitSphere->isIntersecting(shortXAxisRay));

	Ray insideUnitSphereRay(
		Vector3R(0, 0, 0), 
		Vector3R(1, 0, 0), 
		0, 
		0.1_r);
	EXPECT_FALSE(unitSphere->isIntersecting(insideUnitSphereRay));

	Ray fromInsideToOutsideUnitSphereRay(
		Vector3R(0, 0, 0), 
		Vector3R(1, 0, 0), 
		0, 
		std::numeric_limits<real>::max());
	EXPECT_TRUE(unitSphere->isIntersecting(fromInsideToOutsideUnitSphereRay));
}