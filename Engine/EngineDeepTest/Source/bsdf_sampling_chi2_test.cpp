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
#include <Engine/Core/SampleGenerator/SGUniformRandom.h>
#include <Engine/Core/SampleGenerator/SGHalton.h>
#include <Engine/Core/SurfaceHit.h>
#include <Engine/Core/Intersection/PTriangle.h>
#include <Engine/Core/Intersection/PrimitiveMetadata.h>
#include <Engine/Core/Intersection/TMetaInjectionPrimitive.h>
#include <Engine/Core/Texture/constant_textures.h>
#include <Engine/ph_core.h>
#include <Engine/DataIO/FileSystem/Path.h>
#include <Engine/DataIO/FileSystem/Filesystem.h>
#include <Engine/DataIO/Stream/FormattedTextOutputStream.h>

// Optics to test
#include <Engine/Core/SurfaceBehavior/BsdfSampleQuery.h>
#include <Engine/Core/SurfaceBehavior/BsdfPdfQuery.h>
#include <Engine/Core/SurfaceBehavior/Property/SchlickApproxConductorFresnel.h>
#include <Engine/Core/SurfaceBehavior/Property/ExactConductorFresnel.h>
#include <Engine/Core/SurfaceBehavior/Property/SchlickApproxDielectricFresnel.h>
#include <Engine/Core/SurfaceBehavior/Property/IsoTrowbridgeReitzConstant.h>
#include <Engine/Core/SurfaceBehavior/Property/IsoBeckmann.h>
#include <Engine/Core/SurfaceBehavior/Property/AnisoTrowbridgeReitz.h>
#include <Engine/Core/SurfaceBehavior/SurfaceOptics/LambertianReflector.h>
#include <Engine/Core/SurfaceBehavior/SurfaceOptics/OrenNayar.h>
#include <Engine/Core/SurfaceBehavior/SurfaceOptics/OpaqueMicrofacet.h>
#include <Engine/Core/SurfaceBehavior/SurfaceOptics/TranslucentMicrofacet.h>
#include <Engine/Core/SurfaceBehavior/SurfaceOptics/LerpedSurfaceOptics.h>
#include <Engine/Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayeredSurface.h>
#include <Engine/Core/SurfaceBehavior/SurfaceOptics/MicrofacetNormalMapper.h>

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

// Bins used for theta and phi axes
inline constexpr auto theta_res = 90;
inline constexpr auto phi_res = theta_res * 2;

// Extra computation spent on calculating expected frequency;
// more frequency samples need better ground truth to verify against
inline constexpr auto expected_freq_sample_count_multiplier = 32;

inline constexpr bool report_accepted_tests = true;

struct FictionalScene
{
	std::unique_ptr<Primitive> unitObj;
	std::unique_ptr<SurfaceOptics> optics;
};

struct BsdfTestInput
{
	std::string testName = "";
	std::unique_ptr<SurfaceOptics> targetOptics;
	uint64 numSamples = 16;
	bool viewFromUpperHemisphereOnly = true;
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

	SGUniformRandom sampleGen{numSamplesPerBin};
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
	const math::Vector3R& V)
{
	const SurfaceHit X = make_hit(scene);

	std::vector<real> sampleWeights(phiThetaRes.product(), 0.0_r);
	for(std::size_t thetaIdx = 0; thetaIdx < phiThetaRes.y(); ++thetaIdx)
	{
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
	const math::Vector3R& V)
{
	const SurfaceHit X = make_hit(scene);

	SGHalton sampleGen{
		numSamplesPerBin * expected_freq_sample_count_multiplier,
		EHaltonPermutation::Owen,
		EHaltonSequence::Original};
	const auto sampleHandle = sampleGen.declareStageND(2, phiThetaRes.product(), phiThetaRes.toVector());

	const math::TPiecewiseConstantDistribution2D<real> funcDistribution = make_PDF_distribution(
		scene,
		numSamplesPerBin * expected_freq_sample_count_multiplier,// for better quality table
		phiThetaRes * 4,                                         //
		V);

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
	const std::string& reportName,
	const std::string& plotTitle,
	const std::string& plotInfo,
	const std::vector<double>& observedFreq,
	const std::vector<double>& expectedFreq)
{
	PH_ASSERT_EQ(observedFreq.size(), expectedFreq.size());

	const Path reportDir = get_intermediate_directory(EEngineProject::EngineDeepTest) / "bsdf_sampling_chi2";
	Filesystem::createDirectories(reportDir);

	FormattedTextOutputStream out{reportDir / (reportName + ".html")};

	// Use plotly.js for interactive plots, with buttons for mode switching
	out.writeString(R"html(
<!doctype html>
<html lang="en">
<head><meta charset="utf-8">
<title>{}</title>
<script src="https://cdn.plot.ly/plotly-3.1.0.min.js" charset="utf-8"></script>
<style>
	body {{
		margin: 0;
		background-color: #111;
		color: #eee;
		font-family: "Segoe UI", Roboto, sans-serif;
		text-align: center;
	}}
	#button-bar {{
		margin: 12px 0;
	}}
	#button-bar button {{
		padding: 8px 18px;
		margin: 0 8px;
		font-size: 15px;
		cursor: pointer;
		border: 1px solid #444;
		border-radius: 6px;
		background: #222;
		color: #aaa;
		transition: all 0.2s ease;
	}}
	#button-bar button:hover {{
		background: #333;
		color: #fff;
	}}
	#button-bar button.active {{
		background: #06a;
		color: #fff;
		border-color: #06a;
	}}
	#heatmap {{
		width: 95vw;
		height: 88vh;
		margin: auto;
		border-radius: 8px;
		box-shadow: 0 0 16px rgba(0,0,0,0.6);
		background: #111;
	}}
</style>
</head>
<body>
<div id="button-bar"></div>
<div id="heatmap"></div>
<script>
)html", reportName);

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
				auto idx = y * phi_res + x;
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
	out.writeString(R"html(
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
		colorbar: {{ x: 1.02, y: 0.5 }},
		hovertemplate: '<span style="font-family: monospace">x     = %{{x}}<br>y     = %{{y}}<br>value = %{{z}}</span><extra></extra>'
	}},
	{{
		name: 'Expected',
		z: expected,
		type: 'heatmap',
		colorscale: 'Viridis',
		zmin: 0,
		zmax: {},  // robustMax
		visible: false,
		colorbar: {{ x: 1.02, y: 0.5 }},
		hovertemplate: '<span style="font-family: monospace">x     = %{{x}}<br>y     = %{{y}}<br>value = %{{z}}</span><extra></extra>'
	}},
	{{
		name: 'Diff',
		z: diff,
		type: 'heatmap',
		colorscale: 'RdBu',
		zmin: -maxAbs,
		zmax: maxAbs,
		visible: false,
		colorbar: {{ x: 1.02, y: 0.5 }},
		hovertemplate: '<span style="font-family: monospace">x     = %{{x}}<br>y     = %{{y}}<br>value = %{{z}}</span><extra></extra>'
	}}
];

const layout = 
{{
	title: {{ text: '{}<br><sub>{}</sub>', x: 0.5 }},
	margin: {{ t: 160, b: 80, l: 80, r: 140 }},
	xaxis: {{
		title: 'Phi Index',
		type: 'linear',
		automargin: true,
		scaleanchor: 'y'
	}},
	yaxis: {{
		title: 'Theta Index',
		type: 'linear',
		automargin: true,
		scaleanchor: 'x'
	}}
}};

Plotly.newPlot('heatmap', data, layout);

// === Custom button bar ===
const buttons = ['Observed', 'Expected', 'Diff'];
let active = 0;
const buttonBar = document.getElementById('button-bar');

buttons.forEach((label, i) => {{
	const btn = document.createElement('button');
	btn.textContent = label;
	if (i === active) btn.classList.add('active');
	btn.onclick = () => {{
		// Update visibility
		const vis = [false, false, false];
		vis[i] = true;
		Plotly.restyle('heatmap', {{ visible: vis }});

		// Update button styles
		Array.from(buttonBar.children).forEach((b, j) => {{
			b.classList.toggle('active', j === i);
		}});
		active = i;
	}};
	buttonBar.appendChild(btn);
}});
)html", robustLegendMax, robustLegendMax, plotTitle, plotInfo);

	out.writeString("</script>\n</body></html>");
}

inline void test_bsdf(BsdfTestInput p)
{
	FictionalScene scene = make_scene(std::move(p.targetOptics));

	const bool isDelta = scene.optics->getAllPhenomena().hasAny(ESurfacePhenomenon::Delta);
	PH_ASSERT(!isDelta);

	for(std::size_t ti = 0; ti < num_chi2_tests_per_suite; ++ti)
	{
		math::Vector3R V = p.viewFromUpperHemisphereOnly
			? math::THemisphere<real>::makeUnit().sampleToSurfaceCosThetaWeighted(math::Random::sampleND<2>())
			: math::TSphere<real>::makeUnit().sampleToSurfaceAbsCosThetaWeighted(math::Random::sampleND<2>());
		V.normalizeLocal();

		const std::vector<double> freqTable = make_freq_table(
			scene,
			p.numSamples,
			{phi_res, theta_res},
			V);

		const std::vector<double> integratedFreqTable = make_integrated_freq_table(
			scene,
			p.numSamples,
			{phi_res, theta_res},
			V);

		std::vector<std::size_t> poolingBuffer(freqTable.size());
		const auto [x, dof] = math::chi2<double, std::size_t>(
			freqTable,
			integratedFreqTable,
			1e-5 * p.numSamples * phi_res * theta_res,// small freq tolerance
			poolingBuffer);
		const double pValue = math::chi2_p_value(x, dof);
		const double alpha = math::sidak_correction(significance_level, num_chi2_tests_per_suite);
		
		bool isAccepted = false;
		bool needReport = false;
		if(pValue < alpha || !std::isfinite(pValue) || !std::isfinite(alpha))
		{
			isAccepted = false;
			needReport = true;
		}
		else
		{
			isAccepted = true;
			needReport = false;
		}
		needReport = needReport || report_accepted_tests;

		if(needReport)
		{
			std::string testInfo = std::format(
				"p={}, significance={}, chi^2={}, DoF={}, SPP={}, V={}",
				pValue, alpha, x, dof, p.numSamples, V);

			write_report(
				p.testName + "_" + std::to_string(ti),
				p.testName + " (" + (isAccepted ? "Accepted H0" : "Rejected H0") + ")",
				testInfo,
				freqTable,
				integratedFreqTable);
		}

		EXPECT_TRUE(isAccepted);
	}
}

}// end namespace

TEST(BsdfSamplingChi2Test, LambertianReflector)
{
	BsdfTestInput p
	{
		.testName = "LambertianReflector",
		.targetOptics = std::make_unique<LambertianReflector>(
			std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum{0.6_r})),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = true
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, GgxSchlickConductorSmoothReflector)
{
	BsdfTestInput p
	{
		.testName = "GgxSchlickConductorSmoothReflector",
		.targetOptics = std::make_unique<OpaqueMicrofacet>(
			std::make_shared<SchlickApproxConductorFresnel>(math::Spectrum{1}),
			std::make_shared<IsoTrowbridgeReitzConstant>(0.0_r, EMaskingShadowing::HightCorrelated)),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = true
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, GgxSchlickConductorGlossyReflector)
{
	BsdfTestInput p
	{
		.testName = "GgxSchlickConductorGlossyReflector",
		.targetOptics = std::make_unique<OpaqueMicrofacet>(
			std::make_shared<SchlickApproxConductorFresnel>(math::Spectrum{1}),
			std::make_shared<IsoTrowbridgeReitzConstant>(0.5_r, EMaskingShadowing::HightCorrelated)),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = true
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, GgxSchlickConductorRoughReflector)
{
	BsdfTestInput p
	{
		.testName = "GgxSchlickConductorRoughReflector",
		.targetOptics = std::make_unique<OpaqueMicrofacet>(
			std::make_shared<SchlickApproxConductorFresnel>(math::Spectrum{1}),
			std::make_shared<IsoTrowbridgeReitzConstant>(1.0_r, EMaskingShadowing::HightCorrelated)),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = true
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, GgxSchlickConductorRougherReflector)
{
	BsdfTestInput p
	{
		.testName = "GgxSchlickConductorRougherReflector",
		.targetOptics = std::make_unique<OpaqueMicrofacet>(
			std::make_shared<SchlickApproxConductorFresnel>(math::Spectrum{1}),
			std::make_shared<IsoTrowbridgeReitzConstant>(2.0_r, EMaskingShadowing::HightCorrelated)),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = true
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, GgxSchlickSmoothDielectric)
{
	BsdfTestInput p
	{
		.testName = "GgxSchlickSmoothDielectric",
		.targetOptics = std::make_unique<TranslucentMicrofacet>(
			std::make_shared<SchlickApproxDielectricFresnel>(1.0_r, 1.5_r),
			std::make_shared<IsoTrowbridgeReitzConstant>(0.0_r, EMaskingShadowing::HightCorrelated)),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = false
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, GgxSchlickGlossyDielectric)
{
	BsdfTestInput p
	{
		.testName = "GgxSchlickGlossyDielectric",
		.targetOptics = std::make_unique<TranslucentMicrofacet>(
			std::make_shared<SchlickApproxDielectricFresnel>(1.0_r, 1.5_r),
			std::make_shared<IsoTrowbridgeReitzConstant>(0.5_r, EMaskingShadowing::HightCorrelated)),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = false
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, GgxSchlickRoughDielectric)
{
	BsdfTestInput p
	{
		.testName = "GgxSchlickRoughDielectric",
		.targetOptics = std::make_unique<TranslucentMicrofacet>(
			std::make_shared<SchlickApproxDielectricFresnel>(1.0_r, 1.5_r),
			std::make_shared<IsoTrowbridgeReitzConstant>(1.0_r, EMaskingShadowing::HightCorrelated)),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = false
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, GgxSchlickRougherDielectric)
{
	BsdfTestInput p
	{
		.testName = "GgxSchlickRougherDielectric",
		.targetOptics = std::make_unique<TranslucentMicrofacet>(
			std::make_shared<SchlickApproxDielectricFresnel>(1.0_r, 1.5_r),
			std::make_shared<IsoTrowbridgeReitzConstant>(2.0_r, EMaskingShadowing::HightCorrelated)),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = false
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, BeckmannExactConductorRoughReflector)
{
	BsdfTestInput p
	{
		.testName = "BeckmannExactConductorRoughReflector",
		.targetOptics = std::make_unique<OpaqueMicrofacet>(
			std::make_shared<ExactConductorFresnel>(
				1.0_r,
				// Aluminum complex IoR from https://chris.hindefjord.se/resources/rgb-ior-metals/
				math::Spectrum{}.setLinearSRGB({1.34560_r, 0.96521_r, 0.61722_r}, math::EColorUsage::Raw),
				math::Spectrum{}.setLinearSRGB({7.47460_r, 6.39950_r, 5.30310_r}, math::EColorUsage::Raw)),
			std::make_shared<IsoBeckmann>(0.4_r, EMaskingShadowing::Separable)),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = true
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, AnisoGgxExactConductorRoughReflector)
{
	BsdfTestInput p
	{
		.testName = "AnisoGgxExactConductorRoughReflector",
		.targetOptics = std::make_unique<OpaqueMicrofacet>(
			std::make_shared<ExactConductorFresnel>(
				1.33_r,
				// Aluminum complex IoR from https://chris.hindefjord.se/resources/rgb-ior-metals/
				math::Spectrum{}.setLinearSRGB({1.34560_r, 0.96521_r, 0.61722_r}, math::EColorUsage::Raw),
				math::Spectrum{}.setLinearSRGB({7.47460_r, 6.39950_r, 5.30310_r}, math::EColorUsage::Raw)),
			std::make_shared<AnisoTrowbridgeReitz>(0.66_r, 0.03_r, EMaskingShadowing::DirectionCorrelated)),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = true
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, OrenNayarZeroSigma)
{
	BsdfTestInput p
	{
		.testName = "OrenNayarZeroSigma",
		.targetOptics = std::make_unique<OrenNayar>(
			std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum{0.8_r}),
			0.0_r),// 0 sigma is effectively Lambertian
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = true
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, OrenNayar60Degrees)
{
	BsdfTestInput p
	{
		.testName = "OrenNayar60Degrees",
		.targetOptics = std::make_unique<OrenNayar>(
			std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum{0.8_r}),
			60.0_r),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = true
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, LerpedDiffuseAndGlossyReflector)
{
	const auto diffuse = std::make_unique<LambertianReflector>(
		std::make_shared<TConstantTexture<math::Spectrum>>(math::Spectrum{0.3_r}));
	const auto glossy = std::make_unique<OpaqueMicrofacet>(
		std::make_shared<ExactConductorFresnel>(
			1.0_r,
			// Gold complex IoR from https://chris.hindefjord.se/resources/rgb-ior-metals/
			math::Spectrum{}.setLinearSRGB({0.18299_r, 0.42108_r, 1.37340_r}, math::EColorUsage::Raw),
			math::Spectrum{}.setLinearSRGB({3.42420_r, 2.34590_r, 1.77040_r}, math::EColorUsage::Raw)),
		std::make_shared<IsoTrowbridgeReitzConstant>(0.2_r, EMaskingShadowing::HeightDirectionCorrelated));

	BsdfTestInput p
	{
		.testName = "LerpedDiffuseAndGlossyReflector",
		.targetOptics = std::make_unique<LerpedSurfaceOptics>(
			diffuse.get(),
			glossy.get()),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = true
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, LaurentBelcourLayeredSurfaceReflector)
{
	// Layered parameters from Laurent Belcour's paper
	// "Efficient rendering of layered materials using an atomic decomposition with statistical operators"
	// https://dl.acm.org/doi/10.1145/3197517.3201289

	BsdfTestInput p
	{
		.testName = "LaurentBelcourLayeredSurfaceReflector",
		.targetOptics = std::make_unique<LbLayeredSurface>(
			// IoR N
			std::vector<math::Spectrum>
			{
				math::Spectrum{}.setLinearSRGB({1.4_r, 1.4_r, 1.4_r}, math::EColorUsage::Raw),
				math::Spectrum{}.setLinearSRGB({1.0_r, 1.0_r, 1.0_r}, math::EColorUsage::Raw)
			},
			// IoR K
			std::vector<math::Spectrum>
			{
				math::Spectrum{}.setLinearSRGB({0.0_r, 0.0_r, 0.0_r}, math::EColorUsage::Raw),
				math::Spectrum{}.setLinearSRGB({0.8_r, 0.9_r, 0.6_r}, math::EColorUsage::Raw)
			},
			// alpha
			std::vector<real>
			{
				0.01_r,
				0.1_r
			},
			// depth
			std::vector<real>
			{
				0.0_r,
				0.0_r
			},
			// phase function g
			std::vector<real>
			{
				0.0_r,
				0.0_r
			},
			// sigma A
			std::vector<math::Spectrum>
			{
				math::Spectrum{},
				math::Spectrum{}
			},
			// sigma S
			std::vector<math::Spectrum>
			{
				math::Spectrum{},
				math::Spectrum{}
			}),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = true
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, IdentityMicrofacetNormalMapperWithGgx0p15Reflector)
{
	const auto glossy = std::make_unique<OpaqueMicrofacet>(
		std::make_shared<ExactConductorFresnel>(1.05_r, math::Spectrum{1.45_r}, math::Spectrum{0.0_r}),
		std::make_shared<IsoTrowbridgeReitzConstant>(0.15_r, EMaskingShadowing::HightCorrelated));

	// Always point upward (OpenGL style) and compressed to [0, 1]
	math::Vector3R zUpConstantNormalMap{0.0_r, 0.0_r, 1.0_r};
	zUpConstantNormalMap = zUpConstantNormalMap * 0.5_r + 0.5_r;

	BsdfTestInput p
	{
		.testName = "IdentityMicrofacetNormalMapperWithGgx0p15Reflector",
		.targetOptics = std::make_unique<MicrofacetNormalMapper>(
			glossy.get(),
			std::make_shared<TConstantTexture<math::Vector3R>>(zUpConstantNormalMap)),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = true
	};

	test_bsdf(std::move(p));
}

TEST(BsdfSamplingChi2Test, MicrofacetNormalMapperWithGgx0p15Reflector)
{
	const auto glossy = std::make_unique<OpaqueMicrofacet>(
		std::make_shared<ExactConductorFresnel>(1.05_r, math::Spectrum{1.45_r}, math::Spectrum{0.0_r}),
		std::make_shared<IsoTrowbridgeReitzConstant>(0.15_r, EMaskingShadowing::HightCorrelated));

	// Tilted back-leftward (OpenGL style) and compressed to [0, 1]
	math::Vector3R tiltedConstantNormalMap{-1.0_r, -1.0_r, 1.0_r};
	tiltedConstantNormalMap.normalizeLocal();
	tiltedConstantNormalMap = tiltedConstantNormalMap * 0.5_r + 0.5_r;

	BsdfTestInput p
	{
		.testName = "MicrofacetNormalMapperWithGgx0p15Reflector",
		.targetOptics = std::make_unique<MicrofacetNormalMapper>(
			glossy.get(),
			std::make_shared<TConstantTexture<math::Vector3R>>(tiltedConstantNormalMap)),
		.numSamples = 16,
		.viewFromUpperHemisphereOnly = true
	};

	test_bsdf(std::move(p));
}

// TODO: lerped reflector + dielectric
// TODO: lerped dielectric + dielectric
// TODO: lerp with delta?
// TODO: custom context like selected phenomena
// TODO: BSDF/PDF should == BSDF sample weight
