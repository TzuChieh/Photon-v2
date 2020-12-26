#pragma once

#include "Common/config.h"

#ifdef PH_DEBUG

	#include <iostream>
	#include <string>

	namespace ph
	{
		extern void on_assertion_failed();
	}

	#define PH_ASSERT_MSG(condition, failMessage)\
		do\
		{\
			if(!(condition))\
			{\
				std::cerr << "assertion failed at <" << __FILE__ << ">: "\
						  << "line " << __LINE__\
						  << ", condition: <" << #condition << ">";\
				if(!std::string((failMessage)).empty())\
				{\
					std::cerr << "; message: <" << (failMessage) << ">";\
				}\
				std::cerr << std::endl;\
				\
				ph::on_assertion_failed();\
			}\
		} while(0)

	#define PH_INTERNAL_RANGE_MSG(value, lowerBound, upperBound, lowerBoundSymbol, upperBoundSymbol)\
		(std::string(#value) + " = " + std::to_string(value) + ", asserted to be in range = " + \
		lowerBoundSymbol + std::to_string(lowerBound) + ", " + std::to_string(upperBound) + upperBoundSymbol)

#else
	#define PH_ASSERT_MSG(condition, message)
	#define PH_INTERNAL_RANGE_MSG(value, lowerBound, upperBound, lowerBoundSymbol, upperBoundSymbol)
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

/*
	Asserting that <value> is within [<begin>, <end>).
*/
#define PH_ASSERT_IN_RANGE(value, begin, end)\
	PH_ASSERT_MSG(begin <= value && value < end, PH_INTERNAL_RANGE_MSG(value, begin, end, "[", ")"))

/*
	Similar to PH_ASSERT_IN_RANGE(3), except the bounds are inclusive.
*/
#define PH_ASSERT_IN_RANGE_INCLUSIVE(value, lowerBound, upperBound)\
	PH_ASSERT_MSG(lowerBound <= value && value <= upperBound, PH_INTERNAL_RANGE_MSG(value, lowerBound, upperBound, "[", "]"))

/*
	Similar to PH_ASSERT_IN_RANGE(3), except the bounds are exclusive.
*/
#define PH_ASSERT_IN_RANGE_EXCLUSIVE(value, lowerBound, upperBound)\
	PH_ASSERT_MSG(lowerBound < value && value < upperBound, PH_INTERNAL_RANGE_MSG(value, lowerBound, upperBound, "(", ")"))
