#pragma once

#include "Common/config.h"
#include "Common/utility.h"

#include <string>

namespace ph::detail
{

void output_assertion_message(
	const std::string& fileName,
	const std::string& lineNumber,
	const std::string& condition,
	const std::string& message);

void on_assertion_failed();

}// end namespace ph::detail

#ifdef PH_DEBUG

	#define PH_ASSERT_MSG(condition, failMessage)\
		do\
		{\
			if(!(condition))\
			{\
				::ph::detail::output_assertion_message(\
					std::string(__FILE__),\
					std::to_string(__LINE__),\
					std::string(#condition),\
					std::string((failMessage)));\
				\
				::ph::detail::on_assertion_failed();\
			}\
		} while(0)

	#define PH_INTERNAL_RANGE_MSG(value, lowerBound, upperBound, lowerBoundSymbol, upperBoundSymbol)\
		(std::string(#value) + " = " + std::to_string(value) + ", asserted to be in range = " + \
		lowerBoundSymbol + std::to_string(lowerBound) + ", " + std::to_string(upperBound) + upperBoundSymbol)

#else
	#define PH_ASSERT_MSG(condition, message) PH_NO_OP()
	#define PH_INTERNAL_RANGE_MSG(value, lowerBound, upperBound, lowerBoundSymbol, upperBoundSymbol) PH_NO_OP()
#endif

#define PH_ASSERT(condition)\
	PH_ASSERT_MSG(condition, "")

#define PH_ASSERT_UNREACHABLE_SECTION()\
	PH_ASSERT_MSG(false, "executing supposedly unreachable code")

#define PH_ASSERT_EQ(a, b)\
	PH_ASSERT_MSG(a == b, std::string(#a) + " = " + std::to_string(a) + ", " + #b + " = " + std::to_string(b))

#define PH_ASSERT_NE(a, b)\
	PH_ASSERT_MSG(a != b, std::string(#a) + " = " + std::to_string(a) + ", " + #b + " = " + std::to_string(b))

#define PH_ASSERT_GT(a, b)\
	PH_ASSERT_MSG(a > b, std::string(#a) + " = " + std::to_string(a) + ", " + #b + " = " + std::to_string(b))

#define PH_ASSERT_LT(a, b)\
	PH_ASSERT_MSG(a < b, std::string(#a) + " = " + std::to_string(a) + ", " + #b + " = " + std::to_string(b))

#define PH_ASSERT_GE(a, b)\
	PH_ASSERT_MSG(a >= b, std::string(#a) + " = " + std::to_string(a) + ", " + #b + " = " + std::to_string(b))

#define PH_ASSERT_LE(a, b)\
	PH_ASSERT_MSG(a <= b, std::string(#a) + " = " + std::to_string(a) + ", " + #b + " = " + std::to_string(b))

/*! @brief Assert that @p value is within [@p begin, @p end).
*/
#define PH_ASSERT_IN_RANGE(value, begin, end)\
	PH_ASSERT_MSG(begin <= value && value < end, PH_INTERNAL_RANGE_MSG(value, begin, end, "[", ")"))

/*! @brief Similar to PH_ASSERT_IN_RANGE(3), except the bounds are inclusive.
*/
#define PH_ASSERT_IN_RANGE_INCLUSIVE(value, lowerBound, upperBound)\
	PH_ASSERT_MSG(lowerBound <= value && value <= upperBound, PH_INTERNAL_RANGE_MSG(value, lowerBound, upperBound, "[", "]"))

/*! @brief Similar to PH_ASSERT_IN_RANGE(3), except the bounds are exclusive.
*/
#define PH_ASSERT_IN_RANGE_EXCLUSIVE(value, lowerBound, upperBound)\
	PH_ASSERT_MSG(lowerBound < value && value < upperBound, PH_INTERNAL_RANGE_MSG(value, lowerBound, upperBound, "(", ")"))
