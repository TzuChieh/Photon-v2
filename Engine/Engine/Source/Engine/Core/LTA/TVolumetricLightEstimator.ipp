#include "Engine/Core/LTA/TVolumetricLightEstimator.h"
#include "Engine/Math/TVector3.h"
#include "Engine/World/Scene.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/VolumeBehavior/VolumeOptics.h"
#include "Engine/Core/VolumeBehavior/MediumDistanceSample.h"

#include <limits>

namespace ph::lta
{

template<ESidednessPolicy POLICY>
inline TVolumetricLightEstimator<POLICY>::TVolumetricLightEstimator(const Scene* const scene)
	: m_scene(scene)
{
	PH_ASSERT(scene);
}

template<ESidednessPolicy POLICY>
inline bool TVolumetricLightEstimator<POLICY>::sample(
	const SurfaceHit& Xs,
	const math::Vector3R& L,
	SurfaceHit* out_Xe,
	math::Vector3R* out_V,
	math::Spectrum* out_weight,
	math::Spectrum* out_radiance)
{
	out_weight->setColorValues(1);
	out_radiance->setColorValues(0);

	const PrimitiveMetadata& metadata = Xs.getDetail().getPrimitive()->getMetadata();
	const VolumeOptics* interior = metadata.getInterior().getOptics();
	SurfaceHit currXs = Xs;
	SurfaceHit currXe;
	math::Vector3R currL = L;
	while(interior)
	{
		HitProbe probe;
		Ray ray(currXs.getPos(), currL, 0.0001_r, std::numeric_limits<real>::max());
		if(scene.isIntersecting(ray, &probe))
		{
			//currXe = SurfaceHit(ray, probe);
		}
		else
		{
			break;
		}

		MediumDistanceSample distSample;
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

template<ESidednessPolicy POLICY>
inline const Scene& TVolumetricLightEstimator<POLICY>::getScene() const
{
	PH_ASSERT(m_scene);

	return *m_scene;
}

}// end namespace ph::lta
