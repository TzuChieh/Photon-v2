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
inline constexpr auto expected_freq_sample_count_multiplier = 16;

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

			const auto uv = math::TSphere<real>::makeUnit().surfaceToLatLong01(bsdfSample.outputs.getL());
			
			math::Vector2S phiThetaIdx{uv * math::Vector2R{phiThetaRes}};
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

	std::vector<real> sampleWeights(phiThetaRes.product(), 0.0_r);
	for(std::size_t thetaIdx = 0; thetaIdx < phiThetaRes.y(); ++thetaIdx)
	{
		if(isUpperHemisphereOnly && thetaIdx < phiThetaRes.y() / 2)
		{
			continue;
		}

		for(std::size_t phiIdx = 0; phiIdx < phiThetaRes.x(); ++phiIdx)
		{
			const auto binIdx = thetaIdx * phiThetaRes.x() + phiIdx;

			// Sample this bin multiple times
			for(uint64 si = 0; si < numSamplesPerBin; ++si)
			{
				const auto [dU, dV] = math::Random::sampleND<2>();
				const auto phiTheta = math::TSphere<real>::makeUnit().latLong01ToPhiTheta(
					{(phiIdx + dU) / phiThetaRes.x(), (thetaIdx + dV) / phiThetaRes.y()});
				const auto L = math::TSphere<real>::makeUnit().phiThetaToSurface(phiTheta);

				BsdfPdfQuery pdfQuery{make_query_ctx()};
				pdfQuery.inputs.set(X, L, V);
				scene.optics->calcBsdfPdf(pdfQuery);

				const real funcValue = pdfQuery.outputs ? pdfQuery.outputs.getSampleDirPdf().value : 0.0_r;
				sampleWeights[binIdx] += funcValue * std::sin(phiTheta.y()) / numSamplesPerBin;
			}
		}
	}

	math::TPiecewiseConstantDistribution2D<real> distribution{sampleWeights.data(), phiThetaRes};
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
	SGStratified sampleGen{numSamplesPerBin * expected_freq_sample_count_multiplier};
	const auto sampleHandle = sampleGen.declareStageND(2, phiThetaRes.product(), phiThetaRes.toVector());

	const math::TPiecewiseConstantDistribution2D<real> funcDistribution = make_PDF_distribution(
		scene,
		numSamplesPerBin * expected_freq_sample_count_multiplier,// for better quality table
		phiThetaRes * 4,                                         //
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

			real uvSamplePdf;
			const auto uv = funcDistribution.sampleContinuous(sampleFlow.flow2D(), &uvSamplePdf);
			const auto L = math::TSphere<real>::makeUnit().latLong01ToSurface(uv);
			const auto phiTheta = math::TSphere<real>::makeUnit().latLong01ToPhiTheta(uv);

			// UV PDF to solid angle PDF
			const real detJacobian = 2.0_r * math::constant::pi2<real> * std::sin(phiTheta.y());
			const real wSamplePdf = uvSamplePdf / detJacobian;

			math::Vector2S phiThetaIdx{uv * math::Vector2R{phiThetaRes}};
			phiThetaIdx.clampLocal({0, 0}, phiThetaRes - 1);
			const auto binIdx = phiThetaIdx.y() * phiThetaRes.x() + phiThetaIdx.x();
			PH_ASSERT_LT(binIdx, freqTable.size());

			BsdfPdfQuery pdfQuery{make_query_ctx()};
			pdfQuery.inputs.set(X, L, V);
			scene.optics->calcBsdfPdf(pdfQuery);

			// Sum for integrating PDF over the bin's solid angle
			const double funcSample = pdfQuery.outputs ? pdfQuery.outputs.getSampleDirPdf().value : 0.0_r;
			funcSampleSum[binIdx] += funcSample / wSamplePdf;
		}
	}

	const auto totalSamples = phiThetaRes.product() * numSamplesPerBin;
	for(std::size_t bi = 0; bi < freqTable.size(); ++bi)
	{
		// `totalSamples` actually canceled out; we keep it to show how probability is estimated
		const double binProbability = funcSampleSum[bi] / (totalSamples * expected_freq_sample_count_multiplier);
		const double estimatedFreq = binProbability * totalSamples;
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
<script src="https://cdn.plot.ly/plotly-3.1.0.min.js" charset="utf-8"></script>
</head>
<body>
<div id="heatmap" style="width:95vw; height:95vh;"></div>
<script>
)", testInfo);

	out.writeString("const W = {};\n", phi_res);
	out.writeString("const H = {};\n", theta_res);

	auto toJsArrayStr = [](const std::vector<double>& data)
	{
		std::string str = "[\n";
		for(std::size_t y = 0; y < theta_res; ++y)
		{
			str += "\t[";
			for(std::size_t x = 0; x < phi_res; ++x)
			{
				auto idx = static_cast<std::size_t>(y) * phi_res + x;
				str += std::to_string(data[idx]);
				if(x != phi_res - 1)
				{
					str += ", ";
				}
			}
			str += "]";
			if(y != theta_res - 1)
			{
				str += ",";
			}
			str += "\n";
		}
		str += "]";
		return str;
	};

	// Avoid outliers ruining the color scale by using the 95 percentile value as max
	double robustLegendMax = 0.0;
	{
		std::vector<double> allValues = observedFreq;
		allValues.insert(allValues.end(), expectedFreq.begin(), expectedFreq.end());

		const auto nthIdx = allValues.size() * 95 / 100;
		std::nth_element(allValues.begin(), allValues.begin() + nthIdx, allValues.end());
		robustLegendMax = allValues[nthIdx];
	}

	// `observed` array in JS
	out.writeString("const observed = \n{};\n", toJsArrayStr(observedFreq));

	// `expected` array in JS
	out.writeString("const expected = \n{};\n", toJsArrayStr(expectedFreq));

	// `diff = observed - expected`
	out.writeString("const diff = observed.map((row,y)=>row.map((v,x)=>v-expected[y][x]));\n");

	// JS code for subplots
	out.writeString(R"(
// Compute max absolute value for diff
let maxAbs = 0;
for (let y = 0; y < H; ++y)
{{
	for (let x = 0; x < W; ++x)
	{{
		const v = Math.abs(diff[y][x]);
		if (v > maxAbs) maxAbs = v;
	}}
}}

const data = 
[
	{{
		name: 'Observed',
		z: observed,
		type: 'heatmap',
		colorscale: 'Viridis',
		zmin: 0,
		zmax: {},  // robustMax
		visible: true,
		colorbar: {{ x: 1.02, y: 0.5 }}
	}},
	{{
		name: 'Expected',
		z: expected,
		type: 'heatmap',
		colorscale: 'Viridis',
		zmin: 0,
		zmax: {},  // robustMax
		visible: false,
		colorbar: {{ x: 1.02, y: 0.5 }}
	}},
	{{
		name: 'Diff',
		z: diff,
		type: 'heatmap',
		colorscale: 'RdBu',
		zmin: -maxAbs,
		zmax: maxAbs,
		visible: false,
		colorbar: {{ x: 1.02, y: 0.5 }}
	}}
];

const layout = 
{{
	title: {{ text: '{}', x: 0.5 }},
	margin: {{ t: 140, b: 60, l: 60, r: 140 }},
	yaxis: {{ scaleanchor: 'x' }},
	updatemenus: [
		{{
			type: 'buttons',
			x: 0.5,
			y: 1.08,  // below title
			xanchor: 'center',
			yanchor: 'top',
			showactive: true,
			direction: 'left',
			pad: {{ l: 10, r: 10, t: 10, b: 10 }},
			buttons: [
				{{
					label: 'Observed',
					method: 'update',
					args: [{{ visible: [true, false, false] }}]
				}},
				{{
					label: 'Expected',
					method: 'update',
					args: [{{ visible: [false, true, false] }}]
				}},
				{{
					label: 'Diff',
					method: 'update',
					args: [{{ visible: [false, false, true] }}]
				}}
			]
		}}
	]
}};

Plotly.newPlot('heatmap', data, layout);
)", robustLegendMax, robustLegendMax, testInfo);

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
			testInfo += "Rejected H0";
		}
		else
		{
			testInfo += "Accepted H0";
		}
		testInfo += std::format(
			" (p={}, significance={}, chi^2={}, DoF={}, SPP={})",
			pValue, alpha, x, dof, numSamples);

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
		16,
		true);
}
