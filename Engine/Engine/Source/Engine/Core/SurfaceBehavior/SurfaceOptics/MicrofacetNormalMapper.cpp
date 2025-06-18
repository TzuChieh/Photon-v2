#include "Engine/Core/SurfaceBehavior/SurfaceOptics/MicrofacetNormalMapper.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Engine/Math/math.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Core/SampleGenerator/SampleFlow.h"

#include <Common/assertion.h>

#include <cmath>
#include <algorithm>

namespace ph
{

namespace
{

inline math::Vector3R Nt(const math::Vector3R& Ng, const math::Vector3R& Np)
{
	return (Ng * Ng.dot(Np) - Np).normalize();
}

inline real G1(
	const math::Vector3R& Ng, 
	const math::Vector3R& Np, 
	const math::Vector3R& Nt, 
	const math::Vector3R& L)
{
	const real NgDotNp = std::min(Ng.dot(Np), 1.0_r);
	const real sinNgDotNp = std::sqrt(1 - NgDotNp * NgDotNp);
	return math::safe_clamp((Ng.dot(L) * Np.dot(L)) / (Np.dot(L) + Nt.dot(L) * sinNgDotNp), 0.0_r, 1.0_r);
}

inline real lambdaP(
	const math::Vector3R& Ng,
	const math::Vector3R& Np,
	const math::Vector3R& Nt,
	const math::Vector3R& L)
{
	const real NpDotL= Np.dot(L);
	const real NgDotNp = std::min(Ng.dot(Np), 1.0_r);
	const real sinNgDotNp = std::sqrt(1 - NgDotNp * NgDotNp);
	return math::safe_clamp(NpDotL / (NpDotL + Nt.dot(L) * sinNgDotNp), 0.0_r, 1.0_r);
}

}// end namespace

MicrofacetNormalMapper::MicrofacetNormalMapper(
	const SurfaceOptics* target,
	const std::shared_ptr<TTexture<math::Vector3R>>& normalMap)

	: m_target(target)
	, m_normalMap(normalMap)
	, m_sampler(math::EColorUsage::Raw)
{
	PH_ASSERT(target);
	PH_ASSERT(normalMap);

	m_phenomena = m_target->getAllPhenomena();
	m_numElementals = m_target->numElementals();
}

ESurfacePhenomenon MicrofacetNormalMapper::getPhenomenonOf(const SurfaceElemental elemental) const
{
	return m_target->getPhenomenonOf(elemental);
}

void MicrofacetNormalMapper::calcBsdfCore(
	const BsdfQueryContext& ctx,
	const BsdfEvalInput&    in,
	BsdfEvalOutput&         out) const
{
	PH_ASSERT_UNREACHABLE_SECTION();
}

void MicrofacetNormalMapper::genBsdfSampleCore(
	const BsdfQueryContext& ctx,
	const BsdfSampleInput&  in,
	SampleFlow&             sampleFlow,
	BsdfSampleOutput&       out) const
{
	// TODO
	m_target->genBsdfSampleCore(ctx, in, sampleFlow, out);
}

void MicrofacetNormalMapper::calcBsdfPdfCore(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	PH_ASSERT_UNREACHABLE_SECTION();
}

}// end namespace ph
