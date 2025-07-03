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

inline real heaviside(const real x)
{
	return x >= 0 ? 1.0_r : 0.0_r;
}

template<bool IS_PERTURBED_FACET>
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

	real heavisideTerm;
	if constexpr(IS_PERTURBED_FACET)
	{
		// To support transparent perturbed facet, we need to allow transmission
		heavisideTerm = 1;
	}
	else
	{
		heavisideTerm = heaviside(Nt.dot(L));
	}

	const real NgDotNp2 = std::min(math::squared(Ng.dot(Np)), 1.0_r);
	const real sinNgDotNp = std::sqrt(1 - NgDotNp2);
	return heavisideTerm * math::safe_clamp(
		(positiveDot(Ng, L) * positiveDot(Np, L)) / (positiveDot(Np, L) + positiveDot(Nt, L) * sinNgDotNp),
		0.0_r,
		1.0_r);
}

inline real G1OfPerturbedFacet(
	const math::Vector3R& Ng,
	const math::Vector3R& Np,
	const math::Vector3R& Nt,
	const math::Vector3R& L)
{
	return G1<true>(Ng, Np, Nt, L);
}

inline real G1OfTangentFacet(
	const math::Vector3R& Ng,
	const math::Vector3R& Np,
	const math::Vector3R& Nt,
	const math::Vector3R& L)
{
	return G1<false>(Ng, Np, Nt, L);
}

template<bool IS_PERTURBED_FACET>
inline real lambda(
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

	const real NgDotNp2 = std::min(math::squared(Ng.dot(Np)), 1.0_r);
	const real sinNgDotNp = std::sqrt(1 - NgDotNp2);
	const real lambdaP = math::safe_clamp(
		positiveDot(Np, L) / (positiveDot(Np, L) + positiveDot(Nt, L) * sinNgDotNp),
		0.0_r, 
		1.0_r);

	if constexpr(IS_PERTURBED_FACET)
	{
		return lambdaP;
	}
	else
	{
		return 1 - lambdaP;
	}
}

inline real lambdaOfPerturbedFacet(
	const math::Vector3R& Ng,
	const math::Vector3R& Np,
	const math::Vector3R& Nt,
	const math::Vector3R& L)
{
	return lambda<true>(Ng, Np, Nt, L);
}

inline real lambdaOfTangentFacet(
	const math::Vector3R& Ng,
	const math::Vector3R& Np,
	const math::Vector3R& Nt,
	const math::Vector3R& L)
{
	return lambda<false>(Ng, Np, Nt, L);
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
	const auto N = in.getX().getShadingNormal();
	const auto Np = samplePerturbedNormal(in.getX());

	if(isPerturbationTooSmall(N.absDot(Np)))
	{
		m_target->calcBsdfCore(ctx, in, out);
		return;
	}

	out.setContributability(false);

	const auto Nt = tangentFacetNormal(N, Np);
	const real lambdaP = lambdaOfPerturbedFacet(N, Np, Nt, in.getV());
	const real G1PForL = G1OfPerturbedFacet(N, Np, Nt, in.getL());
	const SurfaceHit perturbedX = perturbX(in.getX(), N, Np);

	math::Spectrum weight(0);

	// Case i -> p -> o
	{
		BsdfEvalInput perturbedIn{};
		perturbedIn.set(perturbedX, in.getL(), in.getV());

		BsdfEvalOutput perturbedOut{};
		m_target->calcBsdfCore(ctx, perturbedIn, perturbedOut);
		if(perturbedOut)
		{
			weight += perturbedOut.getBsdf() * (Np.absDot(in.getL()) * lambdaP * G1PForL);
		}
	}

	// Case i -> p -> t -> o; cannot exit from backface 
	if(in.getL().dot(Nt) > 0)
	{
		// Reflect on the tangent facet (saved some negation here)
		const auto Lp = in.getL().reflect(Nt).safeNormalize(N);

		BsdfEvalInput perturbedIn{};
		perturbedIn.set(perturbedX, Lp, in.getV());

		BsdfEvalOutput perturbedOut{};
		m_target->calcBsdfCore(ctx, perturbedIn, perturbedOut);
		if(perturbedOut)
		{
			const real G1PForLp = G1OfPerturbedFacet(N, Np, Nt, Lp);
			const real G1TForL = G1OfTangentFacet(N, Np, Nt, in.getL());
			weight += perturbedOut.getBsdf() * (Np.absDot(Lp) * lambdaP * (1 - G1PForLp) * G1TForL);
		}
	}

	// Case i -> t -> p -> o; cannot enter from backface
	if(in.getV().dot(Nt) > 0)
	{
		// Reflect on the tangent facet (saved some negation here)
		const auto Vp = in.getV().reflect(Nt).safeNormalize(N);

		BsdfEvalInput perturbedIn{};
		perturbedIn.set(perturbedX, in.getL(), Vp);

		BsdfEvalOutput perturbedOut{};
		m_target->calcBsdfCore(ctx, perturbedIn, perturbedOut);
		if(perturbedOut)
		{
			weight += perturbedOut.getBsdf() * (Np.absDot(in.getL()) * (1 - lambdaP) * G1PForL);
		}
	}

	out.setBsdf(weight / N.absDot(in.getL()));
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

	math::Spectrum weight(1);

	// Sample the perturbed facet
	if(sampleFlow.unflowedPick(lambdaOfPerturbedFacet(N, Np, Nt, V)))
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
			if(sampleFlow.unflowedPick(G1OfPerturbedFacet(N, Np, Nt, Lp)))
			{
				// Case i -> p -> o
				out.setL(Lp);
				out.setPdfAppliedBsdfCos(weight, N.absDot(Lp));
			}
			// `Lp` is shadowed
			else
			{
				// Reflect on the tangent facet
				const auto Lt = Lp.reflect(Nt).safeNormalize(N);

				weight *= G1OfTangentFacet(N, Np, Nt, Lt);

				// Case i -> p -> t -> o
				out.setL(Lt);
				out.setPdfAppliedBsdfCos(weight, N.absDot(Lt));
			}
		}
	}
	// Sample the tangent facet
	else
	{
		// Reflect on the tangent facet (saved some negation here)
		const auto Vp = (V).reflect(Nt).safeNormalize(N);

		BsdfSampleInput perturbedIn{};
		perturbedIn.set(perturbedX, Vp);

		BsdfSampleOutput perturbedOut{};
		m_target->genBsdfSampleCore(ctx, perturbedIn, sampleFlow, perturbedOut);
		if(perturbedOut)
		{
			weight *= perturbedOut.getPdfAppliedBsdfCos();

			const auto Lp = perturbedOut.getL();

			weight *= G1OfPerturbedFacet(N, Np, Nt, Lp);

			// Case i -> t -> p -> o
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
	const auto N = in.getX().getShadingNormal();
	const auto Np = samplePerturbedNormal(in.getX());

	if(isPerturbationTooSmall(N.absDot(Np)))
	{
		m_target->calcBsdfPdfCore(ctx, in, out);
		return;
	}

	out.setSampleDirPdf({});

	const auto Nt = tangentFacetNormal(N, Np);
	const real lambdaP = lambdaOfPerturbedFacet(N, Np, Nt, in.getV());
	const real G1PForL = G1OfPerturbedFacet(N, Np, Nt, in.getL());
	const SurfaceHit perturbedX = perturbX(in.getX(), N, Np);

	real pdfW = 0;

	// Case i -> p -> o
	{
		BsdfPdfInput perturbedIn{};
		perturbedIn.set(perturbedX, in.getL(), in.getV());

		BsdfPdfOutput perturbedOut{};
		m_target->calcBsdfPdfCore(ctx, perturbedIn, perturbedOut);
		if(perturbedOut)
		{
			pdfW += perturbedOut.getSampleDirPdfW() * (lambdaP * G1PForL);
		}
	}

	// Case i -> p -> t -> o; cannot exit from backface 
	if(in.getL().dot(Nt) > 0)
	{
		// Reflect on the tangent facet (saved some negation here)
		const auto Lp = in.getL().reflect(Nt).safeNormalize(N);

		BsdfPdfInput perturbedIn{};
		perturbedIn.set(perturbedX, Lp, in.getV());

		BsdfPdfOutput perturbedOut{};
		m_target->calcBsdfPdfCore(ctx, perturbedIn, perturbedOut);
		if(perturbedOut)
		{
			const real G1ForLp = G1OfPerturbedFacet(N, Np, Nt, Lp);
			pdfW += perturbedOut.getSampleDirPdfW() * (lambdaP * (1 - G1ForLp));
		}
	}

	// Case i -> t -> p -> o; cannot enter from backface
	if(in.getV().dot(Nt) > 0)
	{
		// Reflect on the tangent facet (saved some negation here)
		const auto Vp = in.getV().reflect(Nt).safeNormalize(N);

		BsdfPdfInput perturbedIn{};
		perturbedIn.set(perturbedX, in.getL(), Vp);

		BsdfPdfOutput perturbedOut{};
		m_target->calcBsdfPdfCore(ctx, perturbedIn, perturbedOut);
		if(perturbedOut)
		{
			pdfW += perturbedOut.getSampleDirPdfW() * (1 - lambdaP);
		}
	}

	out.setSampleDirPdf(lta::PDF::W(pdfW));
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
