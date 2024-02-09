#include <ph_core.h>
#include <Common/assertion.h>
#include <Common/logging.h>
#include <Common/Utility/string_utils.h>
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
#include <Math/TOrthonormalBasis3.h>
#include <Math/Transform/TDecomposedTransform.h>
#include <DataIO/Data/CsvFile.h>
#include <DataIO/FileSystem/Path.h>
#include <Utility/Timer.h>
#include <Utility/Concurrent/concurrent.h>
#include <Utility/Concurrent/FixedSizeThreadPool.h>

#include <cstdlib>
#include <cstddef>
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
	bool radomizeRayDir = true;
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
			{tls_rng.generateSample(), tls_rng.generateSample()});
		return math::QuaternionR(dir, math::to_radians(degs));
	}

	static real makeRandomSize(const IntersectConfig& config)
	{
		return std::lerp(config.minSize, config.maxSize, tls_rng.generateSample());
	}

	static math::Vector3R makeRandomScale(const IntersectConfig& config)
	{
		const auto radius = makeRandomSize(config);
		return math::TSphere<real>(radius).sampleToSurfaceArchimedes(makeRandomSample2D());
	}

	static math::TDecomposedTransform<real> makeRandomTransform(const IntersectConfig& config)
	{
		return math::TDecomposedTransform<real>(
			makeRandomPoint(config),
			makeRandomRotate(config),
			makeRandomScale(config));
	}

	static math::Matrix4R makeRandomTransformMatrix(const IntersectConfig& config)
	{
		math::Matrix4R mat;
		makeRandomTransform(config).genTransformMatrix(&mat);
		return mat;
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
		return Ray(origin, originToTarget.normalize(), config.rayMinT, config.rayMaxT);
	}

	static Ray makeRandomRay(
		const IntersectConfig& config, 
		const math::Vector3R& targetHitPos,
		const math::Vector3R& targetHitNormal)
	{
		return config.radomizeRayDir 
			? makeRandomDirRay(config, targetHitPos, targetHitNormal)
			: makeRandomPosRay(config, targetHitPos, targetHitNormal);
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
			const auto targetHitPos = triangle.barycentricToSurface(
				triangle.sampleToBarycentricOsada(makeRandomSample2D()));

			IntersectResult result;
			result.objSize = triangle.getAABB().getExtents();
			result.objPos = triangle.getCentroid();
			result.expectedHitPos = targetHitPos;
			result.expectedHitNormal = triangle.getFaceNormal();

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

				HitDetail hitDetail;
				ptriangle.calcIntersectionDetail(ray, probe, &hitDetail);

				result.rayOrigin = ray.getOrigin();
				result.hitPos = hitDetail.getPosition();

				results.push_back(result);
				g_numIntersects.fetch_add(1, std::memory_order_relaxed);

				// DEBUG
				{
					const auto hitDist = AccurateVec3(result.expectedHitPos).length();
					const auto objSize = result.objSize.max();
					const auto errorVec = AccurateVec3(result.hitPos) - AccurateVec3(result.expectedHitPos);
					const auto distToPlane = std::abs(errorVec.dot(AccurateVec3(result.expectedHitNormal)));
					if(hitDist > 0 && distToPlane / hitDist > 0.01)
					{
						PH_LOG(IntersectError, Warning,
							"Outlier: hit distance = {}, error = {}, vA = {}, vB = {}, vC = {}, "
							"aspect ratio = {}", 
							hitDist, distToPlane, triangle.getVa(), triangle.getVb(), triangle.getVc(),
							triangle.getAspectRatio());
					}
				}
			}
		}
	}

	static math::TTriangle<real> makeRandomTriangle(const IntersectConfig& config)
	{
		const auto mat = makeRandomTransformMatrix(config);
		const auto unitSphere = math::TSphere<real>::makeUnit();

		math::Vector3R vA, vB, vC;
		mat.mul(unitSphere.sampleToSurfaceArchimedes(makeRandomSample2D()), 1, &vA);
		mat.mul(unitSphere.sampleToSurfaceArchimedes(makeRandomSample2D()), 1, &vB);
		mat.mul(unitSphere.sampleToSurfaceArchimedes(makeRandomSample2D()), 1, &vC);

		return math::TTriangle<real>(vA, vB, vC);
	}
};

class ChartData final
{
public:
	ChartData() = default;

	ChartData(const std::size_t numEntries, const AccurateReal minX, const AccurateReal maxX)
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

	void addValue(const AccurateReal x, const AccurateReal value)
	{
		// NaN aware
		if(!(m_minX <= x && x <= m_maxX) || std::isnan(value))
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
		std::string strBuffer;
		for(std::size_t ri = 0; ri < m_xs.size(); ++ri)
		{
			const Entry& entry = m_xs[ri];
			if(entry.num == 0)
			{
				continue;
			}

			const auto fraction = (ri + 0.5_r) / m_xs.size();
			const auto logX = std::lerp(m_logMinX, m_logMaxX, fraction);
			const auto x = std::pow(10, logX);
			const auto mean = entry.sum / entry.num;

			CsvFileRow row;
			row.addValue(string_utils::stringify_number(x,         &strBuffer));
			row.addValue(string_utils::stringify_number(entry.num, &strBuffer));
			row.addValue(string_utils::stringify_number(mean,      &strBuffer));
			row.addValue(string_utils::stringify_number(entry.min, &strBuffer));
			row.addValue(string_utils::stringify_number(entry.max, &strBuffer));
			file.addRow(row);
		}

		file.save(filePath);
	}

	void mergeWith(const ChartData& other)
	{
		PH_ASSERT_EQ(m_xs.size(), other.m_xs.size());
		PH_ASSERT_EQ(m_minX, other.m_minX);
		PH_ASSERT_EQ(m_maxX, other.m_maxX);

		for(std::size_t xi = 0; xi < m_xs.size(); ++xi)
		{
			Entry& entry = m_xs[xi];
			const Entry& otherEntry = other.m_xs[xi];

			entry.num += otherEntry.num;
			entry.sum += otherEntry.sum;
			entry.min = std::min(otherEntry.min, entry.min);
			entry.max = std::max(otherEntry.max, entry.max);
		}
	}

private:
	struct Entry
	{
		std::size_t num = 0;
		AccurateReal sum = 0;
		AccurateReal min = std::numeric_limits<AccurateReal>::max();
		AccurateReal max = std::numeric_limits<AccurateReal>::min();
	};

	std::vector<Entry> m_xs;
	AccurateReal m_minX = 1;
	AccurateReal m_maxX = 10;
	AccurateReal m_logMinX = std::log10(1);
	AccurateReal m_logMaxX = std::log10(10);
};

class ChartDataCollection final
{
public:
	ChartData errorVsDistChart;
	ChartData errorVsSizeChart;

	void mergeWith(const ChartDataCollection& other)
	{
		errorVsDistChart.mergeWith(other.errorVsDistChart);
		errorVsSizeChart.mergeWith(other.errorVsSizeChart);
	}
};

ChartDataCollection run_cases(
	const std::vector<std::unique_ptr<IntersectCase>>& cases,
	const IntersectConfig& config)
{
	PH_ASSERT_GT(config.numRaysPerObj, 0);
	const std::size_t iterationSize = 1000000 / config.numRaysPerObj;

	ChartDataCollection charts{
		.errorVsDistChart = ChartData(50000, 1e-15_r, 1e15_r),
		.errorVsSizeChart = ChartData(50000, 1e-15_r, 1e15_r)};

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

			charts.errorVsDistChart.addValue(hitDist, distToPlane);
			charts.errorVsSizeChart.addValue(objSize, distToPlane);
		}
	}

	return charts;
}

ChartDataCollection run_cases_parallel(
	const std::vector<std::unique_ptr<IntersectCase>>& cases,
	const IntersectConfig& config,
	FixedSizeThreadPool& threads)
{
	std::vector<ChartDataCollection> threadCharts(threads.numWorkers());
	parallel_work(threads, config.numObjsPerCase,
		[&cases, &config, &threadCharts]
		(const std::size_t workerIdx, const std::size_t workBegin, const std::size_t workEnd)
		{
			IntersectConfig threadConfig = config;
			threadConfig.numObjsPerCase = workEnd - workBegin;

			threadCharts[workerIdx] = run_cases(cases, threadConfig);
		});

	ChartDataCollection mergedCharts = threadCharts[0];
	for(std::size_t ti = 1; ti < threadCharts.size(); ++ti)
	{
		mergedCharts.mergeWith(threadCharts[ti]);
	}
	return mergedCharts;
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

	std::vector<ChartDataCollection> allCharts;
	for(real objDistance = 1e-6_r; objDistance < 1e9_r; objDistance *= step)
	{
		for(real objSize = 1e-6_r; objSize < 1e7_r; objSize *= step)
		{
			IntersectConfig config;
			config.numObjsPerCase = 1000000;
			config.numRaysPerObj = 128;
			config.minDistance = objDistance / step;
			config.maxDistance = objDistance * step;
			config.minRotateDegs = -72000;
			config.maxRotateDegs = 72000;
			config.minSize = objSize / step;
			config.maxSize = objSize * step;

			allCharts.push_back(run_cases_parallel(cases, config, threads));
		}
	}

	statsThread.request_stop();

	ChartDataCollection mergedCharts = allCharts[0];
	for(std::size_t ci = 1; ci < allCharts.size(); ++ci)
	{
		mergedCharts.mergeWith(allCharts[ci]);
	}

	timer.stop();

	PH_LOG(IntersectError, Note, 
		"Intersects {} objects.", g_numIntersects.load(std::memory_order_relaxed));
	PH_LOG(IntersectError, Note,
		"Time spent: {} s.", timer.getDeltaS());

	mergedCharts.errorVsDistChart.saveAsCsv(get_script_directory(EEngineProject::IntersectError)
		/ Path("error_vs_dist.csv"));
	mergedCharts.errorVsSizeChart.saveAsCsv(get_script_directory(EEngineProject::IntersectError)
		/ Path("error_vs_size.csv"));

	return exit_render_engine() ? EXIT_SUCCESS : EXIT_FAILURE;
}
