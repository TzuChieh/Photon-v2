#pragma once

#include "Math/math_fwd.h"
#include "Math/Color/Spectrum.h"
#include "Core/LTA/SidednessAgreement.h"

#include <Common/primitive_type.h>

namespace ph { class Scene; }
namespace ph { class SurfaceHit; }
namespace ph { class SampleFlow; }
namespace ph { class Emitter; }
namespace ph { class Primitive; }
namespace ph { class SurfaceOptics; }

namespace ph::lta
{

/*! @brief Estimate direct lighting for a surface point.
This is a lightweight helper type for estimating direct lighting. Do not think "direct light" as
lighting from a directional light source, it means the first-bounce lighting for any surface point,
and the surface point can be the N-th one in a path.
*/
template<ESidednessPolicy POLICY>
class TDirectLightEstimator final
{
public:
	explicit TDirectLightEstimator(const Scene* scene);

	/*! @brief Sample lighting using BSDF's suggestion.
	A light sampling technique that is always valid.
	@return Whether output parameters are usable. If `false` is returned, the sample should still
	be treated as valid, albeit its contribution is effectively zero.
	*/
	bool bsdfSampleEmission(
		const SurfaceHit&     X,
		SampleFlow&           sampleFlow,
		math::Vector3R*       out_L = nullptr,
		math::Spectrum*       out_pdfAppliedBsdf = nullptr,
		math::Spectrum*       out_Le = nullptr,
		SurfaceHit*           out_Xe = nullptr) const;

	/*! @brief Sample lighting using next-event estimation.
	This light sampling technique may not always be valid. Calling this method when `isNeeSamplable()`
	returns `false` is an error.
	@return Whether output parameters are usable. If `false` is returned, the sample should still
	be treated as valid, albeit its contribution is effectively zero.
	*/
	bool neeSampleEmission(
		const SurfaceHit&     X,
		SampleFlow&           sampleFlow,
		math::Vector3R*       out_L = nullptr,
		real*                 out_pdfW = nullptr,
		math::Spectrum*       out_Le = nullptr,
		SurfaceHit*           out_Xe = nullptr) const;

	/*! @brief Sample lighting by combining the techniques used by `bsdfSample()` and `neeSample()`.
	A light sampling technique that is always valid.
	@return Whether output parameters are usable. If `false` is returned, the sample should still
	be treated as valid, albeit its contribution is effectively zero.
	*/
	bool bsdfSampleOutgoingWithNee(
		const SurfaceHit&     X,
		SampleFlow&           sampleFlow,
		math::Spectrum*       out_Lo = nullptr,
		math::Vector3R*       out_L = nullptr,
		math::Spectrum*       out_pdfAppliedBsdf = nullptr,
		SurfaceHit*           out_Xe = nullptr) const;

	/*! @brief Get the solid angle domain PDF of an next-event estimation lighting sample.
	Surface occlusion is not taken into account. Calling this method when `isNeeSamplable()`
	returns `false` is an error.
	*/
	real neeSamplePdfWUnoccluded(
		const SurfaceHit&     X,
		const SurfaceHit&     Xe) const;

	/*!
	@return Checks whether next-event estimation is a valid technique to use on the surface `X`.
	*/
	bool isNeeSamplable(const SurfaceHit& X) const;

private:
	const Scene& getScene() const;

	static const Primitive& getPrimitive(const SurfaceHit& X);
	static const SurfaceOptics* getSurfaceOptics(const SurfaceHit& X);
	static const Emitter* getEmitter(const SurfaceHit& X);

	const Scene* m_scene;
};

}// end namespace ph::lta

#include "Core/LTA/TDirectLightEstimator.ipp"
