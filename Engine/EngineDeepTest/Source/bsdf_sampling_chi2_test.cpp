/*!
Chi^2 test for BSDF sampling.

H0: There is no statistically significant difference between the observed frequencies
(`make_freq_table()`) and the expected frequencies (`make_integrated_freq_table()`).
HA: There is a statistically significant difference between the observed frequencies and
the expected frequencies.

`significance_level` controls the strictness of the test. A smaller value means a stricter test.
*/

#include "engine_deep_test_config.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Engine/Math/TVector2.h>
#include <Engine/Math/TVector3.h>
#include <Engine/Math/Geometry/TSphere.h>
#include <Engine/Math/Geometry/THemisphere.h>
#include <Engine/Math/statistics.h>
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
#include <Engine/ph_core.h>
#include <Engine/DataIO/FileSystem/Path.h>
#include <Engine/DataIO/FileSystem/Filesystem.h>
#include <Engine/DataIO/Stream/FormattedTextOutputStream.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <cmath>
#include <memory>
#include <utility>
#include <format>

using namespace ph;

namespace
{

inline constexpr auto significance_level = 0.01;
inline constexpr auto num_chi2_tests_per_suite = 1;
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
			
			math::Vector2S phiThetaIdx{phiTheta * math::Vector2R{phiThetaRes}};
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

			math::Vector2S phiThetaIdx{phiTheta * math::Vector2R{phiThetaRes}};
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

inline void write_report(
	const std::string& testName,
	const std::string& testInfo,
	const std::vector<double>& observedFreq,
	const std::vector<double>& expectedFreq)
{
	PH_ASSERT_EQ(observedFreq.size(), expectedFreq.size());

	const Path reportDir = get_intermediate_directory(EEngineProject::EngineDeepTest) / "bsdf_sampling_chi2";
	Filesystem::createDirectories(reportDir);

	FormattedTextOutputStream out(reportDir / (testName + ".html"));

	out.writeString(R"(
<!doctype html>
<html lang="en">
<head><meta charset="utf-8">
<title>{}</title>
<script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
</head>
<body>
<div id="heatmap" style="width:95vw; height:95vh;"></div>
<script>
)", testInfo);

	out.writeString("const W = {};\n", phi_res);
	out.writeString("const H = {};\n", theta_res);

	// `observed` array in JS
	out.writeString("const observed = \n[\n");
	for(std::size_t y = 0; y < theta_res; ++y)
	{
		out.writeString("\t[");
		for(std::size_t x = 0; x < phi_res; ++x)
		{
			auto idx = static_cast<std::size_t>(y) * phi_res + x;
			out.writeString("{}", observedFreq[idx]);
			if(x != phi_res - 1)
			{
				out.writeString(", ");
			}
		}
		out.writeString("]");
		if(y != theta_res - 1)
		{
			out.writeString(",");
		}
		out.writeNewLine();
	}
	out.writeString("];\n");

	// `expected` array in JS
	out.writeString("const expected = \n[\n");
	for(std::size_t y = 0; y < theta_res; ++y)
	{
		out.writeString("\t[");
		for(std::size_t x = 0; x < phi_res; ++x)
		{
			auto idx = static_cast<std::size_t>(y) * phi_res + x;
			out.writeString("{}", expectedFreq[idx]);
			if(x != phi_res - 1)
			{
				out.writeString(", ");
			}
		}
		out.writeString("]");
		if(y != theta_res - 1)
		{
			out.writeString(",");
		}
		out.writeNewLine();
	}
	out.writeString("];\n");

	// `diff = observed - expected`
	out.writeString("const diff = observed.map((row,y)=>row.map((v,x)=>v-expected[y][x]));\n");

	// JS code for subplots
	out.writeString(R"(
const data =
[
	{{ z: observed, type: 'heatmap', colorscale: 'Viridis', colorbar: {{ title: 'Observed' }}, xaxis: 'x', yaxis: 'y' }},
	{{ z: expected, type: 'heatmap', colorscale: 'Viridis', colorbar: {{ title: 'Expected' }}, xaxis: 'x2', yaxis: 'y2' }},
	{{ z: diff,     type: 'heatmap', colorscale: 'RdBu',   colorbar: {{ title: 'Diff' }}, xaxis: 'x3', yaxis: 'y3' }}
];

const layout =
{{
	title: {{ text: '{}', x:0.5 }},
	grid: {{ rows: 1, columns: 3, pattern: 'independent' }},
	margin: {{ t:50 }}
}};

Plotly.newPlot('heatmap', data, layout);
)", testInfo);

	out.writeString("</script>\n</body></html>");
}

inline void test_bsdf(
	std::unique_ptr<SurfaceOptics> targetOptics,
	const uint64 numSamples,
	const bool isUpperHemisphereOnly)
{
	FictionalScene scene = make_scene(std::move(targetOptics));

	const bool isDelta = scene.optics->getAllPhenomena().hasAny(ESurfacePhenomenon::Delta);
	PH_ASSERT(!isDelta);

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

		std::vector<std::size_t> poolingBuffer(freqTable.size());
		const auto [x, dof] = math::chi2<double, std::size_t>(
			freqTable,
			integratedFreqTable,
			1e-5 * numSamples * phi_res * theta_res,// small freq tolerance
			poolingBuffer);
		const double pValue = math::chi2_p_value(x, dof);
		const double alpha = math::sidak_correction(significance_level, num_chi2_tests_per_suite);
		
		std::string testInfo;
		if(pValue < alpha || !std::isfinite(pValue) || !std::isfinite(alpha))
		{
			testInfo += std::format("Rejected H0 with p={}, significance={}. ", pValue, alpha);
		}
		else
		{
			testInfo += "Accepted H0. ";
		}

		write_report(
			"ttt",
			testInfo,
			freqTable,
			integratedFreqTable);

		// TODO: write test report
		// TODO
	}

	// TODO

	//return result;
}

}// end namespace

TEST(BsdfSamplingChi2Test, ConstantLambertianReflector)
{
	test_bsdf(
		std::make_unique<LambertianReflector>(
			std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum{0.6_r})),
		3,
		true);
}
