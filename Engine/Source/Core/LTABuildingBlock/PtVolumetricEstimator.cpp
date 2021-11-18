#include "Core/LTABuildingBlock/PtVolumetricEstimator.h"
#include "Math/TVector3.h"
#include "World/Scene.h"
#include "Core/SurfaceHit.h"
#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Core/VolumeBehavior/VolumeOptics.h"
#include "Core/VolumeBehavior/VolumeDistanceSample.h"

#include <limits>

namespace ph
{

void PtVolumetricEstimator::sample(
	const Scene& scene,
	const SurfaceHit& Xs,
	const math::Vector3R& L,
	SurfaceHit* out_Xe,
	math::Vector3R* out_V,
	math::Spectrum* out_weight,
	math::Spectrum* out_radiance)
{
	out_weight->setColorValues(1);
	out_radiance->setColorValues(0);

	const PrimitiveMetadata* metadata = Xs.getDetail().getPrimitive()->getMetadata();
	const VolumeOptics* interior = metadata->getInterior().getOptics();
	SurfaceHit currXs = Xs;
	SurfaceHit currXe;
	math::Vector3R currL = L;
	while(interior)
	{
		HitProbe probe;
		Ray ray(currXs.getPosition(), currL, 0.0001_r, std::numeric_limits<real>::max());
		if(scene.isIntersecting(ray, &probe))
		{
			currXe = SurfaceHit(ray, probe);
		}
		else
		{
			break;
		}

		VolumeDistanceSample distSample;
		distSample.inputs.set(currXs, currL, currXe.getDetail().getRayT());
		interior->sample(distSample);

		out_weight->mulLocal(distSample.outputs.pdfAppliedWeight);
		if(!distSample.isMaxDistReached())
		{
			currXs = currXe;

			// ???
			currL = currL;
		}
		else
		{
			*out_Xe = currXe;
			*out_V = currL.mulLocal(-1);
			out_radiance->setColorValues(0);
			break;
		}
	}
}

}// end namespace ph
