#include "EngineEnv/Observer/SingleLensObserver.h"
#include "Core/Receiver/PinholeCamera.h"
#include "Core/Receiver/ThinLensCamera.h"
#include "EngineEnv/CoreCookingContext.h"
#include "EngineEnv/CoreCookedUnit.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Math/Transform/StaticRigidTransform.h"
#include "Math/math.h"

#include <utility>

namespace ph
{

void SingleLensObserver::cook(const CoreCookingContext& ctx, CoreCookedUnit& cooked)
{
	if(m_lensRadiusMM == 0)
	{
		genPinholeCamera(ctx, cooked);
	}
	else
	{
		genThinLensCamera(ctx, cooked);
	}
}

math::TDecomposedTransform<float64> SingleLensObserver::makeRasterToSensor(const CoreCookingContext& ctx) const
{
	const math::Vector2D rasterResolution(ctx.getFrameSizePx());
	const math::Vector2D sensorSize = getSensorSize(ctx);

	PH_ASSERT_GT(sensorSize.x(), 0);
	PH_ASSERT_GT(sensorSize.y(), 0);

	/* The sensor is centered around the z-axis (+z out of the screen). Note
	that a single lens observer will capture an horizontally & vertically flipped
	image of the scene, naturally. As the engine sets the lower-left corner
	to be (0, 0) in the output image, for the captured image to be "normal" the 
	sensor's upper-right corner must have the minimum raster coordinates,
	illustrated below:

	                                +y   sensor-max
	                                |    raster-min
	                        +-------|-------+
	                        |       |       |
	                  --------------+-------------- +x
	                        |       |       |
	                        +-------|-------+
	                  sensor-min    |
	                  raster-max
	*/

	math::TDecomposedTransform<float64> rasterToSensor;

	rasterToSensor.scale(
		-sensorSize.x() / rasterResolution.x(),
		-sensorSize.y() / rasterResolution.y(),
		1);

	rasterToSensor.translate(
		sensorSize.x() / 2,
		sensorSize.y() / 2,
		getSensorOffset(ctx));

	return rasterToSensor;
}

math::Vector2D SingleLensObserver::getSensorSize(const CoreCookingContext& ctx) const
{
	const float64 sensorHeightMM = m_sensorWidthMM / ctx.getAspectRatio();
	return math::Vector2D(m_sensorWidthMM, sensorHeightMM).div(1000.0);
}

float64 SingleLensObserver::getSensorOffset(const CoreCookingContext& ctx) const
{
	if(m_fovDegrees.has_value())
	{
		const auto halfFov = math::to_radians(*m_fovDegrees) * 0.5;
		return (getSensorSize(ctx).x() * 0.5) / std::tan(halfFov);
	}
	else
	{
		return m_sensorOffsetMM / 1000.0;
	}
}

void SingleLensObserver::genPinholeCamera(const CoreCookingContext& ctx, CoreCookedUnit& cooked)
{
	PH_ASSERT_EQ(m_lensRadiusMM, 0);

	auto rasterToSensor = std::make_unique<math::StaticAffineTransform>(
		math::StaticAffineTransform::makeForward(makeRasterToSensor(ctx)));
	auto receiverToWorld = std::make_unique<math::StaticRigidTransform>(
		math::StaticRigidTransform::makeForward(makeObserverPose()));

	auto camera = std::make_unique<PinholeCamera>(
		getSensorSize(ctx),
		rasterToSensor.get(),
		receiverToWorld.get());

	cooked.addTransform(std::move(rasterToSensor));
	cooked.addTransform(std::move(receiverToWorld));
	cooked.addReceiver(std::move(camera));
}

void SingleLensObserver::genThinLensCamera(const CoreCookingContext& ctx, CoreCookedUnit& cooked)
{
	PH_ASSERT_GT(m_lensRadiusMM, 0);

	auto rasterToSensor = std::make_unique<math::StaticAffineTransform>(
		math::StaticAffineTransform::makeForward(makeRasterToSensor(ctx)));
	auto receiverToWorld = std::make_unique<math::StaticRigidTransform>(
		math::StaticRigidTransform::makeForward(makeObserverPose()));

	auto camera = std::make_unique<ThinLensCamera>(
		getLensRadius(),
		getFocalDistance(),
		getSensorSize(ctx),
		rasterToSensor.get(),
		receiverToWorld.get());

	cooked.addTransform(std::move(rasterToSensor));
	cooked.addTransform(std::move(receiverToWorld));
	cooked.addReceiver(std::move(camera));
}

}// end namespace ph
