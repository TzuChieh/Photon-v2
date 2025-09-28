#include "engine_deep_test_config.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Engine/Math/TVector2.h>
#include <Engine/Math/TVector3.h>
#include <Engine/Math/Geometry/TSphere.h>
#include <Engine/Math/Geometry/THemisphere.h>
#include <Engine/Math/Random/Random.h>
#include <Engine/Math/Function/Distribution/TPiecewiseConstantDistribution2D.h>
#include <Engine/Core/SampleGenerator/SGStratified.h>
#include <Engine/Core/SurfaceHit.h>
#include <Engine/Core/Intersection/PTriangle.h>
#include <Engine/Core/Intersection/PrimitiveMetadata.h>
#include <Engine/Core/Intersection/TMetaInjectionPrimitive.h>
#include <Engine/Core/SurfaceBehavior/SurfaceOptics/LambertianReflector.h>
#include <Engine/Core/SurfaceBehavior/BsdfSampleQuery.h>
#include <Engine/Core/SurfaceBehavior/BsdfPdfQuery.h>
#include <Engine/Core/Texture/constant_textures.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <cmath>
#include <memory>
#include <utility>

using namespace ph;

namespace
{

inline constexpr auto num_chi2_tests_per_suite = 5;
inline constexpr auto theta_res = 90;
inline constexpr auto phi_res = theta_res * 2;

struct FictionalScene
{
	std::unique_ptr<Primitive> unitObj;
	std::unique_ptr<SurfaceOptics> optics;
};

inline FictionalScene make_scene(std::unique_ptr<SurfaceOptics> targetOptics)
{
	PH_ASSERT(targetOptics);

	PrimitiveMetadata metadata;
	metadata.surface().setOptics(targetOptics.get());

	constexpr auto triSize = 1.0_r / 2;

	// Lying on xz-plane, so world space directions are also local space directions
	PTriangle triangle{
		{triSize, 0, -triSize},
		{-triSize, 0, -triSize},
		{0, 0, triSize}};
	triangle.setUVWa({1, 0, -1});
	triangle.setUVWb({-1, 0, -1});
	triangle.setUVWc({0, 0, 1});

	TMetaInjectionPrimitive metaPrimitive{
		EmbeddedPrimitiveMetaGetter{metadata},
		TEmbeddedPrimitiveGetter<PTriangle>{triangle}};

	FictionalScene scene;
	scene.unitObj = std::make_unique<decltype(metaPrimitive)>(metaPrimitive);
	scene.optics = std::move(targetOptics);
	return scene;
}

inline SurfaceHit make_hit(const FictionalScene& scene)
{
	PH_ASSERT(scene.unitObj);

	// Any ray that points to the origin
	const Ray ray{math::Vector3R{10}, math::Vector3R{-1}.normalize()};

	HitProbe probe;
	const bool hasHit = scene.unitObj->isIntersecting(ray, probe);
	PH_ASSERT(hasHit);

	SurfaceHit X{ray, probe, ESurfaceHitReason::IncidentRay};
	PH_ASSERT(&X.getSurfaceOptics() == scene.optics.get());
	return X;
}

inline BsdfQueryContext make_query_ctx()
{
	BsdfQueryContext queryCtx{
		ALL_SURFACE_PHENOMENA,
		lta::ETransport::Radiance,
		lta::ESidednessPolicy::Strict};

	// Random key as each query is independent
	queryCtx.key = BsdfKey::makeRandom();

	return queryCtx;
}

// Count number of sample directions in each bin
inline std::vector<double> make_freq_table(
	const FictionalScene& scene,
	const uint64 numSamplesPerBin,
	const math::Vector2S& phiThetaRes,
	const math::Vector3R& V)
{
	const SurfaceHit X = make_hit(scene);

	// Stratify with table resolution
	SGStratified sampleGen{numSamplesPerBin};
	const auto sampleHandle = sampleGen.declareStageND(2, phiThetaRes.product(), phiThetaRes.toVector());

	std::vector<double> freqTable(phiThetaRes.product(), 0.0);
	while(sampleGen.prepareSampleBatch())
	{
		auto sampleStream = sampleGen.getSamplesND(sampleHandle);

		// One sample per bin "on average"
		for(std::size_t si = 0; si < sampleStream.numSamples(); ++si)
		{
			auto sampleFlow = sampleStream.readSampleAsFlow();

			BsdfSampleQuery bsdfSample{make_query_ctx()};
			bsdfSample.inputs.set(X, V);
			scene.optics->genBsdfSample(bsdfSample, sampleFlow);
			if(!bsdfSample.outputs)
			{
				continue;
			}

			const auto phiTheta = math::TSphere<real>::makeUnit().surfaceToPhiTheta(bsdfSample.outputs.getL());
			
			math::Vector2S phiThetaIdx{phiTheta.x() * phiThetaRes.x(), phiTheta.y() * phiThetaRes.y()};
			phiThetaIdx.clampLocal({0, 0}, phiThetaRes - 1);

			const auto binIdx = phiThetaIdx.y() * phiThetaRes.x() + phiThetaIdx.x();
			PH_ASSERT_LT(binIdx, freqTable.size());
			freqTable[binIdx] += 1.0;
		}
	}

	return freqTable;
}

// Make a distribution for better sampling
inline math::TPiecewiseConstantDistribution2D<real> make_PDF_distribution(
	const FictionalScene& scene,
	const uint64 numSamplesPerBin,
	const math::Vector2S& phiThetaRes,
	const math::Vector3R& V,
	const bool isUpperHemisphereOnly)
{
	const SurfaceHit X = make_hit(scene);
	const real dTheta = math::constant::pi<real> / phiThetaRes.y();

	std::vector<real> sampleWeights(phiThetaRes.product(), 0.0_r);
	for(std::size_t thetaIdx = 0; thetaIdx < phiThetaRes.y(); ++thetaIdx)
	{
		if(isUpperHemisphereOnly && thetaIdx >= phiThetaRes.y() / 2)
		{
			break;
		}

		for(std::size_t phiIdx = 0; phiIdx < phiThetaRes.x(); ++phiIdx)
		{
			// Sample this bin multiple times
			for(uint64 si = 0; si < numSamplesPerBin; ++si)
			{
				auto [phi, theta] = math::Random::sampleND<2>();
				theta = (thetaIdx + theta) * (math::constant::pi<real> / phiThetaRes.y());
				phi = (phiIdx + phi) * (math::constant::two_pi<real> * std::sin(theta) / phiThetaRes.x());

				const auto L = math::TSphere<real>::makeUnit().phiThetaToSurface({phi, theta});

				BsdfPdfQuery pdfQuery{make_query_ctx()};
				pdfQuery.inputs.set(X, L, V);
				scene.optics->calcBsdfPdf(pdfQuery);

				const real funcValue = pdfQuery.outputs ? pdfQuery.outputs.getSampleDirPdf().value : 0.0_r;
				sampleWeights[thetaIdx * phiThetaRes.x() + phiIdx] += funcValue * std::sin(theta);
			}
		}
	}

	math::TPiecewiseConstantDistribution2D<real> distribution{
		math::TAABB2D<real>{{0, 0}, {math::constant::two_pi<real>, math::constant::pi<real>}},
		sampleWeights.data(),
		phiThetaRes};
	return distribution;
}

// Use Monte Carlo integration to get expected frequencies of each bin
inline std::vector<double> make_integrated_freq_table(
	const FictionalScene& scene,
	const uint64 numSamplesPerBin,
	const math::Vector2S& phiThetaRes,
	const math::Vector3R& V,
	const bool isUpperHemisphereOnly)
{
	const SurfaceHit X = make_hit(scene);

	// Stratify with table resolution
	SGStratified sampleGen{numSamplesPerBin};
	const auto sampleHandle = sampleGen.declareStageND(2, phiThetaRes.product(), phiThetaRes.toVector());

	const math::TPiecewiseConstantDistribution2D<real> pdfDistribution = make_PDF_distribution(
		scene,
		numSamplesPerBin,
		phiThetaRes,
		V,
		isUpperHemisphereOnly);

	std::vector<double> freqTable(phiThetaRes.product(), 0.0);
	std::vector<double> funcSampleSum(phiThetaRes.product(), 0.0);
	while(sampleGen.prepareSampleBatch())
	{
		auto sampleStream = sampleGen.getSamplesND(sampleHandle);

		// One sample per bin "on average"
		for(std::size_t si = 0; si < sampleStream.numSamples(); ++si)
		{
			auto sampleFlow = sampleStream.readSampleAsFlow();

			real pdfSample;
			const math::Vector2R phiTheta = pdfDistribution.sampleContinuous(math::Random::sampleND<2>(), &pdfSample);
			const math::Vector3R L = math::TSphere<real>::makeUnit().phiThetaToSurface(phiTheta);

			math::Vector2S phiThetaIdx{phiTheta.x() * phiThetaRes.x(), phiTheta.y() * phiThetaRes.y()};
			phiThetaIdx.clampLocal({0, 0}, phiThetaRes - 1);
			const auto binIdx = phiThetaIdx.y() * phiThetaRes.x() + phiThetaIdx.x();
			PH_ASSERT_LT(binIdx, freqTable.size());

			BsdfPdfQuery pdfQuery{make_query_ctx()};
			pdfQuery.inputs.set(X, L, V);
			scene.optics->calcBsdfPdf(pdfQuery);

			// Splitted sums for integrating PDF over the bin's solid angle
			const double funcSample = pdfQuery.outputs ? pdfQuery.outputs.getSampleDirPdf().value : 0.0_r;
			freqTable[binIdx] += 1.0;
			funcSampleSum[binIdx] += funcSample / pdfSample;
		}
	}

	for(std::size_t bi = 0; bi < freqTable.size(); ++bi)
	{
		const double binProbability = funcSampleSum[bi] / freqTable[bi];
		const double estimatedFreq = binProbability * phiThetaRes.product() * numSamplesPerBin;
		freqTable[bi] = std::isfinite(estimatedFreq) ? estimatedFreq : 0.0;
	}

	return freqTable;
}

inline void chi2_test(
	const std::vector<double>& observedFreqTable,
	const std::vector<double>& expectedFreqTable)
{
	PH_ASSERT_EQ(observedFreqTable.size(), expectedFreqTable.size());



	// TODO
}

inline void test_bsdf(
	std::unique_ptr<SurfaceOptics> targetOptics,
	const uint64 numSamples,
	const bool isUpperHemisphereOnly)
{
	FictionalScene scene = make_scene(std::move(targetOptics));

	const bool isDelta = scene.optics->getAllPhenomena().hasAny(ESurfacePhenomenon::Delta);

	for(std::size_t ti = 0; ti < num_chi2_tests_per_suite; ++ti)
	{
		math::Vector3R V = isUpperHemisphereOnly
			? math::THemisphere<real>::makeUnit().sampleToSurfaceCosThetaWeighted(math::Random::sampleND<2>())
			: math::TSphere<real>::makeUnit().sampleToSurfaceAbsCosThetaWeighted(math::Random::sampleND<2>());
		V.normalizeLocal();

		const std::vector<double> freqTable = make_freq_table(
			scene,
			numSamples,
			{phi_res, theta_res},
			V);

		const std::vector<double> integratedFreqTable = make_integrated_freq_table(
			scene,
			numSamples,
			{phi_res, theta_res},
			V,
			isUpperHemisphereOnly);

		chi2_test(freqTable, integratedFreqTable);
	}

	// TODO

	//return result;
}

}// end namespace

TEST(BsdfSamplingChi2Test, ConstantLambertianReflector)
{
	/*test_bsdf(
		std::make_unique<LambertianReflector>(
			std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum{0.6_r})));*/
}
