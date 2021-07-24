#include "Core/Filmic/SampleFilters.h"
#include "Math/Function/TConstant2D.h"
#include "Math/Function/TGaussian2D.h"
#include "Math/Function/TMNCubic2D.h"
#include "Math/Function/TBlackmanHarris2D.h"

#include <memory>
#include <iostream>

namespace ph
{

SampleFilter SampleFilters::createBoxFilter()
{
	const float64 constantValue = 1.0;
	auto constantFunc = std::make_unique<math::TConstant2D<float64>>(constantValue);

	return SampleFilter(std::move(constantFunc), 1.0, 1.0);
}

SampleFilter SampleFilters::createGaussianFilter()
{
	const float64 sigmaX     = 0.5;
	const float64 sigmaY     = 0.5;
	const float64 amplitude  = 1.0;
	const float64 filterSize = 4.0;

	auto gaussianFunc = std::make_unique<math::TGaussian2D<float64>>(sigmaX, sigmaY, amplitude);

	// make the function evaluates to 0 on the filter edge
	const float64 edgeValue = gaussianFunc->evaluate(filterSize / 2.0, filterSize / 2.0);

	// NOTE: is submerging gaussian filter really make sense?
	// see this thread for more discussion:
	// https://developer.blender.org/D1453
	gaussianFunc->setSubmergeAmount(edgeValue);

	return SampleFilter(std::move(gaussianFunc), filterSize, filterSize);
}

SampleFilter SampleFilters::createMitchellNetravaliFilter()
{
	// Reference: Mitchell & Netravali's paper,
	// Reconstruction Filters in Computer Graphics (1998), they 
	// recommends b = c = 1/3, which produces excellent image quality in 
	// their experiments.

	const float64 b = 1.0 / 3.0;
	const float64 c = 1.0 / 3.0;

	auto mnCubicFunc = std::make_unique<math::TMNCubic2D<float64>>(b, c);

	return SampleFilter(std::move(mnCubicFunc), 4.0, 4.0);
}

SampleFilter SampleFilters::createBlackmanHarrisFilter()
{
	const float64 radius = 2.0;

	auto bhFunc = std::make_unique<math::TBlackmanHarris2D<float64>>(radius);

	return SampleFilter(std::move(bhFunc), radius * 2.0, radius * 2.0);
}

}// end namespace ph
