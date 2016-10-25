#include "Core/PathTracer.h"
#include "Common/primitive_type.h"
#include "Frame/HDRFrame.h"
#include "Core/World.h"
#include "Camera/Camera.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"

namespace ph
{

void PathTracer::trace(const Camera& camera, const World& world, HDRFrame* const out_hdrFrame) const
{
	const uint32 widthPx = out_hdrFrame->getWidthPx();
	const uint32 heightPx = out_hdrFrame->getHeightPx();

	Ray ray;
	Intersection intersection;

	for(uint32 y = 0; y < heightPx; y++)
	{
		for(uint32 x = 0; x < widthPx; x++)
		{
			camera.genSampleRay(&ray, widthPx, heightPx, x, y);

			if(world.isIntersecting(ray, &intersection))
			{
				out_hdrFrame->setPixel(x, y, 1.0f);
			}
		}
	}
}

}// end namespace ph