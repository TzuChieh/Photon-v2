#include <Common/stats.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <thread>

using namespace ph;

TEST(TimerStatsReportTest, RecordsManualSpans)
{
	static detail::stats::TimeCounter counter("Counter", "StatsManualSpans");

	// GoogleTest can repeat tests in one process,
	// but we want to only add stats once.
	static bool hasRecorded = false;
	if(!hasRecorded)
	{
		const detail::stats::TimeCounter::Clock::time_point startTime;
		counter.addSpan(startTime, startTime + std::chrono::microseconds(1000));
		counter.addSpan(startTime, startTime + std::chrono::microseconds(2000));
		hasRecorded = true;
	}

	const auto rootNode = TimerStatsReport().getRootTimingNode();
	const auto rootIter = std::ranges::find_if(
		rootNode.subgroups,
		[](const TimerStatsReport::TimingNode& node)
		{
			return node.groupName == "StatsManualSpans";
		});

	ASSERT_NE(rootIter, rootNode.subgroups.end());
	ASSERT_EQ(rootIter->subgroups.size(), 1);

	const TimerStatsReport::TimingNode& counterNode = rootIter->subgroups.front();
	EXPECT_EQ(counterNode.groupName, "Counter");
	EXPECT_EQ(counterNode.count, 2);
	EXPECT_EQ(counterNode.totalMicroseconds, 3000);
	EXPECT_EQ(counterNode.minMicroseconds, 1000);
	EXPECT_EQ(counterNode.maxMicroseconds, 2000);
}

TEST(TimerStatsReportTest, BuildsDottedPathHierarchy)
{
	static detail::stats::TimeCounter alphaCounter("Alpha", "StatsHierarchy.Parent");
	static detail::stats::TimeCounter zetaCounter("Zeta", "StatsHierarchy.Parent");

	// GoogleTest can repeat tests in one process,
	// but we want to only add stats once.
	static bool hasRecorded = false;
	if(!hasRecorded)
	{
		const detail::stats::TimeCounter::Clock::time_point startTime;
		alphaCounter.addSpan(startTime, startTime + std::chrono::microseconds(1000));
		zetaCounter.addSpan(startTime, startTime + std::chrono::microseconds(2000));
		hasRecorded = true;
	}

	const auto rootNode = TimerStatsReport().getRootTimingNode();
	const auto rootIter = std::ranges::find_if(
		rootNode.subgroups,
		[](const TimerStatsReport::TimingNode& node)
		{
			return node.groupName == "StatsHierarchy";
		});

	ASSERT_NE(rootIter, rootNode.subgroups.end());
	ASSERT_EQ(rootIter->subgroups.size(), 1);

	const TimerStatsReport::TimingNode& parentNode = rootIter->subgroups.front();
	ASSERT_EQ(parentNode.groupName, "Parent");
	ASSERT_EQ(parentNode.subgroups.size(), 2);
	EXPECT_EQ(parentNode.count, 2);
	EXPECT_EQ(parentNode.totalMicroseconds, 3000);
	EXPECT_EQ(parentNode.subgroups[0].groupName, "Alpha");
	EXPECT_EQ(parentNode.subgroups[1].groupName, "Zeta");
}

TEST(TimerStatsReportTest, TimedParentKeepsOwnStats)
{
	// Important as a parent timing scope may contain many untimed regions,
	// child timings likely will not add up to parent.

	static detail::stats::TimeCounter parentCounter("Parent", "StatsTimedParent");
	static detail::stats::TimeCounter childCounter("Child", "StatsTimedParent.Parent");

	// GoogleTest can repeat tests in one process,
	// but we want to only add stats once.
	static bool hasRecorded = false;
	if(!hasRecorded)
	{
		const detail::stats::TimeCounter::Clock::time_point startTime;
		parentCounter.addSpan(startTime, startTime + std::chrono::microseconds(2000));
		childCounter.addSpan(startTime, startTime + std::chrono::microseconds(1000));
		hasRecorded = true;
	}

	const auto rootNode = TimerStatsReport().getRootTimingNode();
	const auto rootIter = std::ranges::find_if(
		rootNode.subgroups,
		[](const TimerStatsReport::TimingNode& node)
		{
			return node.groupName == "StatsTimedParent";
		});

	ASSERT_NE(rootIter, rootNode.subgroups.end());
	ASSERT_EQ(rootIter->subgroups.size(), 1);

	const TimerStatsReport::TimingNode& parentNode = rootIter->subgroups.front();
	ASSERT_EQ(parentNode.groupName, "Parent");
	ASSERT_EQ(parentNode.subgroups.size(), 1);
	EXPECT_EQ(parentNode.count, 1);
	EXPECT_EQ(parentNode.totalMicroseconds, 2000);
	EXPECT_EQ(parentNode.subgroups.front().groupName, "Child");
	EXPECT_EQ(parentNode.subgroups.front().count, 1);
	EXPECT_EQ(parentNode.subgroups.front().totalMicroseconds, 1000);
}

TEST(TimerStatsReportTest, ScopedTimerRecordsElapsedTime)
{
	static detail::stats::TimeCounter counter("Counter", "StatsScopedTimer");

	// GoogleTest can repeat tests in one process,
	// but we want to only add stats once.
	static bool hasRecorded = false;
	if(!hasRecorded)
	{
		{
			detail::stats::ScopedTimer scopedTimer(counter);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		hasRecorded = true;
	}

	const auto rootNode = TimerStatsReport().getRootTimingNode();
	const auto rootIter = std::ranges::find_if(
		rootNode.subgroups,
		[](const TimerStatsReport::TimingNode& node)
		{
			return node.groupName == "StatsScopedTimer";
		});

	ASSERT_NE(rootIter, rootNode.subgroups.end());
	ASSERT_EQ(rootIter->subgroups.size(), 1);
	EXPECT_EQ(rootIter->subgroups.front().count, 1);
	EXPECT_GT(rootIter->subgroups.front().totalMicroseconds, 0);
}
