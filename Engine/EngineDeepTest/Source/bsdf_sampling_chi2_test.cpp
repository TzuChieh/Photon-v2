#include "engine_deep_test_config.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Engine/Utility/TSpan.h>
#include <Engine/Math/TVector2.h>
#include <Engine/Math/TVector3.h>
#include <Engine/Math/Geometry/TSphere.h>
#include <Engine/Core/SampleGenerator/SGStratified.h>
#include <Engine/Core/SurfaceHit.h>
#include <Engine/Core/Intersection/PTriangle.h>
#include <Engine/Core/Intersection/PrimitiveMetadata.h>
#include <Engine/Core/Intersection/TMetaInjectionPrimitive.h>
#include <Engine/Core/SurfaceBehavior/SurfaceOptics/LambertianReflector.h>
#include <Engine/Core/SurfaceBehavior/BsdfSampleQuery.h>

#include <gtest/gtest.h>

#include <vector>
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

	constexpr auto triSize = 100.0_r / 2;

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

inline SurfaceHit make_hit(const Primitive& unitObj, const Ray& ray)
{
	HitProbe probe;
	const bool hasHit = unitObj.isIntersecting(ray, probe);
	PH_ASSERT(hasHit);

	return SurfaceHit{ray, probe, SurfaceHitReason{ESurfaceHitReason::IncidentRay}};
}

inline std::vector<double> make_freq_table(
	const FictionalScene& scene,
	const uint64 numSamples,
	const math::Vector2S& phiThetaRes,
	const math::Vector3R& V)
{
	const SurfaceHit X = make_hit(*scene.unitObj, Ray{math::Vector3R{10}, math::Vector3R{-1}.normalize()});
	PH_ASSERT(&X.getSurfaceOptics() == scene.optics.get());

	BsdfQueryContext queryCtx{
		ALL_SURFACE_PHENOMENA,
		lta::ETransport::Radiance,
		lta::ESidednessPolicy::Strict};

	// Stratify with table resolution
	SGStratified sampleGen{numSamples};
	const auto sampleHandle = sampleGen.declareStageND(2, phiThetaRes.product(), phiThetaRes.toVector());

	std::vector<double> freqTable(phiThetaRes.product(), 0.0);
	while(sampleGen.prepareSampleBatch())
	{
		auto sampleStream = sampleGen.getSamplesND(sampleHandle);

		// Count number of sample directions in each bin 
		for(std::size_t si = 0; si < sampleStream.numSamples(); ++si)
		{
			auto sampleFlow = sampleStream.readSampleAsFlow();
			queryCtx.key = BsdfKey::makeRandom();

			BsdfSampleQuery bsdfSample{queryCtx};
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
			freqTable[binIdx] += 1;
		}
	}

	return freqTable;
}

inline std::vector<double> make_integrated_freq_table(
	const FictionalScene& scene,
	const uint64 numSamples,
	const math::Vector2S& phiThetaRes,
	const math::Vector3R& V)
{
	// TODO
}

inline void chi2_test(
	const TSpanView<double> freqTable)
{
	// TODO
}

inline void test_bsdf(
	const SurfaceOptics& optics)
{
	// TODO
}

}// end namespace


