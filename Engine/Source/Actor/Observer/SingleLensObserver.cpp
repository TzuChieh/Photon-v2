#include "Actor/Observer/SingleLensObserver.h"
#include "Core/Receiver/PinholeCamera.h"
#include "Core/Receiver/ThinLensCamera.h"

namespace ph
{

std::unique_ptr<Receiver> SingleLensObserver::genReceiver(const CoreCookingContext& ctx)
{

	if(m_lensRadiusMM == 0)
	{

	}
}

math::TDecomposedTransform<float64> SingleLensObserver::makeRasterToObserver() const
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

	math::TDecomposedTransform<float64> rasterToObserver;

	rasterToObserver.scale(
		-sensorSize.x / rasterResolution.x,
		-sensorSize.y / rasterResolution.y,
		1);

	rasterToObserver.translate(
		sensorSize.x / 2,
		sensorSize.y / 2,
		getSensorOffset());

	return rasterToObserver;
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

void SingleLensObserver::genPinholeCamera(const CoreCookingContext& ctx, PinholeCamera* const out_pinholeCamera)
{
	PH_ASSERT(out_pinholeCamera);
	PH_ASSERT_EQ(m_lensRadiusMM, 0);

	
}

void SingleLensObserver::genThinLensCamera(const CoreCookingContext& ctx, , ThinLensCamera* const out_thinLensCamera)
{
	PH_ASSERT(out_thinLensCamera);

}

}// end namespace ph
