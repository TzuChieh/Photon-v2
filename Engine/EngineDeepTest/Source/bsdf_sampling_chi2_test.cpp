#include "engine_deep_test_config.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Engine/Utility/TSpan.h>
#include <Engine/Math/TVector2.h>
#include <Engine/Math/TVector3.h>
#include <Engine/Math/Random/Pcg32.h>
#include <Engine/Core/SurfaceHit.h>
#include <Engine/Core/Intersection/PLatLong01Sphere.h>
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
inline constexpr auto use_random_seed = true;
inline constexpr auto random_seed = 0x255B1E86;

using RNG = math::Pcg32;

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

	TMetaInjectionPrimitive metaPrimitive{
		EmbeddedPrimitiveMetaGetter{metadata},
		TEmbeddedPrimitiveGetter<PLatLong01Sphere>{PLatLong01Sphere{0.5_r}}};

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

	return SurfaceHit{ray, probe, SurfaceHitReason{ESurfaceHitReason::IncidentRay};
}

inline std::vector<double> make_freq_table(
	const FictionalScene& scene,
	const uint64 numSamples,
	const math::Vector2S& phiThetaRes,
	const math::Vector3R& V,
	RNG& rng)
{
	const SurfaceHit X = make_hit(*scene.unitObj, Ray{math::Vector3R{10}, math::Vector3R{-1}.normalize()});

	BsdfQueryContext queryCtx{
		ALL_SURFACE_PHENOMENA,
		lta::ETransport::Radiance,
		lta::ESidednessPolicy::Strict};
	queryCtx.key = BsdfKey::makeSampled(rng.generateSample());

	std::vector<double> freqTable(phiThetaRes.product(), 0.0);
	for(uint64 si = 0; si < numSamples; ++si)
	{
		queryCtx.key = queryCtx.key.getNext(si);

		BsdfSampleQuery bsdfSample{queryCtx};
		bsdfSample.inputs.set(X, V);
		scene.optics->genBsdfSample(bsdfSample, );

		const math::Vector2R xi(rng.generateSample(), rng.generateSample());
		optics.sampleBsdf(sampleInput, xi, &sampleOutput);

		if(!sampleOutput.isContributable())
		{
			continue;
		}

		const math::Vector3R L = sampleOutput.getL();
		const real theta = std::acos(math::clamp(L.y, -1.0_r, 1.0_r));
		const real phi   = std::atan2(L.z, L.x) + math::pi<real>();

		const int32 thetaIdx = static_cast<int32>(theta / dTheta);
		const int32 phiIdx   = static_cast<int32>(phi   / dPhi);

		if(thetaIdx < 0 || thetaIdx >= phiThetaRes.y ||
		   phiIdx   < 0 || phiIdx   >= phiThetaRes.x)
		{
			continue;
		}

		const int32 binIdx = thetaIdx * phiThetaRes.x + phiIdx;
		freqTable[binIdx] += 1.0;
	}

	return freqTable;
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


