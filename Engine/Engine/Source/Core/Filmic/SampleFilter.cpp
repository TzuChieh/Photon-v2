#include "Core/Filmic/SampleFilter.h"
#include "Math/Function/TConstant2D.h"
#include "Math/Function/TGaussian2D.h"
#include "Math/Function/TMitchellNetravaliCubic2D.h"
#include "Math/Function/TBlackmanHarris2D.h"

#include <Common/assertion.h>

namespace ph
{

SampleFilter SampleFilter::makeBox()
{
	constexpr float64 constantValue = 1.0;

	return make(math::TConstant2D<float64>(constantValue), 1.0, 1.0);
}

SampleFilter SampleFilter::makeGaussian()
{
	constexpr float64 sigmaX     = 0.5;
	constexpr float64 sigmaY     = 0.5;
	constexpr float64 amplitude  = 1.0;
	constexpr float64 filterSize = 4.0;

	math::TGaussian2D<float64> gaussianFunc(sigmaX, sigmaY, amplitude);

	// Make the function evaluates to 0 on the filter edge by subtracting its edge value
	const auto edgeValue = gaussianFunc.evaluate(filterSize / 2.0, filterSize / 2.0);

	// NOTE: is submerging gaussian filter really make sense?
	// see this thread for more discussion:
	// https://developer.blender.org/D1453
	gaussianFunc.setSubmergeAmount(edgeValue);

	return make(gaussianFunc, filterSize, filterSize);
}

SampleFilter SampleFilter::makeMitchellNetravali()
{
	// Reference: Mitchell & Netravali's paper,
	// Reconstruction Filters in Computer Graphics (1998), they 
	// recommends b = c = 1/3, which produces excellent image quality in 
	// their experiments.

	constexpr float64 b = 1.0 / 3.0;
	constexpr float64 c = 1.0 / 3.0;

	math::TMitchellNetravaliCubic2D<float64> mnCubicFunc(b, c);
	return make(mnCubicFunc, 4.0, 4.0);
}

SampleFilter SampleFilter::makeBlackmanHarris()
{
	constexpr float64 radius = 2.0;

	math::TBlackmanHarris2D<float64> bhFunc(radius);
	return make(bhFunc, radius * 2.0, radius * 2.0);
}

SampleFilter::SampleFilter()
	: m_filterFunc(nullptr)
	, m_sizePx(0)
	, m_halfSizePx(0)
{}

SampleFilter::SampleFilter(
	std::shared_ptr<math::TMathFunction2D<float64>> filterFunc,
	const float64 widthPx, 
	const float64 heightPx)

	: m_filterFunc(std::move(filterFunc))
	, m_sizePx(widthPx, heightPx)
	, m_halfSizePx(widthPx * 0.5, heightPx * 0.5)
{
	PH_ASSERT(m_filterFunc);
}

}// end namespace ph
