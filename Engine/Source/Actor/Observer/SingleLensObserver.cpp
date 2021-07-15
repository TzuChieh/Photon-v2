#include "Actor/Observer/SingleLensObserver.h"
#include "Core/Receiver/PinholeCamera.h"
#include "Core/Receiver/ThinLensCamera.h"
#include "Actor/CoreCookingContext.h"
#include "Actor/CoreCookedUnit.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Math/Transform/StaticRigidTransform.h"

#include <utility>

namespace ph
{

void SingleLensObserver::genReceiver(const CoreCookingContext& ctx, CoreCookedUnit& out_cooked)
{
	if(m_lensRadiusMM == 0)
	{
		genPinholeCamera(ctx, out_cooked);
	}
	else
	{
		genThinLensCamera(ctx, out_cooked);
	}
}

math::TDecomposedTransform<float64> SingleLensObserver::makeRasterToSensor() const
{
	const math::Vector2D rasterResolution(getResolution());
	const math::Vector2D sensorSize = getSensorSize();

	PH_ASSERT_GT(sensorSize.x, 0);
	PH_ASSERT_GT(sensorSize.y, 0);

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
		-sensorSize.x / rasterResolution.x,
		-sensorSize.y / rasterResolution.y,
		1);

	rasterToSensor.translate(
		sensorSize.x / 2,
		sensorSize.y / 2,
		getSensorOffset());

	return rasterToSensor;
}

math::Vector2D SingleLensObserver::getSensorSize() const
{
	const float64 sensorHeightMM = m_sensorWidthMM / getAspectRatio();
	return math::Vector2D(m_sensorWidthMM, sensorHeightMM).div(1000.0);
}

float64 SingleLensObserver::getSensorOffset() const
{
	return m_sensorOffsetMM / 1000.0;
}

void SingleLensObserver::genPinholeCamera(const CoreCookingContext& ctx, CoreCookedUnit& out_cooked)
{
	PH_ASSERT_EQ(m_lensRadiusMM, 0);

	auto rasterToSensor = std::make_unique<math::StaticAffineTransform>(
		math::StaticAffineTransform::makeForward(makeRasterToSensor()));
	auto receiverToWorld = std::make_unique<math::StaticRigidTransform>(
		math::StaticAffineTransform::makeForward(makeObserverPose()));

	auto camera = std::make_unique<PinholeCamera>(
		getSensorSize(),
		rasterToSensor.get(),
		receiverToWorld.get());

	out_cooked.addTransform(std::move(rasterToSensor));
	out_cooked.addTransform(std::move(receiverToWorld));
	out_cooked.addReceiver(std::move(camera));
}

void SingleLensObserver::genThinLensCamera(const CoreCookingContext& ctx, CoreCookedUnit& out_cooked)
{
	PH_ASSERT_GT(m_lensRadiusMM, 0);


}

}// end namespace ph
