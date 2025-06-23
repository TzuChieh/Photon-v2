#include "Engine/Core/SurfaceBehavior/SurfaceOptics/MicrofacetNormalMapper.h"
#include "Engine/Core/SurfaceBehavior/BsdfEvalQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfSampleQuery.h"
#include "Engine/Core/SurfaceBehavior/BsdfPdfQuery.h"
#include "Engine/Math/math.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/SurfaceHit.h"
#include "Engine/Core/Texture/TTexture.h"
#include "Engine/Core/SampleGenerator/SampleFlow.h"
#include "Engine/Math/TDecomposedTransform.h"
#include "Engine/Core/Transform/StaticRigidTransform.h"

#include <Common/assertion.h>

#include <cmath>
#include <algorithm>
#include <utility>

namespace ph
{

namespace
{

inline math::Vector3R tangentFacetNormal(const math::Vector3R& Ng, const math::Vector3R& Np)
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
	const math::Vector3R& V)
{
	const real NpDotV= Np.dot(V);
	const real NgDotNp = std::min(Ng.dot(Np), 1.0_r);
	const real sinNgDotNp = std::sqrt(1 - NgDotNp * NgDotNp);
	return math::safe_clamp(NpDotV / (NpDotV + Nt.dot(V) * sinNgDotNp), 0.0_r, 1.0_r);
}

inline std::pair<StaticRigidTransform, StaticRigidTransform> perturbedToWorldTransformPair(
	const math::Vector3R& Ng,
	const math::Vector3R& Np)
{
	const real theta = std::acos(math::clamp(Ng.dot(Np), -1.0_r, 1.0_r));
	const auto rotAxis = Ng.cross(Np).normalize();

	math::TDecomposedTransform<real> perturbedToWorld;
	perturbedToWorld.rotate(rotAxis, theta);

	return {
		StaticRigidTransform::makeForward(perturbedToWorld),
		StaticRigidTransform::makeInverse(perturbedToWorld)};
}

inline SurfaceHit toPerturbedHit(
	const SurfaceHit& worldX,
	const StaticRigidTransform& worldToPerturbed)
{
	Ray localRay;
	worldToPerturbed.transform(worldX.getRay(), &localRay);

	return SurfaceHit(localRay, worldX.getProbe(), worldX.getReason());
}

}// end namespace

MicrofacetNormalMapper::MicrofacetNormalMapper(
	const SurfaceOptics* target,
	const std::shared_ptr<TTexture<math::Vector3R>>& normalMap)

	: m_target(target)
	, m_normalMap(normalMap)
	, m_sampler(math::EColorUsage::Raw)
	, m_format(ENormalMapFormat::PX_PY_PZ)
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
	const auto N = in.getX().getShadingNormal();
	const auto Np = samplePerturbedNormal(in.getX());

	// Just use the original BSDF if perturbation is too small
	if(N.dot(Np) > std::cos(math::to_radians(89.99_r)))
	{
		m_target->genBsdfSampleCore(ctx, in, sampleFlow, out);
		return;
	}

	const auto V = in.getV();
	const auto Nt = tangentFacetNormal(N, Np);
	const auto [perturbedToWorld, worldToPerturbed] = perturbedToWorldTransformPair(N, Np);

	// Sample the perturbed facet
	math::Spectrum bsdfCos(1);
	if(sampleFlow.unflowedPick(lambdaP(N, Np, Nt, V)))
	{
		SurfaceHit perturbedX = toPerturbedHit(in.getX(), worldToPerturbed);

		BsdfSampleInput perturbedIn{};
		perturbedIn.set(perturbedX, perturbedX.getIncidentRay().getDir().normalize() * -1);

		BsdfSampleOutput perturbedOut{};
		m_target->genBsdfSampleCore(ctx, perturbedIn, sampleFlow, perturbedOut);
		if(perturbedOut)
		{
			math::Vector3R L;
			perturbedToWorld.transformV(perturbedOut.getL(), &L);

			// TODO
		}
		else
		{
			bsdfCos *= 0;
		}
	}
	// Sample the tangent facet
	else
	{
		// TODO
	}

	// TODO
}

void MicrofacetNormalMapper::calcBsdfPdfCore(
	const BsdfQueryContext& ctx,
	const BsdfPdfInput&     in,
	BsdfPdfOutput&          out) const
{
	PH_ASSERT_UNREACHABLE_SECTION();
}

math::Vector3R MicrofacetNormalMapper::samplePerturbedNormal(const SurfaceHit& X) const
{
	math::Vector3R Np = m_sampler.sample(*m_normalMap, X);

	if(m_format == ENormalMapFormat::PX_PY_PZ)
	{
		Np = Np * 2 - 1;
		
		// Swizzle into Photon's local space convention
		Np = {Np.y(), Np.z(), Np.x()};

		// Basically the TBN matrix commonly heard in real-time rendering
		Np = X.getDetail().getShadingBasis().localToWorld(Np);
		Np = Np.normalize();
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}

	return Np;
}

}// end namespace ph
