#include "Common/stats.h"
#include "Common/assertion.h"

#include <algorithm>
#include <cmath>
#include <format>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace ph
{

namespace detail::stats
{

using Clock = TimeCounter::Clock;
using ClockRep = Clock::duration::rep;

void TimingRecord::addSpan(const ClockRep elapsedTicks)
{
	totalTicks += elapsedTicks;
	++count;
	minTicks = std::min(minTicks, elapsedTicks);
	maxTicks = std::max(maxTicks, elapsedTicks);
}

void TimingRecord::merge(const TimingRecord& other)
{
	totalTicks += other.totalTicks;
	count += other.count;
	minTicks = std::min(minTicks, other.minTicks);
	maxTicks = std::max(maxTicks, other.maxTicks);
}

void TimingRecord::clear()
{
	counter = nullptr;
	totalTicks = 0;
	count = 0;
	minTicks = std::numeric_limits<ClockRep>::max();
	maxTicks = 0;
}

class TimingCentralStorage final
{
public:
	static TimingCentralStorage& INSTANCE()
	{
		static auto instance = std::make_unique<TimingCentralStorage>();
		return *instance;
	}

	void registerCounter(TimeCounter& counter)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		for(const TimeCounter& existingCounter : m_counters)
		{
			PH_ASSERT_MSG(
				existingCounter.name != counter.name || existingCounter.path != counter.path,
				"Timer stat full paths must be unique.");
		}

		PH_ASSERT_MSG(
			m_counters.size() <= std::numeric_limits<uint32>::max(),
			"Maximum number of timing counters exceeded.");
		counter.id = static_cast<uint32>(m_counters.size());
		m_counters.push_back(counter);
	}

	void mergeLocalRecords(const std::vector<TimingRecord>& localRecords)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		if(m_records.size() < m_counters.size())
		{
			m_records.resize(m_counters.size());
		}

		for(std::size_t counterIdx = 0; counterIdx < localRecords.size(); ++counterIdx)
		{
			if(localRecords[counterIdx].count == 0)
			{
				continue;
			}

			PH_ASSERT_LT(counterIdx, m_records.size());
			m_records[counterIdx].counter = &m_counters[counterIdx].get();
			m_records[counterIdx].merge(localRecords[counterIdx]);
		}
	}

	void copyTimingRecords(std::vector<TimingRecord>& out_records)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		out_records.clear();
		for(const TimingRecord& record : m_records)
		{
			if(record.counter && record.count != 0)
			{
				out_records.push_back(record);
			}
		}
	}

private:
	mutable std::mutex m_mutex;
	std::vector<std::reference_wrapper<const TimeCounter>> m_counters;
	std::vector<TimingRecord> m_records;
};

class TimingLocalStorage final
{
public:
	static TimingLocalStorage& INSTANCE()
	{
		thread_local TimingLocalStorage instance;
		return instance;
	}

	~TimingLocalStorage()
	{
		flush();
	}

	void addSpan(
		const TimeCounter& counter,
		const Clock::time_point startTime,
		const Clock::time_point endTime)
	{
		if(m_records.size() <= counter.id)
		{
			m_records.resize(static_cast<std::size_t>(counter.id) + 1);
		}

		m_records[counter.id].addSpan((endTime - startTime).count());
	}

	void flush()
	{
		TimingCentralStorage::INSTANCE().mergeLocalRecords(m_records);

		for(TimingRecord& record : m_records)
		{
			record.clear();
		}
	}

private:
	std::vector<TimingRecord> m_records;
};

TimeCounter::TimeCounter(std::string name, std::string path)
	: name(std::move(name))
	, path(std::move(path))
	, id(0)
{
	TimingCentralStorage::INSTANCE().registerCounter(*this);
}

void TimeCounter::addSpan(
	const Clock::time_point startTime,
	const Clock::time_point endTime) const
{
	TimingLocalStorage::INSTANCE().addSpan(*this, startTime, endTime);
}

}// end namespace detail::stats

uint64 TimerStatsReport::ticksToMicroseconds(const TimingClockRep ticks)
{
	return static_cast<uint64>(
		std::chrono::duration_cast<std::chrono::microseconds>(
			detail::stats::TimeCounter::Clock::duration(ticks)).count());
}

std::string TimerStatsReport::formatDuration(const uint64 microseconds, const uint64 count)
{
	const float64 displayMicroseconds = count != 0
		? static_cast<float64>(microseconds) / static_cast<float64>(count)
		: static_cast<float64>(microseconds);

	if(displayMicroseconds >= 1'000'000.0)
	{
		return std::format("{:.2f}s", displayMicroseconds / 1'000'000.0);
	}
	else if(displayMicroseconds >= 1'000.0)
	{
		return std::format("{:.2f}ms", displayMicroseconds / 1'000.0);
	}
	else if(count != 0)
	{
		return std::format("{:.2f}us", displayMicroseconds);
	}

	return std::format("{}us", microseconds);
}

std::string TimerStatsReport::formatCallCount(const uint64 count)
{
	constexpr uint64 scientificCountThreshold = 100'000;

	if(count < scientificCountThreshold)
	{
		return std::format("{}", count);
	}

	uint32 exponent = static_cast<uint32>(std::floor(
		std::log10(static_cast<float64>(count))));
	uint32 mantissa = static_cast<uint32>(std::round(
		static_cast<float64>(count) / std::pow(10.0, exponent) * 1000.0));
	if(mantissa == 10000)
	{
		mantissa = 1000;
		++exponent;
	}

	return std::format(
		"{}.{}x10^{:02}",
		mantissa / 1000,
		std::format("{:03}", mantissa % 1000),
		exponent);
}

std::string TimerStatsReport::formatShare(
	const uint64 totalMicroseconds,
	const uint64 parentTotalMicroseconds)
{
	if(parentTotalMicroseconds == 0)
	{
		return "-";
	}

	return std::format(
		"{:.2f}x",
		static_cast<float64>(totalMicroseconds) / static_cast<float64>(parentTotalMicroseconds));
}

std::string TimerStatsReport::fitNameColumn(const std::string& name)
{
	constexpr std::size_t maxNameLength = 21;

	if(name.size() <= maxNameLength)
	{
		return name;
	}

	return name.substr(0, maxNameLength - 1) + "~";
}

std::string TimerStatsReport::makeFullTimingPath(const detail::stats::TimeCounter& counter)
{
	if(counter.path.empty())
	{
		return counter.name;
	}

	return counter.path + "." + counter.name;
}

std::string TimerStatsReport::makeParentTimingPath(const std::string& path)
{
	const auto parentEnd = path.rfind('.');
	return parentEnd != std::string::npos
		? path.substr(0, parentEnd)
		: "";
}

std::string TimerStatsReport::makeTimingPathName(const std::string& path)
{
	const auto nameBegin = path.rfind('.');
	return nameBegin != std::string::npos
		? path.substr(nameBegin + 1)
		: path;
}

void TimerStatsReport::addStatsFromNode(
	TimerStatsReport::TimingNode&       receivingNode,
	const TimerStatsReport::TimingNode& sourceNode)
{
	if(sourceNode.count == 0)
	{
		return;
	}

	receivingNode.totalMicroseconds += sourceNode.totalMicroseconds;
	receivingNode.minMicroseconds = std::min(
		receivingNode.minMicroseconds,
		sourceNode.minMicroseconds);
	receivingNode.maxMicroseconds = std::max(
		receivingNode.maxMicroseconds,
		sourceNode.maxMicroseconds);
	receivingNode.count += sourceNode.count;
}

TimerStatsReport::TimingNode TimerStatsReport::makeTimingNode(
	const std::map<std::string, TimerStatsReport::TimingNode>& pathToNodes,
	const std::string& path)
{
	TimerStatsReport::TimingNode node = pathToNodes.at(path);
	const bool hasOwnStats = node.count != 0;

	for(const auto& childPathAndNode : pathToNodes)
	{
		const std::string& childPath = childPathAndNode.first;
		if(makeParentTimingPath(childPath) != path)
		{
			continue;
		}

		TimerStatsReport::TimingNode childNode = makeTimingNode(pathToNodes, childPath);
		if(!hasOwnStats)
		{
			addStatsFromNode(node, childNode);
		}
		node.subgroups.push_back(std::move(childNode));
	}

	return node;
}

TimerStatsReport::TimingNode TimerStatsReport::makeRootTimingNode(
	const std::vector<detail::stats::TimingRecord>& records)
{
	std::map<std::string, TimerStatsReport::TimingNode> pathToNodes;

	for(const detail::stats::TimingRecord& record : records)
	{
		PH_ASSERT(record.counter);

		const std::string fullPath = makeFullTimingPath(*record.counter);
		for(std::size_t pathEnd = fullPath.find('.');
		    pathEnd != std::string::npos;
		    pathEnd = fullPath.find('.', pathEnd + 1))
		{
			const std::string path = fullPath.substr(0, pathEnd);
			pathToNodes.try_emplace(path).first->second.groupName = makeTimingPathName(path);
		}

		TimerStatsReport::TimingNode& node = pathToNodes.try_emplace(fullPath).first->second;
		PH_ASSERT_MSG(node.count == 0,
			"Duplicate timer stat full path found: " + fullPath);
		if(node.count != 0)
		{
			continue;
		}

		node.groupName = makeTimingPathName(fullPath);
		node.totalMicroseconds = ticksToMicroseconds(record.totalTicks);
		node.minMicroseconds = ticksToMicroseconds(record.minTicks);
		node.maxMicroseconds = ticksToMicroseconds(record.maxTicks);
		node.count = record.count;
	}

	// Build the display tree from sorted full paths. Nodes without own samples
	// are category nodes and derive their totals from direct children.
	TimerStatsReport::TimingNode rootNode;
	for(const auto& pathAndNode : pathToNodes)
	{
		const std::string& path = pathAndNode.first;
		if(!makeParentTimingPath(path).empty())
		{
			continue;
		}

		TimerStatsReport::TimingNode childNode = makeTimingNode(pathToNodes, path);
		addStatsFromNode(rootNode, childNode);
		rootNode.subgroups.push_back(std::move(childNode));
	}

	return rootNode;
}

TimerStatsReport::TimerStatsReport()
	: m_records()
{
	detail::stats::TimingLocalStorage::INSTANCE().flush();

	detail::stats::TimingCentralStorage::INSTANCE().copyTimingRecords(m_records);
}

void TimerStatsReport::flushTimerStats()
{
	detail::stats::TimingLocalStorage::INSTANCE().flush();
}

TimerStatsReport::TimingNode TimerStatsReport::getRootTimingNode() const
{
	return makeRootTimingNode(m_records);
}

std::string TimerStatsReport::detailedReport() const
{
	TimingNode rootNode = getRootTimingNode();
	if(rootNode.groupName.empty())
	{
		rootNode.groupName = "Timing Report";
	}

	std::string reportStr;
	reportStr += rootNode.groupName + "\n";
	reportStr += std::format(
		"avg=total/calls, share=total/parent-total, timers={}, records={}\n",
		m_records.size(), m_records.size());
	reportStr += std::string(80, '-') + "\n";
	reportStr += std::format(
		"{:<21} {:>9} {:>9} {:>11} {:>9} {:>9} {:>6}\n",
		"name", "avg", "total", "calls", "min", "max", "share");
	reportStr += std::string(80, '-') + "\n";

	for(const TimingNode& subgroup : rootNode.subgroups)
	{
		reportStr += makeTimingNodeReportRecursive(
			subgroup,
			"",
			rootNode.totalMicroseconds);
	}

	reportStr += std::string(80, '-') + "\n";

	return reportStr;
}

std::string TimerStatsReport::makeTimingNodeReportRecursive(
	const TimerStatsReport::TimingNode& node,
	const std::string&                  linePrefix,
	const uint64                        parentTotalMicroseconds)
{
	std::string reportStr;
	reportStr.reserve(128 * node.subgroups.size());

	reportStr += std::format(
		"{:<21} {:>9} {:>9} {:>11} {:>9} {:>9} {:>6}\n",
		fitNameColumn(linePrefix + node.groupName),
		node.count != 0 ? formatDuration(node.totalMicroseconds, node.count) : "-",
		formatDuration(node.totalMicroseconds),
		formatCallCount(node.count),
		node.count != 0 ? formatDuration(node.minMicroseconds) : "-",
		node.count != 0 ? formatDuration(node.maxMicroseconds) : "-",
		formatShare(node.totalMicroseconds, parentTotalMicroseconds));

	for(const TimerStatsReport::TimingNode& subgroup : node.subgroups)
	{
		reportStr += makeTimingNodeReportRecursive(
			subgroup,
			linePrefix + "  ",
			node.totalMicroseconds);
	}

	return reportStr;
}

}// end namespace ph
