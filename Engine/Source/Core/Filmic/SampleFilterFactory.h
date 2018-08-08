#pragma once

#include "Core/Filmic/SampleFilter.h"

#include <string_view>
#include <string>

namespace ph
{

class SampleFilterFactory final
{
public:
	static constexpr std::string_view BOX_NAME                  = "box";
	static constexpr std::string_view BOX_ABBREV                = "box";
	static constexpr std::string_view GAUSSIAN_NAME             = "gaussian";
	static constexpr std::string_view GAUSSIAN_ABBREV           = "gaussian";
	static constexpr std::string_view MITCHELL_NETRAVALI_NAME   = "mitchell-netravali";
	static constexpr std::string_view MITCHELL_NETRAVALI_ABBREV = "mn";

	static SampleFilter create(const std::string& name);

	static SampleFilter createBoxFilter();
	static SampleFilter createGaussianFilter();
	static SampleFilter createMitchellNetravaliFilter();
};

}// end namespace ph