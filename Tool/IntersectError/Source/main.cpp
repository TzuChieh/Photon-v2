#include <ph_core.h>
#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/Utility/string_utils.h>
#include <Core/Intersection/PTriangle.h>
#include <Core/Intersection/PLatLong01Sphere.h>
#include <Core/Intersection/TransformedIntersectable.h>
#include <Core/Ray.h>
#include <Core/HitProbe.h>
#include <Core/HitDetail.h>
#include <Math/math.h>
#include <Math/TVector3.h>
#include <Math/TQuaternion.h>
#include <Math/TMatrix4.h>
#include <Math/Random/Pcg32.h>
#include <Math/Random/DeterministicSeeder.h>
#include <Math/Geometry/TSphere.h>
#include <Math/Geometry/TTriangle.h>
#include <Math/Geometry/THemisphere.h>
#include <Math/TOrthonormalBasis3.h>
#include <Math/Transform/TDecomposedTransform.h>
#include <Math/Transform/StaticAffineTransform.h>
#include <DataIO/Data/CsvFile.h>
#include <DataIO/FileSystem/Path.h>
#include <Utility/Timer.h>
#include <Utility/Concurrent/concurrent.h>
#include <Utility/Concurrent/FixedSizeThreadPool.h>

#include <cstdlib>
#include <cstddef>
#include <cmath>
#include <memory>
#include <array>
#include <vector>
#include <algorithm>
#include <atomic>
#include <limits>
#include <thread>
#include <chrono>
#include <stop_token>

namespace
{

using namespace ph;

using AccurateReal = double;
using AccurateVec3 = math::TVector3<AccurateReal>;
using AccurateQuat = math::TQuaternion<AccurateReal>;
using AccurateMat4 = math::TMatrix4<AccurateReal>;

thread_local math::Pcg32 tls_rng(math::DeterministicSeeder::nextSeed<uint32>());
std::atomic_uint64_t g_numIntersects(0);
std::atomic_uint64_t g_numMissed(0);
std::atomic_uint64_t g_numDegenerateObjs(0);

PH_DEFINE_INTERNAL_LOG_GROUP(IntersectError, IntersectError);

struct IntersectConfig final
{
	std::size_t numObjsPerCase;
	std::size_t numRaysPerObj;
	real minDistance;
	real maxDistance;
	real minRotateDegs;
	real maxRotateDegs;
	real minSize;
	real maxSize;
	real rayMinT = 0;
	real rayMaxT = std::numeric_limits<real>::max();
	bool randomizeRayDir = true;
	bool cosWeightedRay = false;
	real maxObjAspectRatio = 1e6_r;
};

struct IntersectResult final
{
	math::Vector3R objSize;
	math::Vector3R objPos;
	math::Vector3R rayOrigin;
	math::Vector3R hitPos;
	math::Vector3R expectedHitPos;
	math::Vector3R expectedHitNormal;
	real offsetDistance;
};

class IntersectCase
{
public:
	virtual ~IntersectCase() = default;

	virtual void run(
		const IntersectConfig& config,
		std::vector<IntersectResult>& results) const = 0;

	static std::array<real, 2> makeRandomSample2D()
	{
		return {tls_rng.generateSample(), tls_rng.generateSample()};
	}

	static real makeRandomDistance(const IntersectConfig& config)
	{
		return std::lerp(config.minDistance, config.maxDistance, tls_rng.generateSample());
	}

	static math::Vector3R makeRandomPoint(const IntersectConfig& config)
	{
		const auto radius = makeRandomDistance(config);
		return math::TSphere<real>(radius).sampleToSurfaceArchimedes(makeRandomSample2D());
	}

	static math::QuaternionR makeRandomRotate(const IntersectConfig& config)
	{
		const auto degs = std::lerp(
			config.minRotateDegs, config.maxRotateDegs, tls_rng.generateSample());
		const auto dir = math::TSphere<real>::makeUnit().sampleToSurfaceArchimedes(
			makeRandomSample2D());
		return math::QuaternionR(dir, math::to_radians(degs));
	}

	static real makeRandomSize(const IntersectConfig& config)
	{
		return std::lerp(config.minSize, config.maxSize, tls_rng.generateSample());
	}

	static math::Vector3R makeRandomScale(const IntersectConfig& config)
	{
		while(true)
		{
			const auto radius = makeRandomSize(config);
			const auto scale = math::TSphere<real>(radius).sampleToSurfaceArchimedes(makeRandomSample2D());
			if(scale.min() * config.maxObjAspectRatio >= scale.max())
			{
				return scale;
			}
		}

		PH_ASSERT_UNREACHABLE_SECTION();
		return {};
	}

	static math::TDecomposedTransform<real> makeRandomTransform(const IntersectConfig& config)
	{
		return math::TDecomposedTransform<real>(
			makeRandomPoint(config),
			makeRandomRotate(config),
			makeRandomScale(config));
	}

	static void makeRandomTransforms(
		const IntersectConfig& config, 
		const std::size_t numTransforms,
		std::vector<math::TDecomposedTransform<real>>& out_transforms)
	{
		for(std::size_t ti = 0; ti < numTransforms; ++ti)
		{
			out_transforms.push_back(makeRandomTransform(config));
		}
	}

	static Ray makeRandomDirRay(
		const IntersectConfig& config,
		const math::Vector3R& targetHitPos,
		const math::Vector3R& targetHitNormal)
	{
		const auto unitSphere = math::TSphere<real>::makeUnit();
		const auto localDir = config.cosWeightedRay
			? unitSphere.sampleToSurfaceAbsCosThetaWeighted(makeRandomSample2D())
			: unitSphere.sampleToSurfaceArchimedes(makeRandomSample2D());
		const auto dir = math::Basis3R::makeFromUnitY(targetHitNormal).localToWorld(localDir);
		return Ray(targetHitPos, dir.normalize(), config.rayMinT, config.rayMaxT);
	}

	static Ray makeRandomPosRay(
		const IntersectConfig& config, 
		const math::Vector3R& targetHitPos,
		const math::Vector3R& targetHitNormal)
	{
		const auto factor = tls_rng.generateSample();
		const auto radius = std::pow(10.0_r, factor * 20.0_r - 10.0_r);// [10^-10, 10^10]
		const auto sphere = math::TSphere<real>(radius);
		const auto localOrigin = config.cosWeightedRay
			? sphere.sampleToSurfaceAbsCosThetaWeighted(makeRandomSample2D())
			: sphere.sampleToSurfaceArchimedes(makeRandomSample2D());
		const auto origin = 
			targetHitPos + 
			math::Basis3R::makeFromUnitY(targetHitNormal).localToWorld(localOrigin);
		const auto originToTarget = targetHitPos - origin;
		const auto dir = originToTarget.safeNormalize({0, 1, 0});
		return Ray(origin, dir, config.rayMinT, config.rayMaxT);
	}

	static Ray makeHemisphericalRandomDirRay(
		const IntersectConfig& config,
		const math::Vector3R& targetHitPos,
		const math::Vector3R& targetHitNormal)
	{
		const auto unitHemisphere = math::THemisphere<real>::makeUnit();
		const auto localDir = config.cosWeightedRay
			? unitHemisphere.sampleToSurfaceCosThetaWeighted(makeRandomSample2D())
			: unitHemisphere.sampleToSurfaceArchimedes(makeRandomSample2D());
		const auto dir = math::Basis3R::makeFromUnitY(targetHitNormal).localToWorld(localDir);
		return Ray(targetHitPos, dir.normalize(), config.rayMinT, config.rayMaxT);
	}

	static Ray makeHemisphericalRandomPosRay(
		const IntersectConfig& config, 
		const math::Vector3R& targetHitPos,
		const math::Vector3R& targetHitNormal)
	{
		const auto factor = tls_rng.generateSample();
		const auto radius = std::pow(10.0_r, factor * 20.0_r - 10.0_r);// [10^-10, 10^10]
		const auto hemisphere = math::THemisphere<real>(radius);
		const auto localOrigin = config.cosWeightedRay
			? hemisphere.sampleToSurfaceCosThetaWeighted(makeRandomSample2D())
			: hemisphere.sampleToSurfaceArchimedes(makeRandomSample2D());
		const auto origin = 
			targetHitPos + 
			math::Basis3R::makeFromUnitY(targetHitNormal).localToWorld(localOrigin);
		const auto originToTarget = targetHitPos - origin;
		return Ray(origin, originToTarget.normalize(), config.rayMinT, config.rayMaxT);
	}

	static Ray makeRandomRay(
		const IntersectConfig& config, 
		const math::Vector3R& targetHitPos,
		const math::Vector3R& targetHitNormal)
	{
		return config.randomizeRayDir 
			? makeRandomDirRay(config, targetHitPos, targetHitNormal)
			: makeRandomPosRay(config, targetHitPos, targetHitNormal);
	}

	static Ray makeHemisphericalRandomRay(
		const IntersectConfig& config,
		const math::Vector3R& targetHitPos,
		const math::Vector3R& targetHitNormal)
	{
		return config.randomizeRayDir
			? makeHemisphericalRandomDirRay(config, targetHitPos, targetHitNormal)
			: makeHemisphericalRandomPosRay(config, targetHitPos, targetHitNormal);
	}

	static bool findInitialHit(
		const IntersectConfig& config,
		const Intersectable& intersectable,
		const math::Vector3R& potentialHitPos,
		const math::Vector3R& potentialHitNormal,
		math::Vector3R* const out_actualHitPos,
		math::Vector3R* const out_actualhitNormal)
	{
		// Use longest random position ray towards potential hit to have better chance getting a hit
		auto ray = makeRandomPosRay(config, potentialHitPos, potentialHitNormal);
		ray.setRange(0, std::numeric_limits<real>::max());

		HitProbe probe;
		if(!intersectable.isIntersecting(ray, probe))
		{
			return false;
		}

		HitDetail detail;
		intersectable.calcHitDetail(ray, probe, &detail);

		PH_ASSERT(out_actualHitPos);
		PH_ASSERT(out_actualhitNormal);
		*out_actualHitPos = detail.getPosition();
		*out_actualhitNormal = detail.getGeometryNormal();
		return true;
	}

	static real findOffsetDistance(
		const IntersectConfig& config,
		const Intersectable& intersectable,
		const Ray& targetRay,
		const math::Vector3R& targetHitPos,
		const math::Vector3R& targetHitNormal)
	{
		// Offset in the hemisphere of N, so we will not bump into ourself
		// (TODO: for some shapes we need more logics, e.g., offset into -N for concaves)
		const auto offsetDir = targetHitNormal.dot(targetRay.getDirection()) >= 0.0_r
			? targetHitNormal : -targetHitNormal;

		// Assumes `targetHitPos` is an intersection point on `intersectable`
		
		// First find an offset that results in no intersection
		real maxDist = targetHitPos.length() * 1e-5_r;// based on statistics of `IntersectionResult`
		maxDist = maxDist > 0.0_r && std::isfinite(maxDist) ? maxDist : 1e-6_r;
		while(true)
		{
			HitProbe probe;
			Ray ray = targetRay;
			ray.setOrigin(targetHitPos + offsetDir * maxDist);
			if(!intersectable.isIntersecting(ray, probe))
			{
				break;
			}

			maxDist *= 2.0_r;
		}

		// Then use bisection method to find the smallest distance that results in no intersection
		real minDist = 0.0_r;
		while(true)
		{
			const real midDist = (minDist + maxDist) * 0.5_r;
			if(!(minDist < midDist && midDist < maxDist))
			{
				return maxDist;
			}

			HitProbe probe;
			Ray ray = targetRay;
			ray.setOrigin(targetHitPos + offsetDir * midDist);
			if(!intersectable.isIntersecting(ray, probe))
			{
				maxDist = midDist;
			}
			else
			{
				minDist = midDist;
			}
		}
	}
};

class TriangleCase : public IntersectCase
{
public:
	void run(
		const IntersectConfig& config,
		std::vector<IntersectResult>& results) const override
	{
		for(std::size_t oi = 0; oi < config.numObjsPerCase; ++oi)
		{
			const auto triangle = makeRandomTriangle(config);
			if(triangle.isDegenerate() || triangle.getAspectRatio() > config.maxObjAspectRatio)
			{
				g_numDegenerateObjs.fetch_add(1, std::memory_order_relaxed);
				continue;
			}

			const PTriangle ptriangle(triangle.getVa(), triangle.getVb(), triangle.getVc());
			const auto potentialHitPos = triangle.barycentricToSurface(
				triangle.sampleToBarycentricOsada(makeRandomSample2D()));

			IntersectResult result;
			if(!findInitialHit(
				config, ptriangle, potentialHitPos, triangle.getFaceNormal(),
				&result.expectedHitPos, &result.expectedHitNormal))
			{
				continue;
			}

			result.objSize = triangle.getAABB().getExtents();
			result.objPos = triangle.getCentroid();

			for(std::size_t ri = 0; ri < config.numRaysPerObj; ++ri)
			{
				const auto ray = makeRandomRay(
					config, 
					result.expectedHitPos,
					result.expectedHitNormal);

				HitProbe probe;
				if(!ptriangle.isIntersecting(ray, probe))
				{
					g_numMissed.fetch_add(1, std::memory_order_relaxed);
					continue;
				}

				HitDetail detail;
				ptriangle.calcHitDetail(ray, probe, &detail);

				result.rayOrigin = ray.getOrigin();
				result.hitPos = detail.getPosition();
				result.offsetDistance = findOffsetDistance(
					config, ptriangle, ray, detail.getPosition(), detail.getGeometryNormal());

				results.push_back(result);
				g_numIntersects.fetch_add(1, std::memory_order_relaxed);
			}
		}
	}

	static math::TTriangle<real> makeRandomTriangle(const IntersectConfig& config)
	{
		math::Matrix4R mat;
		makeRandomTransform(config).genTransformMatrix(&mat);
		const auto unitSphere = math::TSphere<real>::makeUnit();

		math::Vector3R vA, vB, vC;
		mat.mul(unitSphere.sampleToSurfaceArchimedes(makeRandomSample2D()), 1, &vA);
		mat.mul(unitSphere.sampleToSurfaceArchimedes(makeRandomSample2D()), 1, &vB);
		mat.mul(unitSphere.sampleToSurfaceArchimedes(makeRandomSample2D()), 1, &vC);

		return math::TTriangle<real>(vA, vB, vC);
	}
};

class TransformedTriangleCase : public IntersectCase
{
public:
	explicit TransformedTriangleCase(const std::size_t numTransformLevels)
		: m_numTransformLevels(numTransformLevels)
	{
		PH_ASSERT_GT(numTransformLevels, 0);
	}

	void run(
		const IntersectConfig& config,
		std::vector<IntersectResult>& results) const override
	{
		TriangleData data;
		for(std::size_t oi = 0; oi < config.numObjsPerCase; ++oi)
		{
			makeRandomTriangle(config, data);
			if(data.localTriangle.isDegenerate() || 
			   data.localTriangle.getAspectRatio() > config.maxObjAspectRatio)
			{
				g_numDegenerateObjs.fetch_add(1, std::memory_order_relaxed);
				continue;
			}

			const auto localPotentialHitPos = data.localTriangle.barycentricToSurface(
				data.localTriangle.sampleToBarycentricOsada(makeRandomSample2D()));
			const auto localPotentialHitNormal = data.localTriangle.getFaceNormal();

			math::Vector3R potentialHitPos, potentialHitNormal;
			data.localToWorld.transformP(localPotentialHitPos, &potentialHitPos);
			data.localToWorld.transformO(localPotentialHitNormal, &potentialHitNormal);
			potentialHitNormal = potentialHitNormal.safeNormalize({0, 0, 0});

			IntersectResult result;
			if(potentialHitNormal.isZero() || !findInitialHit(
				config, *data.triangle, potentialHitPos, potentialHitNormal, 
				&result.expectedHitPos, &result.expectedHitNormal))
			{
				continue;
			}

			result.objSize = data.triangle->calcAABB().getExtents();
			data.localToWorld.transformP(data.localTriangle.getCentroid(), &result.objPos);

			for(std::size_t ri = 0; ri < config.numRaysPerObj; ++ri)
			{
				const auto ray = makeRandomRay(
					config, 
					result.expectedHitPos,
					result.expectedHitNormal);

				HitProbe probe;
				if(!data.triangle->isIntersecting(ray, probe))
				{
					g_numMissed.fetch_add(1, std::memory_order_relaxed);
					continue;
				}

				HitDetail detail;
				data.triangle->calcHitDetail(ray, probe, &detail);

				result.rayOrigin = ray.getOrigin();
				result.hitPos = detail.getPosition();
				result.offsetDistance = findOffsetDistance(
					config, *data.triangle, ray, detail.getPosition(), detail.getGeometryNormal());

				results.push_back(result);
				g_numIntersects.fetch_add(1, std::memory_order_relaxed);
			}
		}
	}

private:
	struct TriangleData
	{
		TransformedIntersectable* triangle = nullptr;
		math::TTriangle<real> localTriangle = math::TTriangle<real>();
		math::StaticAffineTransform localToWorld = math::StaticAffineTransform::IDENTITY();
		PTriangle localPTriangle = PTriangle(math::Vector3R(0), math::Vector3R(1), math::Vector3R(2));
		std::vector<TransformedIntersectable> intersectables;
		std::vector<math::StaticAffineTransform> transforms;
		std::vector<math::StaticAffineTransform> inversedTransforms;
		std::vector<math::TDecomposedTransform<real>> decomposedTransforms;
	};

	void makeRandomTriangle(const IntersectConfig& config, TriangleData& out_data) const
	{
		out_data.decomposedTransforms.clear();
		makeRandomTransforms(config, m_numTransformLevels, out_data.decomposedTransforms);

		out_data.localToWorld = math::StaticAffineTransform::makeParentedForward(
			out_data.decomposedTransforms);

		out_data.localTriangle = TriangleCase::makeRandomTriangle(config);
		out_data.localPTriangle = PTriangle(
			out_data.localTriangle.getVa(), out_data.localTriangle.getVb(), out_data.localTriangle.getVc());

		out_data.intersectables.resize(m_numTransformLevels);
		out_data.transforms.resize(m_numTransformLevels);
		out_data.inversedTransforms.resize(m_numTransformLevels);
		for(std::size_t n = out_data.transforms.size(); n > 0; --n)
		{
			const Intersectable* intersectable = n == out_data.transforms.size()
				? static_cast<const Intersectable*>(&out_data.localPTriangle)
				: static_cast<const Intersectable*>(&out_data.intersectables[n]);

			out_data.transforms[n - 1] = math::StaticAffineTransform::makeForward(
				out_data.decomposedTransforms[n - 1]);
			out_data.inversedTransforms[n - 1] = math::StaticAffineTransform::makeInverse(
				out_data.decomposedTransforms[n - 1]);
			out_data.intersectables[n - 1] = TransformedIntersectable(
				intersectable,
				&out_data.transforms[n - 1],
				&out_data.inversedTransforms[n - 1]);
		}
		out_data.triangle = &out_data.intersectables.front();
	}

	std::size_t m_numTransformLevels;
};

class SphereCase : public IntersectCase
{
public:
	void run(
		const IntersectConfig& config,
		std::vector<IntersectResult>& results) const override
	{
		for(std::size_t oi = 0; oi < config.numObjsPerCase; ++oi)
		{
			const auto sphere = makeRandomSphere(config);
			if(sphere.getRadius() <= 0)
			{
				g_numDegenerateObjs.fetch_add(1, std::memory_order_relaxed);
				continue;
			}

			const PLatLong01Sphere psphere(sphere.getRadius());
			const auto potentialHitPos = sphere.sampleToSurfaceArchimedes(makeRandomSample2D());

			IntersectResult result;
			if(!findInitialHit(
				config, psphere, potentialHitPos, potentialHitPos / sphere.getRadius(),
				&result.expectedHitPos, &result.expectedHitNormal))
			{
				continue;
			}

			result.objSize = sphere.getAABB().getExtents();
			result.objPos = math::Vector3R(0);

			for(std::size_t ri = 0; ri < config.numRaysPerObj; ++ri)
			{
				// Use ray in the hemisphere defined by normal, otherwise we can get the further
				// ray-sphere hit point
				const auto ray = makeHemisphericalRandomRay(
					config, 
					result.expectedHitPos,
					result.expectedHitNormal);

				HitProbe probe;
				if(!psphere.isIntersecting(ray, probe))
				{
					g_numMissed.fetch_add(1, std::memory_order_relaxed);
					continue;
				}

				HitDetail detail;
				psphere.calcHitDetail(ray, probe, &detail);

				result.rayOrigin = ray.getOrigin();
				result.hitPos = detail.getPosition();
				result.offsetDistance = findOffsetDistance(
					config, psphere, ray, detail.getPosition(), detail.getGeometryNormal());

				results.push_back(result);
				g_numIntersects.fetch_add(1, std::memory_order_relaxed);
			}
		}
	}

	static math::TSphere<real> makeRandomSphere(const IntersectConfig& config)
	{
		return math::TSphere<real>(makeRandomSize(config));
	}
};

class TransformedSphereCase : public IntersectCase
{
public:
	explicit TransformedSphereCase(const std::size_t numTransformLevels)
		: m_numTransformLevels(numTransformLevels)
	{
		PH_ASSERT_GT(numTransformLevels, 0);
	}

	void run(
		const IntersectConfig& config,
		std::vector<IntersectResult>& results) const override
	{
		SphereData data;
		for(std::size_t oi = 0; oi < config.numObjsPerCase; ++oi)
		{
			makeRandomSphere(config, data);
			if(data.localSphere.getRadius() <= 0)
			{
				g_numDegenerateObjs.fetch_add(1, std::memory_order_relaxed);
				continue;
			}

			const auto localPotentialHitPos = data.localSphere.sampleToSurfaceArchimedes(
				makeRandomSample2D());
			const auto localPotentialHitNormal = localPotentialHitPos / data.localSphere.getRadius();

			math::Vector3R potentialHitPos, potentialHitNormal;
			data.localToWorld.transformP(localPotentialHitPos, &potentialHitPos);
			data.localToWorld.transformO(localPotentialHitNormal, &potentialHitNormal);
			potentialHitNormal = potentialHitNormal.safeNormalize({0, 0, 0});

			IntersectResult result;
			if(potentialHitNormal.isZero() || !findInitialHit(
				config, *data.sphere, potentialHitPos, potentialHitNormal, 
				&result.expectedHitPos, &result.expectedHitNormal))
			{
				continue;
			}

			result.objSize = data.sphere->calcAABB().getExtents();
			result.objPos = data.sphere->calcAABB().getCentroid();

			for(std::size_t ri = 0; ri < config.numRaysPerObj; ++ri)
			{
				// Use ray in the hemisphere defined by normal, otherwise we can get the further
				// ray-sphere hit point
				const auto ray = makeHemisphericalRandomRay(
					config, 
					result.expectedHitPos,
					result.expectedHitNormal);

				HitProbe probe;
				if(!data.sphere->isIntersecting(ray, probe))
				{
					g_numMissed.fetch_add(1, std::memory_order_relaxed);
					continue;
				}

				HitDetail detail;
				data.sphere->calcHitDetail(ray, probe, &detail);

				result.rayOrigin = ray.getOrigin();
				result.hitPos = detail.getPosition();
				result.offsetDistance = findOffsetDistance(
					config, *data.sphere, ray, detail.getPosition(), detail.getGeometryNormal());

				results.push_back(result);
				g_numIntersects.fetch_add(1, std::memory_order_relaxed);
			}
		}
	}

private:
	struct SphereData
	{
		TransformedIntersectable* sphere = nullptr;
		math::TSphere<real> localSphere = math::TSphere<real>(0);
		math::StaticAffineTransform localToWorld = math::StaticAffineTransform::IDENTITY();
		PLatLong01Sphere localPSphere = PLatLong01Sphere(0);
		std::vector<TransformedIntersectable> intersectables;
		std::vector<math::StaticAffineTransform> transforms;
		std::vector<math::StaticAffineTransform> inversedTransforms;
		std::vector<math::TDecomposedTransform<real>> decomposedTransforms;
	};

	void makeRandomSphere(const IntersectConfig& config, SphereData& out_data) const
	{
		out_data.decomposedTransforms.clear();
		makeRandomTransforms(config, m_numTransformLevels, out_data.decomposedTransforms);

		out_data.localToWorld = math::StaticAffineTransform::makeParentedForward(
			out_data.decomposedTransforms);

		out_data.localSphere = SphereCase::makeRandomSphere(config);
		out_data.localPSphere = PLatLong01Sphere(out_data.localSphere.getRadius());

		out_data.intersectables.resize(m_numTransformLevels);
		out_data.transforms.resize(m_numTransformLevels);
		out_data.inversedTransforms.resize(m_numTransformLevels);
		for(std::size_t n = out_data.transforms.size(); n > 0; --n)
		{
			const Intersectable* intersectable = n == out_data.transforms.size()
				? static_cast<const Intersectable*>(&out_data.localPSphere)
				: static_cast<const Intersectable*>(&out_data.intersectables[n]);

			out_data.transforms[n - 1] = math::StaticAffineTransform::makeForward(
				out_data.decomposedTransforms[n - 1]);
			out_data.inversedTransforms[n - 1] = math::StaticAffineTransform::makeInverse(
				out_data.decomposedTransforms[n - 1]);
			out_data.intersectables[n - 1] = TransformedIntersectable(
				intersectable,
				&out_data.transforms[n - 1],
				&out_data.inversedTransforms[n - 1]);
		}
		out_data.sphere = &out_data.intersectables.front();
	}

	std::size_t m_numTransformLevels;
};

/*!
Basically a xy-chart, where x is in log scale. The interval [`minX`, `maxX`] is divided into
`numBins` sub-intervals. Since the width of the bins are in log scale, larger x corresponds to
a wider bin.
*/
class BinnedData final
{
public:
	BinnedData() = default;

	BinnedData(const std::size_t numBins, const AccurateReal minX, const AccurateReal maxX)
		: m_xs(numBins)
		, m_minX(minX)
		, m_maxX(maxX)
		, m_logMinX()
		, m_logMaxX()
	{
		PH_ASSERT_GE(numBins, 1);
		PH_ASSERT_GT(minX, 0);
		PH_ASSERT_LT(minX, maxX);

		m_logMinX = std::log10(minX);
		m_logMaxX = std::log10(maxX);
	}

	void addValue(const AccurateReal x, const AccurateReal y)
	{
		// NaN aware
		if(!(m_minX <= x && x <= m_maxX) || std::isnan(y))
		{
			return;
		}

		const auto logX = std::log10(x);
		const auto fraction = (logX - m_logMinX) / (m_logMaxX - m_logMinX);
		auto idx = static_cast<std::size_t>(fraction * m_xs.size());
		idx = idx < m_xs.size() ? idx : m_xs.size() - 1;

		Bin& bin = m_xs[idx];
		bin.numY++;
		bin.sumY += y;
		bin.sumY2 += y * y;
		bin.minY = std::min(y, bin.minY);
		bin.maxY = std::max(y, bin.maxY);
	}

	void saveAsCsv(const Path& filePath)
	{
		CsvFile file;
		std::string strBuffer;
		for(std::size_t ri = 0; ri < m_xs.size(); ++ri)
		{
			const Bin& bin = m_xs[ri];
			if(bin.numY == 0 || 
			   bin.minY >= std::numeric_limits<AccurateReal>::max() ||
			   bin.maxY <= std::numeric_limits<AccurateReal>::min())
			{
				continue;
			}

			const auto fraction = (ri + 0.5) / m_xs.size();
			const auto logX = std::lerp(m_logMinX, m_logMaxX, fraction);
			const auto x = std::pow(10, logX);
			const auto mean = bin.sumY / bin.numY;
			const auto sigma = std::sqrt(std::max(bin.sumY2 / bin.numY - mean * mean, 0.0));

			CsvFileRow row;
			row.addValue(string_utils::stringify_number(x,        &strBuffer));
			row.addValue(string_utils::stringify_number(bin.numY, &strBuffer));
			row.addValue(string_utils::stringify_number(mean,     &strBuffer));
			row.addValue(string_utils::stringify_number(sigma,    &strBuffer));
			row.addValue(string_utils::stringify_number(bin.minY, &strBuffer));
			row.addValue(string_utils::stringify_number(bin.maxY, &strBuffer));
			file.addRow(row);
		}

		file.save(filePath);
	}

	void mergeWith(const BinnedData& other)
	{
		PH_ASSERT_EQ(m_xs.size(), other.m_xs.size());
		PH_ASSERT_EQ(m_minX, other.m_minX);
		PH_ASSERT_EQ(m_maxX, other.m_maxX);

		for(std::size_t xi = 0; xi < m_xs.size(); ++xi)
		{
			Bin& bin = m_xs[xi];
			const Bin& otherBin = other.m_xs[xi];

			bin.numY  += otherBin.numY;
			bin.sumY  += otherBin.sumY;
			bin.sumY2 += otherBin.sumY2;
			bin.minY  =  std::min(otherBin.minY, bin.minY);
			bin.maxY  =  std::max(otherBin.maxY, bin.maxY);
		}
	}

private:
	struct Bin
	{
		std::size_t numY = 0;
		AccurateReal sumY = 0;
		AccurateReal sumY2 = 0;
		AccurateReal minY = std::numeric_limits<AccurateReal>::max();
		AccurateReal maxY = std::numeric_limits<AccurateReal>::min();
	};

	std::vector<Bin> m_xs;
	AccurateReal m_minX = 1;
	AccurateReal m_maxX = 10;
	AccurateReal m_logMinX = std::log10(1);
	AccurateReal m_logMaxX = std::log10(10);
};

class BinnedDataCollection final
{
public:
	BinnedData errorVsDist;
	BinnedData errorVsSize;
	BinnedData offsetVsDist;

	void mergeWith(const BinnedDataCollection& other)
	{
		errorVsDist.mergeWith(other.errorVsDist);
		errorVsSize.mergeWith(other.errorVsSize);
		offsetVsDist.mergeWith(other.offsetVsDist);
	}
};

BinnedDataCollection run_cases(
	const std::vector<std::unique_ptr<IntersectCase>>& cases,
	const IntersectConfig& config)
{
	PH_ASSERT_GT(config.numRaysPerObj, 0);
	const std::size_t iterationSize = 1000000 / config.numRaysPerObj;
	
	BinnedDataCollection bins{
		.errorVsDist = BinnedData(50000, 1e-30, 1e30),
		.errorVsSize = BinnedData(50000, 1e-30, 1e30),
		.offsetVsDist = BinnedData(50000, 1e-30, 1e30)};

	IntersectConfig iterationConfig = config;
	std::vector<IntersectResult> results;
	std::size_t numObjsPerCaseRemained = config.numObjsPerCase;
	while(numObjsPerCaseRemained > 0)
	{
		iterationConfig.numObjsPerCase = std::min(iterationSize, numObjsPerCaseRemained);
		numObjsPerCaseRemained -= iterationConfig.numObjsPerCase;

		results.clear();
		for(const auto& intersectCase: cases)
		{
			intersectCase->run(iterationConfig, results);
		}

		for(const IntersectResult& result : results)
		{
			const auto hitDist = AccurateVec3(result.expectedHitPos).length();
			const auto objSize = result.objSize.max();
			const auto errorVec = AccurateVec3(result.hitPos) - AccurateVec3(result.expectedHitPos);
			const auto distToPlane = std::abs(errorVec.dot(AccurateVec3(result.expectedHitNormal)));

			bins.errorVsDist.addValue(hitDist, distToPlane);
			bins.errorVsSize.addValue(objSize, distToPlane);
			bins.offsetVsDist.addValue(hitDist, result.offsetDistance);
		}
	}

	return bins;
}

BinnedDataCollection run_cases_parallel(
	const std::vector<std::unique_ptr<IntersectCase>>& cases,
	const IntersectConfig& config,
	FixedSizeThreadPool& threads)
{
	std::vector<BinnedDataCollection> threadBins(threads.numWorkers());
	parallel_work(threads, config.numObjsPerCase,
		[&cases, &config, &threadBins]
		(const std::size_t workerIdx, const std::size_t workBegin, const std::size_t workEnd)
		{
			IntersectConfig threadConfig = config;
			threadConfig.numObjsPerCase = workEnd - workBegin;

			threadBins[workerIdx] = run_cases(cases, threadConfig);
		});

	BinnedDataCollection mergedBins = threadBins[0];
	for(std::size_t ti = 1; ti < threadBins.size(); ++ti)
	{
		mergedBins.mergeWith(threadBins[ti]);
	}
	return mergedBins;
}

}// end anonymous namespace

int main(int argc, char* argv[])
{
	if(!init_render_engine(EngineInitSettings{}))
	{
		return EXIT_FAILURE;
	}

	Timer timer;
	timer.start();

	FixedSizeThreadPool threads(10);

	std::vector<std::unique_ptr<IntersectCase>> cases;
	cases.push_back(std::make_unique<TriangleCase>());
	//cases.push_back(std::make_unique<TransformedTriangleCase>(3));
	//cases.push_back(std::make_unique<SphereCase>());
	//cases.push_back(std::make_unique<TransformedSphereCase>(2));

	PH_LOG(IntersectError, Note,
		"Run intersection cases using {} threads.", threads.numWorkers());

	std::jthread statsThread([](std::stop_token token)
	{
		while(!token.stop_requested())
		{
			PH_LOG(IntersectError, Note,
				"Intersects: {}, missed: {}, degenerate objs: {}",
				g_numIntersects.load(std::memory_order_relaxed),
				g_numMissed.load(std::memory_order_relaxed),
				g_numDegenerateObjs.load(std::memory_order_relaxed));

			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		}
	});

	constexpr real step = 5.0_r;

	std::vector<BinnedDataCollection> allBins;
	//for(real objDistance = 1e-3_r; objDistance < 1e3_r; objDistance *= step)
	for(real objDistance = 1e-6_r; objDistance < 1e8_r; objDistance *= step)
	{
		for(real objSize = 1e-2_r; objSize < 1e-1_r; objSize *= step)
		//for(real objSize = 1e-6_r; objSize < 1e6_r; objSize *= step)
		{
			IntersectConfig config;
			config.numObjsPerCase = 100000;
			config.numRaysPerObj = 16;
			config.minDistance = objDistance / step;
			config.maxDistance = objDistance * step;
			config.minRotateDegs = -72000;
			config.maxRotateDegs = 72000;
			config.minSize = objSize / step;
			config.maxSize = objSize * step;

			allBins.push_back(run_cases_parallel(cases, config, threads));
		}
	}

	statsThread.request_stop();

	BinnedDataCollection mergedBins = allBins[0];
	for(std::size_t ci = 1; ci < allBins.size(); ++ci)
	{
		mergedBins.mergeWith(allBins[ci]);
	}

	timer.stop();

	PH_LOG(IntersectError, Note, 
		"Intersects {} objects.", g_numIntersects.load(std::memory_order_relaxed));
	PH_LOG(IntersectError, Note,
		"Time spent: {} s.", timer.getDeltaS());

	mergedBins.errorVsDist.saveAsCsv(get_script_directory(EEngineProject::IntersectError)
		/ Path("error_vs_dist.csv"));
	mergedBins.errorVsSize.saveAsCsv(get_script_directory(EEngineProject::IntersectError)
		/ Path("error_vs_size.csv"));
	mergedBins.offsetVsDist.saveAsCsv(get_script_directory(EEngineProject::IntersectError)
		/ Path("offset_vs_dist.csv"));

	return exit_render_engine() ? EXIT_SUCCESS : EXIT_FAILURE;
}
