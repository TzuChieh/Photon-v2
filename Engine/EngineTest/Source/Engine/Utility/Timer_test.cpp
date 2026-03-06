#include <Engine/Utility/Timer.h>

#include <gtest/gtest.h>

#include <thread>
#include <chrono>

using namespace ph;

TEST(TimerTest, InitialStateIsZero)
{
	// Logic Derivation:
	// 1. A default constructed Timer should have no accumulated duration.
	// 2. Both high-precision (ns) and coarse-grained (ms) queries should return 0.
	Timer timer;

	EXPECT_EQ(timer.getDeltaNs(), 0);
	EXPECT_EQ(timer.getDeltaMs(), 0);
	EXPECT_EQ(timer.getDelta().count(), 0);
}

TEST(TimerTest, MeasuresElapsedTimeCorrectly)
{
	// Logic Derivation:
	// 1. We record the current time using the same clock source as the Timer.
	// 2. We start the timer, sleep for a short duration, and stop it.
	// 3. We record the end time immediately after stopping.
	// 4. The Timer's reported delta should be equal to the manually calculated duration (End - Start).
	Timer timer;

	const auto startTime = Timer::Clock::now();
	timer.start();
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	timer.stop();
	const auto endTime = Timer::Clock::now();

	// We only check for GE as the sleep time cannot be guaranteed
	EXPECT_GE(
		std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count(),
		timer.getDeltaMs());
}

TEST(TimerTest, AccumulatesTimeAcrossMultipleSessions)
{
	// Logic Derivation:
	// 1. Run the timer for a short period (Segment A). Stop it.
	// 2. Resume the timer using accumulatedStart() for another period (Segment B). Stop it.
	// 3. The total reported time should be the sum of actual elapsed times of Segment A and Segment B.
	//    It should NOT include the gap between A and B.
	Timer timer;

	// Segment A
	const auto startA = Timer::Clock::now();
	timer.start();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	timer.stop();
	const auto endA = Timer::Clock::now();

	// Gap (not measured)
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	// Segment B
	const auto startB = Timer::Clock::now();
	timer.accumulatedStart();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	timer.stop();
	const auto endB = Timer::Clock::now();

	const auto actualDurationA = endA - startA;
	const auto actualDurationB = endB - startB;

	// We only check for GE as the sleep time cannot be guaranteed
	EXPECT_GE(
		std::chrono::duration_cast<std::chrono::milliseconds>(actualDurationA + actualDurationB).count(),
		timer.getDeltaMs());
}

TEST(TimerTest, MarkLapReturnsIntervalAndUpdatesTotal)
{
	// Logic Derivation:
	// 1. Start timer. Sleep for Interval 1.
	// 2. markLap() should return approximately Interval 1.
	// 3. Sleep for Interval 2.
	// 4. markLap() should return approximately Interval 2.
	// 5. Total delta should be the sum of Interval 1 and Interval 2.
	Timer timer;
	const auto startTime = Timer::Clock::now();
	timer.start();

	// Interval 1
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	const auto mark1Time = Timer::Clock::now();
	const auto lap1Ms = timer.markLapMs();

	// Interval 2
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	const auto mark2Time = Timer::Clock::now();
	const auto lap2Ms = timer.markLapMs();

	const auto expectedLap1 = std::chrono::duration_cast<std::chrono::milliseconds>(mark1Time - startTime).count();
	const auto expectedLap2 = std::chrono::duration_cast<std::chrono::milliseconds>(mark2Time - mark1Time).count();

	// Verify each lap matches the manual timing
	// We only check for GE as the sleep time cannot be guaranteed
	EXPECT_GE(lap1Ms, expectedLap1);
	EXPECT_GE(lap2Ms, expectedLap2);

	// Verify total duration includes both laps exactly (since markLap adds to total)
	EXPECT_EQ(timer.getDeltaMs(), lap1Ms + lap2Ms);
}

TEST(TimerTest, PeekLapDoesNotResetInterval)
{
	// Logic Derivation:
	// 1. Start timer. Sleep.
	// 2. peekLap() returns current interval.
	// 3. Sleep more.
	// 4. peekLap() returns larger interval (includes previous sleep).
	// 5. markLap() returns the total interval (confirming peek didn't reset).
	Timer timer;
	timer.start();

	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	const auto peek1 = timer.peekLap();

	std::this_thread::sleep_for(std::chrono::milliseconds(5));
	const auto peek2 = timer.peekLap();

	// The second peek should be strictly larger than the first
	EXPECT_LT(peek1.count(), peek2.count());

	// Ensure peeking didn't consume the time (reset the lap marker)
	const auto lap = timer.markLap();
	EXPECT_GE(lap.count(), peek2.count());
}

TEST(TimerTest, SupportsMultipleTimeUnits)
{
	// Logic Derivation:
	// 1. Run timer for a known duration (e.g., 20ms).
	// 2. Verify conversions to us and ms match expected ratios.
	Timer timer;
	timer.start();
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	timer.stop();

	const auto deltaMs = timer.getDeltaMs();
	const auto deltaUs = timer.getDeltaUs();
	
	// Basic validation that we measured *something*
	EXPECT_GE(deltaMs, 20);

	// Verify relationship between ms and us (allow small floating point error if any)
	// 1 ms = 1000 us
	EXPECT_NEAR(static_cast<double>(deltaUs) / 1000.0, static_cast<double>(deltaMs), 1.0);
}
