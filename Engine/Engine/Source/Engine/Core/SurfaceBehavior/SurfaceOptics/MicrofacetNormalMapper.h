#pragma once

#include "Engine/Core/SurfaceBehavior/SurfaceOptics.h"
#include "Engine/Core/Texture/texture_fwd.h"
#include "Engine/Math/math_fwd.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/Texture/TSampler.h"
#include "Engine/Core/SurfaceBehavior/Property/enums.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <algorithm>
#include <cmath>
#include <memory>

namespace ph
{

/*! @brief Microfacet-based normal mapping.
Implements the "normalmap_microfacet_default" model in the original paper.
*/
class MicrofacetNormalMapper : public SurfaceOptics
{
public:
	MicrofacetNormalMapper(
		const SurfaceOptics* target,
		const std::shared_ptr<TTexture<math::Vector3R>>& normalMap,
		ENormalMapFormat format = ENormalMapFormat::PXPYPZ_8Bits);

	ESurfacePhenomenon getPhenomenonOf(SurfaceElemental elemental) const override;

	void calcElementalBsdf(
		const BsdfQueryContext& ctx,
		const BsdfEvalInput&    in,
		BsdfEvalOutput&         out) const override;

	void genElementalBsdfSample(
		const BsdfQueryContext& ctx,
		const BsdfSampleInput&  in,
		SampleFlow&             sampleFlow,
		BsdfSampleOutput&       out) const override;

	void calcElementalBsdfPdf(
		const BsdfQueryContext& ctx,
		const BsdfPdfInput&     in,
		BsdfPdfOutput&          out) const override;

	std::string toString() const override;

	static math::Vector3R decodeNormalMap(math::Vector3R encodedNormal, ENormalMapFormat format);

private:
	/*!
	@return Mapped normal vector in world space.
	*/
	math::Vector3R samplePerturbedNormal(const SurfaceHit& X) const;

	/*!
	@return Is the perturbation too small. It is adviced to just use the original BSDF if this method
	returns true to avoid numerical error later during transform.
	*/
	bool isPerturbationTooSmall(real cosPerturbation) const;

	const SurfaceOptics*                      m_target;
	std::shared_ptr<TTexture<math::Vector3R>> m_normalMap;
	TSampler<math::Vector3R>                  m_sampler;
	ENormalMapFormat                          m_format;
};

// In-header Implementations:

inline std::string MicrofacetNormalMapper::toString() const
{
	return 
		"Microfacet Normal Mapper (Surface Optics), "
		"target: <" + (m_target ? m_target->toString() : "null" ) + ">" +
		", " + SurfaceOptics::toString();
}

inline math::Vector3R MicrofacetNormalMapper::decodeNormalMap(
	math::Vector3R encodedNormal,
	const ENormalMapFormat format)
{
	switch(format)
	{
	case ENormalMapFormat::PXPYPZ_8Bits:
		encodedNormal = encodedNormal * 2 - 1;
		// Swizzle into Photon's local space convention
		return {encodedNormal.y(), encodedNormal.z(), encodedNormal.x()};

	case ENormalMapFormat::PXNYPZ_8Bits:
		encodedNormal = encodedNormal * 2 - 1;
		encodedNormal.y() *= -1;
		// Swizzle into Photon's local space convention
		return {encodedNormal.y(), encodedNormal.z(), encodedNormal.x()};

	case ENormalMapFormat::PXNY_8Bits:
		encodedNormal = encodedNormal * 2 - 1;
		encodedNormal.y() *= -1;
		encodedNormal.z() = std::sqrt(std::max(
			1.0_r - encodedNormal.x() * encodedNormal.x() - encodedNormal.y() * encodedNormal.y(),
			0.0_r));
		// Swizzle into Photon's local space convention
		return {encodedNormal.y(), encodedNormal.z(), encodedNormal.x()};

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return {0, 1, 0};
	}
}

inline bool MicrofacetNormalMapper::isPerturbationTooSmall(real cosPerturbation) const
{
	PH_ASSERT_GE(cosPerturbation, 0);

	switch(m_format)
	{
	case ENormalMapFormat::PXPYPZ_8Bits:
	case ENormalMapFormat::PXNYPZ_8Bits:
	case ENormalMapFormat::PXNY_8Bits:
		// For neutral normal, we have 0.3 degrees of error. See "Normal Unpacking and Quantiation Errors"
		// by Giuseppe (https ://www.aclockworkberry.com/normal-unpacking-quantization-errors/).
		// In our tests, thresholding at 0.4 degree indeed gives us a good result (for a normal map with
		// some off-by-1 error on its neutral normals).

		// > cos(0.4 degree)
		return cosPerturbation > 0.9999756307_r;

	default:
		PH_ASSERT_UNREACHABLE_SECTION();
		return true;
	}
}

}// end namespace ph
