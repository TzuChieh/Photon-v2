#include "engine_test_constants.h"

#include <Engine/Core/Filmic/HdrRgbVarianceFilm.h>
#include <Engine/Core/Filmic/SampleFilter.h>
#include <Engine/Frame/TFrame.h>
#include <Engine/Math/Color/Spectrum.h>
#include <Engine/Math/Color/spectral_samples.h>

#include <gtest/gtest.h>

using namespace ph;
using namespace ph::math;

TEST(HdrRgbVarianceFilmTest, DevelopsSampleVarianceToFrame)
{
	HdrRgbFrame frame(1, 1);
	HdrRgbVarianceFilm film(1, 1, SampleFilter::makeBox());

	// Samples are 1 and 3. Sample variance is:
	// mean = 2, M2 = (1 - 2)^2 + (3 - 2)^2 = 2, variance = M2 / (2 - 1) = 2.
	film.addRgbSample(0.5, 0.5, Vector3D(1.0, 1.0, 1.0));
	film.addRgbSample(0.5, 0.5, Vector3D(3.0, 3.0, 3.0));
	film.develop(frame);

	const auto pixel = frame.getPixel({0, 0});
	EXPECT_NEAR(pixel[0], 2.0_r, TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(pixel[1], 2.0_r, TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(pixel[2], 2.0_r, TEST_FLOAT32_EPSILON);
}

TEST(HdrRgbVarianceFilmTest, DevelopsVarianceWithZeroValuedSample)
{
	HdrRgbFrame frame(1, 1);
	HdrRgbVarianceFilm film(1, 1, SampleFilter::makeBox());

	// Samples are 0 and 2. Sample variance is:
	// mean = 1, M2 = (0 - 1)^2 + (2 - 1)^2 = 2, variance = M2 / (2 - 1) = 2.
	film.addRgbSample(0.5, 0.5, Vector3D(0.0, 0.0, 0.0));
	film.addRgbSample(0.5, 0.5, Vector3D(2.0, 2.0, 2.0));
	film.develop(frame);

	const auto pixel = frame.getPixel({0, 0});
	EXPECT_NEAR(pixel[0], 2.0_r, TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(pixel[1], 2.0_r, TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(pixel[2], 2.0_r, TEST_FLOAT32_EPSILON);
}

TEST(HdrRgbVarianceFilmTest, DevelopsTristimulusVariance)
{
	if constexpr(is_tristimulus(Spectrum::getColorSpace()))
	{
		Spectrum sampleA;
		sampleA[0] = 0.0_r;
		sampleA[1] = 0.0_r;
		sampleA[2] = 0.0_r;

		Spectrum sampleB;
		sampleB[0] = 2.0_r;
		sampleB[1] = 4.0_r;
		sampleB[2] = 6.0_r;

		HdrRgbVarianceFilm film(1, 1, SampleFilter::makeBox());
		film.addSample(0.5, 0.5, sampleA);
		film.addSample(0.5, 0.5, sampleB);

		HdrRgbFrame frame(1, 1);
		film.develop(frame);

		const auto pixel = frame.getPixel({0, 0});
		// Deviations to mean are +/-{1, 2, 3}; their squared sums are {2, 8, 18}.
		EXPECT_NEAR(pixel[0], 2.0_r, TEST_FLOAT32_EPSILON);
		EXPECT_NEAR(pixel[1], 8.0_r, TEST_FLOAT32_EPSILON);
		EXPECT_NEAR(pixel[2], 18.0_r, TEST_FLOAT32_EPSILON);
	}
}

TEST(HdrRgbVarianceFilmTest, DevelopsSpectralVariance)
{
	if constexpr(!is_tristimulus(Spectrum::getColorSpace()))
	{
		Spectrum sampleA;
		sampleA.setSpectral(resample_illuminant_D65<ColorValue>(), EColorUsage::EMR);
		Spectrum sampleB(sampleA);
		sampleB.mulLocal(3.0_r);

		HdrRgbVarianceFilm film(1, 1, SampleFilter::makeBox());
		film.addSample(0.5, 0.5, sampleA);
		film.addSample(0.5, 0.5, sampleB);

		HdrRgbFrame frame(1, 1);
		film.develop(frame);

		const auto pixel = frame.getPixel({0, 0});
		// D65 maps to 1 per channel, so samples 1 and 3 have sample variance 2.
		constexpr real ACCEPTABLE_ERROR = 0.001_r;
		for(int componentIdx = 0; componentIdx < 3; ++componentIdx)
		{
			EXPECT_NEAR(pixel[componentIdx], 2.0_r, ACCEPTABLE_ERROR);
		}
	}
}

TEST(HdrRgbVarianceFilmTest, MergeMatchesGroundTruth)
{
	const auto filter = SampleFilter::makeBox();

	HdrRgbVarianceFilm filmGroundTruth(1, 1, filter);
	filmGroundTruth.addRgbSample(0.5, 0.5, Vector3D(1.0, 1.0, 1.0));
	filmGroundTruth.addRgbSample(0.5, 0.5, Vector3D(2.0, 2.0, 2.0));
	filmGroundTruth.addRgbSample(0.5, 0.5, Vector3D(5.0, 5.0, 5.0));

	HdrRgbVarianceFilm filmA(1, 1, filter);
	filmA.addRgbSample(0.5, 0.5, Vector3D(1.0, 1.0, 1.0));
	filmA.addRgbSample(0.5, 0.5, Vector3D(2.0, 2.0, 2.0));

	HdrRgbVarianceFilm filmB(1, 1, filter);
	filmB.addRgbSample(0.5, 0.5, Vector3D(5.0, 5.0, 5.0));

	filmA.mergeWith(filmB);

	HdrRgbFrame mergedFrame(1, 1);
	HdrRgbFrame groundTruthFrame(1, 1);
	filmA.develop(mergedFrame);
	filmGroundTruth.develop(groundTruthFrame);

	const auto mergedPixel = mergedFrame.getPixel({0, 0});
	const auto gtPixel = groundTruthFrame.getPixel({0, 0});
	EXPECT_NEAR(mergedPixel[0], gtPixel[0], TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(mergedPixel[1], gtPixel[1], TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(mergedPixel[2], gtPixel[2], TEST_FLOAT32_EPSILON);
}

TEST(HdrRgbVarianceFilmTest, SetPixelProducesSingleSampleVariance)
{
	HdrRgbFrame frame(1, 1);
	HdrRgbVarianceFilm film(1, 1, SampleFilter::makeBox());

	film.addRgbSample(0.5, 0.5, Vector3D(1.0, 1.0, 1.0));
	film.addRgbSample(0.5, 0.5, Vector3D(3.0, 3.0, 3.0));

	// `setRgbPixel()` will overwrite all previous samples
	film.setRgbPixel(0.5, 0.5, Vector3D(9.0, 9.0, 9.0), 1.0);

	film.develop(frame);

	// Single sample, no variance
	const auto pixel = frame.getPixel({0, 0});
	EXPECT_NEAR(pixel[0], 0.0_r, TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(pixel[1], 0.0_r, TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(pixel[2], 0.0_r, TEST_FLOAT32_EPSILON);
}

TEST(HdrRgbVarianceFilmTest, ClearResetsFilmState)
{
	HdrRgbFrame frame(1, 1);
	HdrRgbVarianceFilm film(1, 1, SampleFilter::makeBox());

	film.addRgbSample(0.5, 0.5, Vector3D(1.0, 1.0, 1.0));
	film.addRgbSample(0.5, 0.5, Vector3D(3.0, 3.0, 3.0));
	film.clear();
	film.develop(frame);

	const auto pixel = frame.getPixel({0, 0});
	EXPECT_NEAR(pixel[0], 0.0_r, TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(pixel[1], 0.0_r, TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(pixel[2], 0.0_r, TEST_FLOAT32_EPSILON);
}

TEST(HdrRgbVarianceFilmTest, EmptyFilmDevelopsZeroVariance)
{
	HdrRgbFrame frame(1, 1);
	HdrRgbVarianceFilm film(1, 1, SampleFilter::makeBox());

	// No sample added
	film.develop(frame);

	const auto pixel = frame.getPixel({0, 0});
	EXPECT_NEAR(pixel[0], 0.0_r, TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(pixel[1], 0.0_r, TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(pixel[2], 0.0_r, TEST_FLOAT32_EPSILON);
}

TEST(HdrRgbVarianceFilmTest, NegativeWeightSetPixelIgnored)
{
	HdrRgbFrame frame(1, 1);
	HdrRgbVarianceFilm film(1, 1, SampleFilter::makeBox());

	// This would normally produce non-zero variance.
	film.addRgbSample(0.5, 0.5, Vector3D(1.0, 1.0, 1.0));
	film.addRgbSample(0.5, 0.5, Vector3D(3.0, 3.0, 3.0));

	// `setRgbPixel()` resets the pixel; negative weight then contributes nothing
	film.setRgbPixel(0.5, 0.5, Vector3D(9.0, 9.0, 9.0), -1.0);
	film.develop(frame);

	const auto pixel = frame.getPixel({0, 0});
	EXPECT_NEAR(pixel[0], 0.0_r, TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(pixel[1], 0.0_r, TEST_FLOAT32_EPSILON);
	EXPECT_NEAR(pixel[2], 0.0_r, TEST_FLOAT32_EPSILON);
}

TEST(HdrRgbVarianceFilmTest, MakeCopyCanSkipOrCopySamples)
{
	HdrRgbVarianceFilm film(1, 1, SampleFilter::makeBox());
	film.addRgbSample(0.5, 0.5, Vector3D(1.0, 1.0, 1.0));
	film.addRgbSample(0.5, 0.5, Vector3D(5.0, 5.0, 5.0));

	auto copiedWithoutSamplesRes = film.makeCopy(false);
	ASSERT_TRUE(copiedWithoutSamplesRes);

	auto copiedWithSamplesRes = film.makeCopy(true);
	ASSERT_TRUE(copiedWithSamplesRes);

	auto* copiedWithoutSamples = dynamic_cast<HdrRgbVarianceFilm*>(copiedWithoutSamplesRes.get());
	ASSERT_TRUE(copiedWithoutSamples);

	auto* copiedWithSamples = dynamic_cast<HdrRgbVarianceFilm*>(copiedWithSamplesRes.get());
	ASSERT_TRUE(copiedWithSamples);

	EXPECT_EQ(copiedWithoutSamples->getActualResPx(), film.getActualResPx());
	EXPECT_EQ(copiedWithoutSamples->getEffectiveWindowPx(), film.getEffectiveWindowPx());
	EXPECT_EQ(copiedWithSamples->getActualResPx(), film.getActualResPx());
	EXPECT_EQ(copiedWithSamples->getEffectiveWindowPx(), film.getEffectiveWindowPx());

	HdrRgbFrame srcFrame(1, 1);
	HdrRgbFrame copiedWithoutSamplesFrame(1, 1);
	HdrRgbFrame copiedWithSamplesFrame(1, 1);
	film.develop(srcFrame);
	copiedWithoutSamples->develop(copiedWithoutSamplesFrame);
	copiedWithSamples->develop(copiedWithSamplesFrame);

	// Source has two samples {1, 5}. Sample variance is:
	// mean = 3, M2 = (1 - 3)^2 + (5 - 3)^2 = 8, variance = M2 / (2 - 1) = 8.
	for(const auto componentValue : srcFrame.getPixel({0, 0}))
	{
		EXPECT_NEAR(componentValue, 8.0_r, TEST_FLOAT32_EPSILON);
	}

	// Copied film without samples starts clean.
	for(const auto componentValue : copiedWithoutSamplesFrame.getPixel({0, 0}))
	{
		EXPECT_NEAR(componentValue, 0.0_r, TEST_FLOAT32_EPSILON);
	}

	// Copied film with samples has the same developed value.
	for(const auto componentValue : copiedWithSamplesFrame.getPixel({0, 0}))
	{
		EXPECT_NEAR(componentValue, 8.0_r, TEST_FLOAT32_EPSILON);
	}
}
