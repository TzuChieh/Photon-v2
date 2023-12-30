#pragma once

#include "Math/math_fwd.h"
#include "Math/Color/Spectrum.h"
#include "Math/TVector3.h"
#include "Core/LTABuildingBlock/SidednessAgreement.h"

#include <Common/primitive_type.h>

namespace ph
{

class Scene;
class SurfaceHit;
class Time;
class SampleFlow;
class Emitter;
class Primitive;
class SurfaceOptics;

/*! @brief Estimate direct lighting for a surface point.
This is a lightweight helper type for estimating direct lighting. Do not think "direct lighting" as
lighting from a directional light source, it means the first-bounce lighting for any surface point,
and the surface point can be the N-th one in a path.
*/
template<ESidednessPolicy POLICY>
class TDirectLightEstimator final
{
public:
	explicit TDirectLightEstimator(const Scene* scene);

	/*! @brief Sample light using BSDF's suggestion.
	*/
	bool bsdfSample(
		const SurfaceHit&     X,
		const math::Vector3R& V,
		const Time&           time,
		SampleFlow&           sampleFlow,
		math::Vector3R*       out_L = nullptr,
		real*                 out_pdfW = nullptr,
		math::Spectrum*       out_radianceLe = nullptr,
		SurfaceHit*           out_Xe = nullptr) const;

	/*! @brief Sample light using next-event estimation.
	*/
	bool neeSample(
		const SurfaceHit&     X,
		const Time&           time,
		SampleFlow&           sampleFlow,
		math::Vector3R*       out_L = nullptr,
		real*                 out_pdfW = nullptr,
		math::Spectrum*       out_radianceLe = nullptr,
		SurfaceHit*           out_Xe = nullptr) const;

	/*! @brief Sample light by combining the techniques used by `bsdfSample()` and `neeSample()`.
	*/
	/*bool misSample(
		const SurfaceHit&     X,
		const Time&           time,
		SampleFlow&           sampleFlow,
		math::Spectrum*       out_radianceLe = nullptr) const;*/

	/*! @brief Get the solid angle domain PDF of an next-event estimation sample.
	*/
	real neeSamplePdfWUnoccluded(
		const SurfaceHit&     X,
		const SurfaceHit&     Xe,
		const Time&           time) const;

private:
	const Primitive& getPrimitive(const SurfaceHit& X) const;
	const SurfaceOptics* getSurfaceOptics(const SurfaceHit& X) const;
	const Emitter* getEmitter(const SurfaceHit& X) const;

	const Scene* m_scene;
};

}// end namespace ph

#include "Core/LTABuildingBlock/TDirectLightEstimator.ipp"
