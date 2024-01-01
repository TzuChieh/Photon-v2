#pragma once

#include <Common/assertion.h>
#include <Common/primitive_type.h>

#include <string>

namespace ph::lta
{

enum class EMisStyle
{
	Balance,
	Power
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

		if constexpr(STYLE == EMisStyle::Balance)
		{
			return pdf0 / (pdf0 + pdf1);
		}

		// Power heuristic with beta = 2
		if constexpr(STYLE == EMisStyle::Power)
		{
			return (pdf0 * pdf0) / (pdf0 * pdf0 + pdf1 * pdf1);
		}

		PH_ASSERT_UNREACHABLE_SECTION();
		return 0.0_r;
	}
};

}// end namespace ph::lta
