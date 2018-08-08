#include "Core/Filmic/SampleFilterFactory.h"
#include "Math/Function/TConstant2D.h"
#include "Math/Function/TGaussian2D.h"
#include "Math/Function/TMNCubic2D.h"
#include "Math/Function/TBlackmanHarris2D.h"

#include <memory>
#include <iostream>

namespace ph
{

SampleFilter SampleFilterFactory::create(const std::string& name)
{
	if(name == BOX_NAME || name == BOX_ABBREV)
	{
		return createBoxFilter();
	}
	else if(name == GAUSSIAN_NAME || name == GAUSSIAN_ABBREV)
	{
		return createGaussianFilter();
	}
	else if(name == MITCHELL_NETRAVALI_NAME || name == MITCHELL_NETRAVALI_ABBREV)
	{
		return createMitchellNetravaliFilter();
	}
	else if(name == BLACKMAN_HARRIS_NAME || name == BLACKMAN_HARRIS_ABBREV)
	{
		return createBlackmanHarrisFilter();
	}
	else
	{
		std::cerr << "warning: at SampleFilterFactory::create(), "
		          << "<" << name << "> is not supported, using " << GAUSSIAN_NAME << std::endl;
		return createGaussianFilter();
	}
}

SampleFilter SampleFilterFactory::createBoxFilter()
{
	const float64 constantValue = 1.0;
	auto constantFunc = std::make_unique<TConstant2D<float64>>(constantValue);

	return SampleFilter(std::move(constantFunc), 1.0, 1.0);
}

SampleFilter SampleFilterFactory::createGaussianFilter()
{
	const float64 sigmaX     = 0.5;
	const float64 sigmaY     = 0.5;
	const float64 amplitude  = 1.0;
	const float64 filterSize = 4.0;

	auto gaussianFunc = std::make_unique<TGaussian2D<float64>>(sigmaX, sigmaY, amplitude);

	// make the function evaluates to 0 on the filter edge
	const float64 edgeValue = gaussianFunc->evaluate(filterSize / 2.0, filterSize / 2.0);

	// NOTE: is submerging gaussian filter really make sense?
	// see this thread for more discussion:
	// https://developer.blender.org/D1453
	gaussianFunc->setSubmergeAmount(edgeValue);

	return SampleFilter(std::move(gaussianFunc), filterSize, filterSize);
}

SampleFilter SampleFilterFactory::createMitchellNetravaliFilter()
{
	// Reference: Mitchell & Netravali's paper,
	// Reconstruction Filters in Computer Graphics (1998), they 
	// recommends b = c = 1/3, which produces excellent image quality in 
	// their experiments.

	const float64 b = 1.0 / 3.0;
	const float64 c = 1.0 / 3.0;

	auto mnCubicFunc = std::make_unique<TMNCubic2D<float64>>(b, c);

	return SampleFilter(std::move(mnCubicFunc), 4.0, 4.0);
}

SampleFilter SampleFilterFactory::createBlackmanHarrisFilter()
{
	const float64 radius = 2.0;

	auto bhFunc = std::make_unique<TBlackmanHarris2D<float64>>(radius);

	return SampleFilter(std::move(bhFunc), radius * 2.0, radius * 2.0);
}

}// end namespace ph