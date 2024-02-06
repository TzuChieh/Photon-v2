#include <ph_core.h>
#include <Common/assertion.h>
#include <Common/logging.h>
#include <Core/Intersectable/PTriangle.h>
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
#include <Math/Transform/TDecomposedTransform.h>
#include <DataIO/Data/CsvFile.h>
#include <DataIO/FileSystem/Path.h>
#include <Utility/Timer.h>

#include <cstdlib>
#include <cstddef>
#include <memory>
#include <array>
#include <vector>
#include <algorithm>
#include <atomic>

namespace
{

using namespace ph;

using AccurateReal = double;
using AccurateVec3 = math::TVector3<AccurateReal>;
using AccurateQuat = math::TQuaternion<AccurateReal>;

thread_local math::Pcg32 tls_rng(math::DeterministicSeeder::nextSeed<uint32>());
std::atomic_uint64_t g_numIntersects(0);

PH_DEFINE_INTERNAL_LOG_GROUP(IntersectError, IntersectError);

struct IntersectConfig final
{
	std::size_t numObjsPerCase;
	std::size_t numRaysPerObj;
	math::Vector3R minTranslate;
	math::Vector3R maxTranslate;
	real minRotateDegs;
	real maxRotateDegs;
	math::Vector3R minScale;
	math::Vector3R maxScale;
};

struct IntersectResult final
{
	math::Vector3R objSize;
	math::Vector3R objPos;
	math::Vector3R rayOrigin;
	math::Vector3R hitPos;
	math::Vector3R expectedHitPos;
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

	static math::Vector3R makeRandomTranslate(const IntersectConfig& config)
	{
		return math::Vector3R::lerp(
			config.minTranslate, 
			config.maxTranslate, 
			tls_rng.generateSample());
	}

	static math::QuaternionR makeRandomRotate(const IntersectConfig& config)
	{
		const auto degs = std::lerp(
			config.minRotateDegs, config.maxRotateDegs, tls_rng.generateSample());
		const auto dir = math::TSphere<real>::makeUnit().sampleToSurfaceArchimedes(
			{tls_rng.generateSample(), tls_rng.generateSample()});
		return math::QuaternionR(dir, math::to_radians(degs));
	}

	static math::Vector3R makeRandomScale(const IntersectConfig& config)
	{
		return math::Vector3R::lerp(
			config.minScale,
			config.maxScale,
			tls_rng.generateSample());
	}

	static math::TDecomposedTransform<real> makeRandomTransform(const IntersectConfig& config)
	{
		return math::TDecomposedTransform<real>(
			makeRandomTranslate(config),
			makeRandomRotate(config),
			makeRandomScale(config));
	}

	static math::Matrix4R makeRandomTransformMatrix(const IntersectConfig& config)
	{
		math::Matrix4R mat;
		makeRandomTransform(config).genTransformMatrix(&mat);
		return mat;
	}

	static Ray makeRandomRay(const IntersectConfig& config, const math::Vector3R& targetPos)
	{
		while(true)
		{
			const auto mat = makeRandomTransformMatrix(config);
			const auto unitSphere = math::TSphere<real>::makeUnit();

			math::Vector3R origin;
			mat.mul(unitSphere.sampleToSurfaceArchimedes(makeRandomSample2D()), 1, &origin);

			const auto originToTarget = targetPos - origin;
			if(originToTarget.lengthSquared() > 0)
			{
				return Ray(origin, originToTarget.normalize());
			}
		}
		
		PH_ASSERT_UNREACHABLE_SECTION();
		return Ray();
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
			const PTriangle ptriangle(triangle.getVa(), triangle.getVb(), triangle.getVc());
			const auto targetHitPos = triangle.barycentricToSurface(
				triangle.sampleToBarycentricOsada(makeRandomSample2D()));

			IntersectResult result;
			result.objSize = triangle.getAABB().getExtents();
			result.objPos = triangle.getCentroid();
			result.expectedHitPos = targetHitPos;

			for(std::size_t ri = 0; ri < config.numRaysPerObj; ++ri)
			{
				const auto ray = makeRandomRay(config, targetHitPos);

				HitProbe probe;
				if(!ptriangle.isIntersecting(ray, probe))
				{
					continue;
				}

				HitDetail hitDetail;
				ptriangle.calcIntersectionDetail(ray, probe, &hitDetail);

				result.rayOrigin = ray.getOrigin();
				result.hitPos = hitDetail.getPosition();

				results.push_back(result);
				g_numIntersects.fetch_add(1, std::memory_order_relaxed);
			}
		}
	}

	static math::TTriangle<real> makeRandomTriangle(const IntersectConfig& config)
	{
		while(true)
		{
			const auto mat = makeRandomTransformMatrix(config);
			const auto unitSphere = math::TSphere<real>::makeUnit();

			math::Vector3R vA, vB, vC;
			mat.mul(unitSphere.sampleToSurfaceArchimedes(makeRandomSample2D()), 1, &vA);
			mat.mul(unitSphere.sampleToSurfaceArchimedes(makeRandomSample2D()), 1, &vB);
			mat.mul(unitSphere.sampleToSurfaceArchimedes(makeRandomSample2D()), 1, &vC);

			math::TTriangle<real> triangle(vA, vB, vC);
			if(!triangle.isDegenerate())
			{
				return triangle;
			}
		}

		PH_ASSERT_UNREACHABLE_SECTION();
		return math::TTriangle<real>();
	}
};

class ChartData final
{
public:
	ChartData(const std::size_t numEntries, const real minX, const real maxX)
		: m_xs(numEntries)
		, m_minX(minX)
		, m_maxX(maxX)
		, m_logMinX()
		, m_logMaxX()
	{
		PH_ASSERT_GE(numEntries, 1);
		PH_ASSERT_GT(minX, 0);
		PH_ASSERT_LT(minX, maxX);

		m_logMinX = std::log10(minX);
		m_logMaxX = std::log10(maxX);
	}

	void addValue(const real x, const real value)
	{
		if(x < m_minX || m_maxX < x)
		{
			return;
		}

		const auto logX = std::log10(x);
		const auto fraction = (logX - m_logMinX) / (m_logMaxX - m_logMinX);
		auto idx = static_cast<std::size_t>(fraction * m_xs.size());
		idx = idx < m_xs.size() ? idx : m_xs.size() - 1;

		Entry& entry = m_xs[idx];
		entry.num++;
		entry.sum += value;
		entry.min = std::min(value, entry.min);
		entry.max = std::max(value, entry.max);
	}

	void saveAsCsv(const Path& filePath)
	{
		CsvFile file;
		for(std::size_t ri = 0; ri < m_xs.size(); ++ri)
		{
			const Entry& entry = m_xs[ri];

			const auto fraction = (ri + 0.5_r) / m_xs.size();
			const auto logX = std::lerp(m_logMinX, m_logMaxX, fraction);
			const auto x = std::pow(10, logX);
			const auto mean = entry.num > 0 ? entry.sum / entry.num : 0.0;

			CsvFileRow row;
			row.addValue(std::to_string(x));
			row.addValue(std::to_string(entry.num));
			row.addValue(std::to_string(mean));
			row.addValue(std::to_string(entry.min));
			row.addValue(std::to_string(entry.max));

			file.addRow(row);
		}

		file.save(filePath);
	}

private:
	struct Entry
	{
		std::size_t num = 0;
		AccurateReal sum = 0;
		real min = 0;
		real max = 0;
	};

	std::vector<Entry> m_xs;
	real m_minX;
	real m_maxX;
	real m_logMinX;
	real m_logMaxX;
};

}// end anonymous namespace

int main(int argc, char* argv[])
{
	if(!init_render_engine(EngineInitSettings{}))
	{
		return EXIT_FAILURE;
	}

	Timer timer;
	timer.start();

	IntersectConfig config;
	config.numObjsPerCase = 1000;
	config.numRaysPerObj = 4;
	config.minTranslate = {-1e3_r, -1e3_r, -1e3_r};
	config.maxTranslate = {1e3_r, 1e3_r, 1e3_r};
	config.minRotateDegs = -720;
	config.maxRotateDegs = 720;
	config.minScale = {-1e2_r, -1e2_r, -1e2_r};
	config.maxScale = {1e2_r, 1e2_r, 1e2_r};

	std::vector<IntersectResult> results;

	TriangleCase triangleCase;
	triangleCase.run(config, results);

	timer.stop();

	PH_LOG(IntersectError, Note, 
		"Intersects {} objects.", g_numIntersects.load(std::memory_order_relaxed));
	PH_LOG(IntersectError, Note,
		"Time spent: {} s.", timer.getDeltaS());

	ChartData chart(100, 1e-5_r, 1e5_r);
	for(const IntersectResult& result : results)
	{
		const real objDistance = result.objPos.length();
		const auto hitError = static_cast<real>(
			(AccurateVec3(result.expectedHitPos) - AccurateVec3(result.hitPos)).length());

		chart.addValue(objDistance, hitError);
	}
	chart.saveAsCsv(Path("./intersect_error.csv"));

	return exit_render_engine() ? EXIT_SUCCESS : EXIT_FAILURE;
}
