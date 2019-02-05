#pragma once

#include "Core/Filmic/SampleFilter.h"

#include <string_view>
#include <string>

namespace ph
{

class SampleFilters final
{
public:
	static constexpr std::string_view BOX_NAME                  = "box";
	static constexpr std::string_view BOX_ABBREV                = "box";
	static constexpr std::string_view GAUSSIAN_NAME             = "gaussian";
	static constexpr std::string_view GAUSSIAN_ABBREV           = "gaussian";
	static constexpr std::string_view MITCHELL_NETRAVALI_NAME   = "mitchell-netravali";
	static constexpr std::string_view MITCHELL_NETRAVALI_ABBREV = "mn";
	static constexpr std::string_view BLACKMAN_HARRIS_NAME      = "blackman-harris";
	static constexpr std::string_view BLACKMAN_HARRIS_ABBREV    = "bh";

	static SampleFilter create(const std::string& name);

	static SampleFilter createBoxFilter();
	static SampleFilter createGaussianFilter();
	static SampleFilter createMitchellNetravaliFilter();
	static SampleFilter createBlackmanHarrisFilter();
};

}// end namespace ph