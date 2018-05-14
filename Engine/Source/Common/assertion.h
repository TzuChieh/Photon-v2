#pragma once

#include "Common/config.h"

#ifdef PH_DEBUG

	//#include <boost/stacktrace.hpp>

	#include <iostream>
	#include <string>
	#include <cstdlib>

	#ifdef PH_ABORT_ON_ASSERTION_FAILED
		#define PH_INTERNAL_ASSERTION_ABORT() std::abort()
	#else
		#define PH_INTERNAL_ASSERTION_ABORT()
	#endif

	#define PH_ASSERT_MSG(condition, message)\
		do\
		{\
			if(!(condition))\
			{\
				std::cerr << "assertion failed at <" << __FILE__ << ">: "\
						  << "line " << __LINE__\
						  << ", condition: <" << #condition << ">";\
				if(!std::string((message)).empty())\
				{\
					std::cerr << "; message: <" << (message) << ">";\
				}\
				std::cerr << std::endl;\
				\
				PH_INTERNAL_ASSERTION_ABORT();\
			}\
		} while(0)

	#define PH_ASSERT(condition) PH_ASSERT_MSG(condition, "")

	#define PH_ASSERT_UNREACHABLE_SECTION() PH_ASSERT_MSG(false, "executing supposedly unreachable code")

#else
	#define PH_ASSERT_MSG(condition, message)
	#define PH_ASSERT(condition)
	#define PH_ASSERT_UNREACHABLE_SECTION()
#endif