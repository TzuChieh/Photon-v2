#pragma once

#include "Common/config.h"

#include <string>
#include <type_traits>

#define PH_CONCAT_2(a, b) a##b
#define PH_CONCAT_3(a, b, c) a##b##c
#define PH_CONCAT_4(a, b, c, d) a##b##c##d
#define PH_CONCAT_5(a, b, c, d, e) a##b##c##d##e
#define PH_CONCAT_6(a, b, c, d, e, f) a##b##c##d##e##f
#define PH_CONCAT_7(a, b, c, d, e, f, g) a##b##c##d##e##f##g
#define PH_CONCAT_8(a, b, c, d, e, f, g, h) a##b##c##d##e##f##g##h

namespace ph::detail
{

void output_not_implemented_warning(
	const std::string& filename,
	const std::string& lineNumber);

}// end namespace ph::detail

#define PH_NOT_IMPLEMENTED_WARNING()\
	do\
	{\
		::ph::detail::output_not_implemented_warning(\
			std::string(__FILE__),\
			std::to_string(__LINE__));\
	} while(0)

#define PH_NO_OP() ((void)0)
