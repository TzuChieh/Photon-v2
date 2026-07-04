#include "engine_test_util.h"

#include <Engine/Core/Receiver/ThinLensCamera.h>
#include <Engine/Core/Ray.h>
#include <Engine/Core/SampleGenerator/SampleFlow.h>
#include <Engine/Core/Quantity/TimeStep.h>
#include <Engine/Core/Transform/StaticAffineTransform.h>
#include <Engine/Core/Transform/StaticRigidTransform.h>
#include <Engine/Math/TDecomposedTransform.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(ThinLensCameraTest, SamplesRayTimeInStep)
{
	math::TDecomposedTransform<float64> rasterToSensorData;
	rasterToSensorData.translate(0, 0, 1);
	const StaticAffineTransform rasterToSensor = StaticAffineTransform::makeForward(rasterToSensorData);

	const ThinLensCamera camera(
		0.5,
		2.0,
		math::Vector2D(1, 1),
		&rasterToSensor,
		&StaticRigidTransform::IDENTITY(),
		TimeStep(0, 10.0_r, 2.0_r));// 10 <= abs time <= 12

	SampleFlow sampleFlow;

	Ray ray;
	camera.receiveRay(math::Vector2D(0, 0), sampleFlow, &ray);

	EXPECT_GE(ray.getTime().getAbsoluteS(), 10.0_r);
	EXPECT_LE(ray.getTime().getAbsoluteS(), 12.0_r);
	EXPECT_GE(ray.getTime().getStepT(), 0.0_r);
	EXPECT_LE(ray.getTime().getStepT(), 1.0_r);
}

TEST(ThinLensCameraTest, ReportsRaySampleDims)
{
	const ThinLensCamera staticCamera(
		0.5,
		2.0,
		math::Vector2D(1, 1),
		&StaticAffineTransform::IDENTITY(),
		&StaticRigidTransform::IDENTITY(),
		TimeStep());

	const ThinLensCamera motionCamera(
		0.5,
		2.0,
		math::Vector2D(1, 1),
		&StaticAffineTransform::IDENTITY(),
		&StaticRigidTransform::IDENTITY(),
		TimeStep(0, 10.0_r, 2.0_r));

	EXPECT_GT(motionCamera.numRaySampleDims(), staticCamera.numRaySampleDims());
}