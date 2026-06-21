#include "Engine/Core/Filmic/SampleFilter.h"
#include "Engine/Math/Function/TConstant2D.h"
#include "Engine/Math/Function/TGaussian2D.h"
#include "Engine/Math/Function/TMitchellNetravaliCubic2D.h"
#include "Engine/Math/Function/TBlackmanHarris2D.h"

#include <Common/assertion.h>

namespace ph
{

SampleFilter SampleFilter::makeBox()
{
	constexpr float32 constantValue = 1.0f;
	constexpr float32 filterSize    = 1.0f;

	return make(
		math::TConstant2D<float32>(constantValue),
		filterSize,
		filterSize,
		false);
}

SampleFilter SampleFilter::makeGaussian(const bool useTabulated)
{
	constexpr float32 sigmaX     = 0.5f;
	constexpr float32 sigmaY     = 0.5f;
	constexpr float32 amplitude  = 1.0f;
	constexpr float32 filterSize = 4.0f;

	math::TGaussian2D<float32> gaussianFunc(sigmaX, sigmaY, amplitude);

	// Make the function evaluates to 0 on the filter edge by subtracting its edge value
	const auto edgeValue = gaussianFunc.evaluate(filterSize / 2, filterSize / 2);

	// NOTE: is submerging gaussian filter really make sense?
	// see this thread for more discussion:
	// https://developer.blender.org/D1453
	gaussianFunc.setSubmergeAmount(edgeValue);

	return make(
		gaussianFunc,
		filterSize,
		filterSize,
		useTabulated);
}

SampleFilter SampleFilter::makeMitchellNetravali(const bool useTabulated)
{
	// Reference: Mitchell & Netravali's paper,
	// Reconstruction Filters in Computer Graphics (1998), they 
	// recommends b = c = 1/3, which produces excellent image quality in 
	// their experiments.

	constexpr float32 b = 1.0f / 3.0f;
	constexpr float32 c = 1.0f / 3.0f;
	constexpr float32 filterSize = 4.0f;

	math::TMitchellNetravaliCubic2D<float32> mnCubicFunc(b, c);
	return make(
		mnCubicFunc,
		filterSize,
		filterSize,
		useTabulated);
}

SampleFilter SampleFilter::makeBlackmanHarris(const bool useTabulated)
{
	constexpr float32 radius = 2.0f;
	constexpr float32 filterSize = radius * 2.0f;

	math::TBlackmanHarris2D<float32> bhFunc(radius);
	return make(
		bhFunc,
		filterSize,
		filterSize,
		useTabulated);
}

SampleFilter::SampleFilter()
	: m_filterFunc(nullptr)
	, m_sizePx(0)
	, m_halfSizePx(0)
{}

SampleFilter::SampleFilter(
	std::shared_ptr<math::TMathFunction2D<float32>> filterFunc,
	const float64 widthPx, 
	const float64 heightPx)

	: m_filterFunc(std::move(filterFunc))
	, m_sizePx(widthPx, heightPx)
	, m_halfSizePx(widthPx * 0.5, heightPx * 0.5)
{
	PH_ASSERT(m_filterFunc);
}

}// end namespace ph
