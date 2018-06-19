#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <string>

namespace ph
{

enum class EMisStyle
{
	BALANCE,
	POWER
};

template<EMisStyle STYLE>
class TMis final
{
public:
	real weight(const real pdf0, const real pdf1) const
	{
		PH_ASSERT_MSG(pdf0 >= 0.0_r && pdf1 >= 0.0_r && pdf0 + pdf1 != 0.0_r, 
			"pdf0 = " + std::to_string(pdf0) + ", "
			"pdf1 = " + std::to_string(pdf1));

		if constexpr(STYLE == EMisStyle::BALANCE)
		{
			return pdf0 / (pdf0 + pdf1);
		}

		// power heuristic with beta = 2
		//
		if constexpr(STYLE == EMisStyle::POWER)
		{
			return (pdf0 * pdf0) / (pdf0 * pdf0 + pdf1 * pdf1);
		}

		PH_ASSERT_UNREACHABLE_SECTION();
		return 0.0_r;
	}
};

}// end namespace ph