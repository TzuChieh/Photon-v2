#pragma once

#include "Core/LTA/enums.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <string>

namespace ph::lta
{

/*! @brief Static helper for Multiple Importance Sampling (MIS).
See the paper by Veach et al. @cite Veach:1995:Optimally for more theoretical background.
*/
template<EMISStyle STYLE>
class TMIS final
{
public:
	real weight(const real pdf0, const real pdf1) const
	{
		PH_ASSERT_MSG(pdf0 >= 0.0_r && pdf1 >= 0.0_r && pdf0 + pdf1 != 0.0_r, 
			"pdf0 = " + std::to_string(pdf0) + ", "
			"pdf1 = " + std::to_string(pdf1));

		if constexpr(STYLE == EMISStyle::Balance)
		{
			return pdf0 / (pdf0 + pdf1);
		}

		// Power heuristic with beta = 2
		if constexpr(STYLE == EMISStyle::Power)
		{
			return (pdf0 * pdf0) / (pdf0 * pdf0 + pdf1 * pdf1);
		}

		PH_ASSERT_UNREACHABLE_SECTION();
		return 0.0_r;
	}
};

}// end namespace ph::lta
