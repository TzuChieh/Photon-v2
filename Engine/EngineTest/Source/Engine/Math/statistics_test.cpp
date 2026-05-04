#include <Engine/Math/statistics.h>

#include <gtest/gtest.h>

#include <vector>
#include <limits>

using namespace ph::math;

TEST(StatisticsTest, SidakCorrection)
{
	constexpr double smallError = 1e-9;

	// Single test should have no correction
	EXPECT_NEAR(sidak_correction(0.05, 1), 0.05, smallError);

	// Common statistical scenario
	EXPECT_NEAR(sidak_correction(0.05, 5), 0.010206218313011494736, smallError);

	// Stricter alpha
	EXPECT_NEAR(sidak_correction(0.01, 10), 0.0010045287082499632092, smallError);

	// Large number of tests
	EXPECT_NEAR(sidak_correction(0.05, 100), 0.00051280141626230957286, smallError);
}

TEST(StatisticsTest, Chi2Cdf)
{
	// Testing by calling R's `qchisq()` as follows
	// `print(qchisq(p, dof, lower.tail=FALSE), d=22)`

	constexpr double smallError = 1e-12;
	constexpr double mediumError = 1e-9;
	constexpr double largeError = 1e-4;

	EXPECT_NEAR(chi2_p_value(3.841458820694125808615, 1), 0.05, smallError);
	EXPECT_NEAR(chi2_p_value(2.705543454095415523142, 1), 0.10, smallError);
	EXPECT_NEAR(chi2_p_value(0.1484718618325455152807, 1), 0.70, smallError);
	EXPECT_NEAR(chi2_p_value(0.00393214000001953015162, 1), 0.95, smallError);

	EXPECT_NEAR(chi2_p_value(5.991464547107979043972, 2), 0.05, smallError);
	EXPECT_NEAR(chi2_p_value(4.605170185988091802187, 2), 0.10, smallError);
	EXPECT_NEAR(chi2_p_value(0.713349887877464894359, 2), 0.70, smallError);
	EXPECT_NEAR(chi2_p_value(0.1025865887751011562568, 2), 0.95, smallError);

	EXPECT_NEAR(chi2_p_value(19.02074334820110479427, 10), 0.04, smallError);
	EXPECT_NEAR(chi2_p_value(9.341817765591969191519, 10), 0.5, smallError);
	EXPECT_NEAR(chi2_p_value(3.696541444956689570489, 10), 0.96, smallError);

	EXPECT_NEAR(chi2_p_value(68.80386402631833675514, 50), 0.04, smallError);
	EXPECT_NEAR(chi2_p_value(49.33493673397684631254, 50), 0.5, smallError);
	EXPECT_NEAR(chi2_p_value(33.94258999717953173558, 50), 0.96, smallError);

	EXPECT_NEAR(chi2_p_value(232.0003307125071216888, 200), 0.06, smallError);
	EXPECT_NEAR(chi2_p_value(189.0485987177614788379, 200), 0.7, smallError);
	EXPECT_NEAR(chi2_p_value(171.3086735204458932458, 200), 0.93, smallError);

	EXPECT_NEAR(chi2_p_value(1070.458677799041197432, 1000), 0.06, smallError);
	EXPECT_NEAR(chi2_p_value(976.07359125777418285, 1000), 0.7, smallError);
	EXPECT_NEAR(chi2_p_value(934.8035378152002294883, 1000), 0.93, smallError);

	EXPECT_NEAR(chi2_p_value(10233.74889767793683859, 10000), 0.05, mediumError);
	EXPECT_NEAR(chi2_p_value(10073.67533170680508192, 10000), 0.3, mediumError);
	EXPECT_NEAR(chi2_p_value(9768.525135667536233086, 10000), 0.95, mediumError);

	EXPECT_NEAR(chi2_p_value(100736.7361773190059466, 100000), 0.05, mediumError);
	EXPECT_NEAR(chi2_p_value(100234.0348271952680079, 100000), 0.3, mediumError);
	EXPECT_NEAR(chi2_p_value(99265.53787816064141225, 100000), 0.95, mediumError);

	EXPECT_NEAR(chi2_p_value(5005202.620659505017102, 5000000), 0.05, mediumError);
	EXPECT_NEAR(chi2_p_value(5001657.816566350869834, 5000000), 0.3, mediumError);
	EXPECT_NEAR(chi2_p_value(4994799.653398384340107, 5000000), 0.95, mediumError);

	EXPECT_NEAR(chi2_p_value(50000520149.52490234375, 50000000000), 0.05, largeError);
	EXPECT_NEAR(chi2_p_value(50000165829.51929473877, 50000000000), 0.3, largeError);
	EXPECT_NEAR(chi2_p_value(49999479852.74915313721, 50000000000), 0.95, largeError);

	// Edge case: chi^2 = 0 should yield p = 1
	EXPECT_NEAR(chi2_p_value(0.0, 0), 1, smallError);
	EXPECT_NEAR(chi2_p_value(0.0, 1), 1, smallError);

	// Edge case: chi^2 = Inf (or very large) should yield p = 0
	if constexpr(std::numeric_limits<double>::has_infinity)
	{
		EXPECT_NEAR(chi2_p_value(std::numeric_limits<double>::infinity(), 0), 0, smallError);
		EXPECT_NEAR(chi2_p_value(std::numeric_limits<double>::infinity(), 1), 0, smallError);
	}
	else
	{
		EXPECT_NEAR(chi2_p_value(std::numeric_limits<double>::max(), 0), 0, smallError);
		EXPECT_NEAR(chi2_p_value(std::numeric_limits<double>::max(), 1), 0, smallError);
	}
}

TEST(StatisticsTest, Chi2)
{
	// Same observed and expected frequencies should yield chi^2 = 0
	{
		const std::vector<float> of = {1, 1};
		const std::vector<float> ef = {1, 1};
		const auto [x, dof] = chi2<float, int>(of, ef);
		EXPECT_EQ(x, 0);
		EXPECT_EQ(dof, 2 - 1);
	}

	// Same observed and expected frequencies should yield chi^2 = 0
	{
		const std::vector<float> of = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		const std::vector<float> ef = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		const auto [x, dof] = chi2<float, int>(of, ef);
		EXPECT_EQ(x, 0);
		EXPECT_EQ(dof, 10 - 1);
	}
	
	// A simple non-zero case
	{
		const std::vector<float> of = {2, 2};
		const std::vector<float> ef = {1, 1};
		const auto [x, dof] = chi2<float, int>(of, ef);
		EXPECT_EQ(x, 1 + 1);
		EXPECT_EQ(dof, 2 - 1);
	}
}

TEST(StatisticsTest, WeightedWelfordAdd)
{
	constexpr double smallError = 1e-12;

	double weightSum = 0;
	double weightSquaredSum = 0;
	double mean = 0;
	double sumSquaredDiff = 0;

	weighted_welford_add(1.0, 1.0, weightSum, weightSquaredSum, mean, sumSquaredDiff);
	weighted_welford_add(2.0, 1.0, weightSum, weightSquaredSum, mean, sumSquaredDiff);
	weighted_welford_add(3.0, 1.0, weightSum, weightSquaredSum, mean, sumSquaredDiff);

	EXPECT_NEAR(weightSum, 3.0, smallError);
	EXPECT_NEAR(weightSquaredSum, 3.0, smallError);
	EXPECT_NEAR(mean, 2.0, smallError);
	EXPECT_NEAR(sumSquaredDiff, 2.0, smallError);

	EXPECT_NEAR(weighted_welford_population_variance(weightSum, sumSquaredDiff), 2.0 / 3.0, smallError);
	EXPECT_NEAR(weighted_welford_unbiased_variance(weightSum, weightSquaredSum, sumSquaredDiff), 1.0, smallError);
}

TEST(StatisticsTest, WeightedWelfordAddWithWeights)
{
	constexpr double smallError = 1e-12;

	double weightSum = 0;
	double weightSquaredSum = 0;
	double mean = 0;
	double sumSquaredDiff = 0;

	weighted_welford_add(1.0, 1.0, weightSum, weightSquaredSum, mean, sumSquaredDiff);
	weighted_welford_add(3.0, 3.0, weightSum, weightSquaredSum, mean, sumSquaredDiff);

	EXPECT_NEAR(weightSum, 4.0, smallError);
	EXPECT_NEAR(weightSquaredSum, 10.0, smallError);
	EXPECT_NEAR(mean, 2.5, smallError);
	EXPECT_NEAR(sumSquaredDiff, 3.0, smallError);

	EXPECT_NEAR(weighted_welford_population_variance(weightSum, sumSquaredDiff), 0.75, smallError);
	EXPECT_NEAR(weighted_welford_unbiased_variance(weightSum, weightSquaredSum, sumSquaredDiff), 2.0, smallError);
}

TEST(StatisticsTest, WeightedWelfordZeroWeightIgnored)
{
	constexpr double smallError = 1e-12;

	double weightSum = 0;
	double weightSquaredSum = 0;
	double mean = 0;
	double sumSquaredDiff = 0;

	weighted_welford_add(2.0, 0.0, weightSum, weightSquaredSum, mean, sumSquaredDiff);

	EXPECT_NEAR(weightSum, 0.0, smallError);
	EXPECT_NEAR(weightSquaredSum, 0.0, smallError);
	EXPECT_NEAR(mean, 0.0, smallError);
	EXPECT_NEAR(sumSquaredDiff, 0.0, smallError);
}

TEST(StatisticsTest, WeightedWelfordMerge)
{
	constexpr double smallError = 1e-12;

	// Ground truth accumulation.
	double gtWeightSum = 0;
	double gtWeightSquaredSum = 0;
	double gtMean = 0;
	double gtSumSquaredDiff = 0;

	weighted_welford_add(1.5, 2.0, gtWeightSum, gtWeightSquaredSum, gtMean, gtSumSquaredDiff);
	weighted_welford_add(4.0, 1.0, gtWeightSum, gtWeightSquaredSum, gtMean, gtSumSquaredDiff);
	weighted_welford_add(5.0, 3.0, gtWeightSum, gtWeightSquaredSum, gtMean, gtSumSquaredDiff);
	weighted_welford_add(-1.0, 2.0, gtWeightSum, gtWeightSquaredSum, gtMean, gtSumSquaredDiff);

	// Split accumulation, then merge.
	double weightSumA = 0;
	double weightSquaredSumA = 0;
	double meanA = 0;
	double sumSquaredDiffA = 0;

	double weightSumB = 0;
	double weightSquaredSumB = 0;
	double meanB = 0;
	double sumSquaredDiffB = 0;

	weighted_welford_add(1.5, 2.0, weightSumA, weightSquaredSumA, meanA, sumSquaredDiffA);
	weighted_welford_add(4.0, 1.0, weightSumA, weightSquaredSumA, meanA, sumSquaredDiffA);

	weighted_welford_add(5.0, 3.0, weightSumB, weightSquaredSumB, meanB, sumSquaredDiffB);
	weighted_welford_add(-1.0, 2.0, weightSumB, weightSquaredSumB, meanB, sumSquaredDiffB);

	weighted_welford_merge(
		weightSumB, weightSquaredSumB, meanB, sumSquaredDiffB,
		weightSumA, weightSquaredSumA, meanA, sumSquaredDiffA);

	EXPECT_NEAR(weightSumA, gtWeightSum, smallError);
	EXPECT_NEAR(weightSquaredSumA, gtWeightSquaredSum, smallError);
	EXPECT_NEAR(meanA, gtMean, smallError);
	EXPECT_NEAR(sumSquaredDiffA, gtSumSquaredDiff, smallError);
}

TEST(StatisticsTest, WeightedWelfordInvalidUnbiasedDenominator)
{
	constexpr double smallError = 1e-12;

	double weightSum = 0;
	double weightSquaredSum = 0;
	double mean = 0;
	double sumSquaredDiff = 0;

	weighted_welford_add(10.0, 2.0, weightSum, weightSquaredSum, mean, sumSquaredDiff);

	EXPECT_NEAR(weighted_welford_unbiased_variance(weightSum, weightSquaredSum, sumSquaredDiff), 0.0, smallError);
}

TEST(StatisticsTest, WeightedWelfordInvalidVarianceInputsAreClamped)
{
	constexpr double smallError = 1e-12;

	// Population variance: invalid total weight.
	EXPECT_NEAR(weighted_welford_population_variance(0.0, 10.0), 0.0, smallError);

	// Unbiased variance: invalid denominator (w - w2 / w <= 0).
	EXPECT_NEAR(weighted_welford_unbiased_variance(0.0, 0.0, 10.0), 0.0, smallError);
	EXPECT_NEAR(weighted_welford_unbiased_variance(1.0, 1.0, 10.0), 0.0, smallError);

	// Both variance functions clamp negative outputs.
	EXPECT_NEAR(weighted_welford_population_variance(2.0, -1.0), 0.0, smallError);
	EXPECT_NEAR(weighted_welford_unbiased_variance(2.0, 1.0, -1.0), 0.0, smallError);
}
