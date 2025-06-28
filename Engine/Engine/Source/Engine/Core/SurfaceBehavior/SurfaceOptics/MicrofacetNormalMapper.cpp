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
#include "Engine/Core/Transform/StaticAffineTransform.h"

#include <cmath>
#include <algorithm>
#include <array>

namespace ph
{

namespace
{

inline math::Vector3R tangentFacetNormal(const math::Vector3R& Ng, const math::Vector3R& Np)
{
	return (Ng * Ng.dot(Np) - Np).normalize();
}

inline real positiveDot(const math::Vector3R& A, const math::Vector3R& B)
{
	return std::max(A.dot(B), 0.0_r);
}

inline real G1(
	const math::Vector3R& Ng, 
	const math::Vector3R& Np, 
	const math::Vector3R& Nt, 
	math::Vector3R L)
{
	// If light is coming from back face, flip its direction to correctly account for invisible facets
	if(Ng.dot(L) < 0)
	{
		L *= -1;
	}

	const real NgDotNp = std::min(Ng.dot(Np), 1.0_r);
	const real sinNgDotNp = std::sqrt(1 - NgDotNp * NgDotNp);
	return math::safe_clamp(
		(positiveDot(Ng, L) * positiveDot(Np, L)) / (positiveDot(Np, L) + positiveDot(Nt, L) * sinNgDotNp),
		0.0_r,
		1.0_r);
}

inline real lambdaP(
	const math::Vector3R& Ng,
	const math::Vector3R& Np,
	const math::Vector3R& Nt,
	math::Vector3R L)
{
	// If light is coming from back face, flip its direction to correctly account for invisible facets
	if(Ng.dot(L) < 0)
	{
		L *= -1;
	}

	const real NgDotNp = std::min(Ng.dot(Np), 1.0_r);
	const real sinNgDotNp = std::sqrt(1 - NgDotNp * NgDotNp);
	return math::safe_clamp(
		positiveDot(Np, L) / (positiveDot(Np, L) + positiveDot(Nt, L) * sinNgDotNp),
		0.0_r, 
		1.0_r);
}

inline SurfaceHit perturbX(
	const SurfaceHit& X,
	const math::Vector3R& Ng,
	const math::Vector3R& Np)
{
	const real theta = std::acos(math::clamp(Ng.dot(Np), -1.0_r, 1.0_r));
	const auto rotAxis = Ng.cross(Np).normalize();

	std::array<math::TDecomposedTransform<real>, 2> perturbedToWorld;
	perturbedToWorld[0].translate(-X.getPos());// move to origin
	perturbedToWorld[1].rotate(rotAxis, theta);// rotate to the inclined angle
	perturbedToWorld[1].translate(X.getPos()); // move back to hit pos

	SurfaceHit perturbedX;
	StaticAffineTransform::makeParentedForward<real>(perturbedToWorld).transform(X, &perturbedX);
	return perturbedX;
}

}// end namespace

MicrofacetNormalMapper::MicrofacetNormalMapper(
	const SurfaceOptics* target,
	const std::shared_ptr<TTexture<math::Vector3R>>& normalMap)

	: m_target(target)
	, m_normalMap(normalMap)
	, m_sampler(math::EColorUsage::Raw)
	, m_format(ENormalMapFormat::PXPYPZ_8Bits)
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

	if(isPerturbationTooSmall(N.absDot(Np)))
	{
		m_target->genBsdfSampleCore(ctx, in, sampleFlow, out);
		return;
	}

	out.setContributability(false);

	const auto V = in.getV();
	const auto Nt = tangentFacetNormal(N, Np);
	const SurfaceHit perturbedX = perturbX(in.getX(), N, Np);

	// Sample the perturbed facet
	math::Spectrum weight(1);
	if(sampleFlow.unflowedPick(lambdaP(N, Np, Nt, V)))
	{
		BsdfSampleInput perturbedIn{};
		perturbedIn.set(perturbedX, V);

		BsdfSampleOutput perturbedOut{};
		m_target->genBsdfSampleCore(ctx, perturbedIn, sampleFlow, perturbedOut);
		if(perturbedOut)
		{
			weight *= perturbedOut.getPdfAppliedBsdfCos();

			const auto Lp = perturbedOut.getL();

			// `Lp` is not shadowed
			if(sampleFlow.unflowedPick(G1(N, Np, Nt, Lp)))
			{
				out.setL(Lp);
				out.setPdfAppliedBsdfCos(weight, N.absDot(Lp));
			}
			// `Lp` is shadowed
			else
			{
				// Reflect on the tangent facet
				const auto Lt = Lp.reflect(Nt).safeNormalize(-Lp);

				weight *= G1(N, Np, Nt, Lt);

				out.setL(Lt);
				out.setPdfAppliedBsdfCos(weight, N.absDot(Lt));
			}
		}
	}
	// Sample the tangent facet
	else
	{
		const auto Lt = (-V).reflect(Nt).safeNormalize(V);

		BsdfSampleInput perturbedIn{};
		perturbedIn.set(perturbedX, -Lt);

		BsdfSampleOutput perturbedOut{};
		m_target->genBsdfSampleCore(ctx, perturbedIn, sampleFlow, perturbedOut);
		if(perturbedOut)
		{
			weight *= perturbedOut.getPdfAppliedBsdfCos();

			const auto Lp = perturbedOut.getL();

			weight *= G1(N, Np, Nt, Lp);

			out.setL(Lp);
			out.setPdfAppliedBsdfCos(weight, N.absDot(Lp));
		}
	}
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

	if(m_format == ENormalMapFormat::PXPYPZ_8Bits)
	{
		Np = Np * 2 - 1;
		
		// Swizzle into Photon's local space convention
		Np = {Np.y(), Np.z(), Np.x()};
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}

	// Renormalize local normal, in case they it is interpolated or not stored in unit length.
	// Some normal map also have quantization error and renormalization helps.
	Np = Np.safeNormalize({0, 1, 0});

	// To world space, with basically the TBN matrix commonly heard in real-time rendering
	Np = X.getDetail().getShadingBasis().localToWorld(Np);

	return Np;
}

}// end namespace ph
