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
#include <Math/Random/TPwcDistribution1D.h>
#include <Math/Geometry/TSphere.h>
#include <Math/Geometry/TTriangle.h>
#include <Math/Transform/TDecomposedTransform.h>
#include <DataIO/Data/CsvFile.h>
#include <DataIO/FileSystem/Path.h>
#include <Utility/Timer.h>
#include <Utility/Concurrent/concurrent.h>

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

PH_DEFINE_INTERNAL_LOG_GROUP(IntersectError, IntersectError);

struct IntersectConfig final
{
	std::size_t numObjsPerCase;
	std::size_t numRaysPerObj;
	real minDistance;
	real maxDistance;
	real minRotateDegs;
	real maxRotateDegs;
	math::TPwcDistribution1D<real> distanceDistribution;
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

	static math::Vector3R makeRandomPoint(const IntersectConfig& config)
	{
		const auto factor = config.distanceDistribution.sampleContinuous(tls_rng.generateSample());
		const auto radius = std::lerp(config.minDistance, config.maxDistance, factor);
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

	static math::TDecomposedTransform<real> makeRandomTransform(const IntersectConfig& config)
	{
		return math::TDecomposedTransform<real>(
			makeRandomPoint(config),
			makeRandomRotate(config),
			makeRandomPoint(config));
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
			result.expectedHitNormal = triangle.getFaceNormal();

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
			entry.max = std::min(otherEntry.max, entry.max);
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
		.errorVsDistChart = ChartData(10000, 1e-8_r, 1e8_r),
		.errorVsSizeChart = ChartData(10000, 1e-8_r, 1e8_r)};

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
	const std::size_t numThreads)
{
	PH_ASSERT_GT(numThreads, 0);

	std::jthread statsThread([](std::stop_token token)
	{
		while(!token.stop_requested())
		{
			PH_LOG(IntersectError, Note,
				"Intersects {} objects.", g_numIntersects.load(std::memory_order_relaxed));
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	});

	std::vector<ChartDataCollection> threadCharts(numThreads);
	parallel_work(config.numObjsPerCase, numThreads,
		[&cases, &config, &threadCharts]
		(const std::size_t workerIdx, const std::size_t workBegin, const std::size_t workEnd)
		{
			IntersectConfig threadConfig = config;
			threadConfig.numObjsPerCase = workEnd - workBegin;

			threadCharts[workerIdx] = run_cases(cases, threadConfig);
		});
	statsThread.request_stop();

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

	constexpr bool favorSmallerDistance = true;
	constexpr auto numThreads = 10;

	IntersectConfig config;
	config.numObjsPerCase = 10000000;
	config.numRaysPerObj = 16;
	config.minDistance = 1e-6_r;
	config.maxDistance = 1e6_r;
	config.minRotateDegs = -7200;
	config.maxRotateDegs = 7200;

	if constexpr(favorSmallerDistance)
	{
		std::vector<real> weights(1000);
		for(std::size_t wi = 0; wi < weights.size(); ++wi)
		{
			weights[wi] = static_cast<real>((weights.size() - wi) / weights.size());
		}
		config.distanceDistribution = math::TPwcDistribution1D<real>(weights);
	}
	else
	{
		config.distanceDistribution = math::TPwcDistribution1D<real>({1});
	}

	std::vector<std::unique_ptr<IntersectCase>> cases;
	cases.push_back(std::make_unique<TriangleCase>());

	PH_LOG(IntersectError, Note,
		"Run intersection cases using {} threads.", numThreads);

	ChartDataCollection charts = run_cases_parallel(cases, config, numThreads);

	timer.stop();

	PH_LOG(IntersectError, Note, 
		"Intersects {} objects.", g_numIntersects.load(std::memory_order_relaxed));
	PH_LOG(IntersectError, Note,
		"Time spent: {} s.", timer.getDeltaS());

	charts.errorVsDistChart.saveAsCsv(get_script_directory(EEngineProject::IntersectError)
		/ Path("triangle_error_vs_dist.csv"));
	charts.errorVsSizeChart.saveAsCsv(get_script_directory(EEngineProject::IntersectError)
		/ Path("triangle_error_vs_size.csv"));

	return exit_render_engine() ? EXIT_SUCCESS : EXIT_FAILURE;
}
