#pragma once

#include "Common/config.h"

#ifdef PH_DEBUG

	#include <iostream>
	#include <string>

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
		}\
	} while(0)

#define PH_ASSERT(condition) PH_ASSERT_MSG(condition, "")

#else
	#define PH_ASSERT_MSG(condition, message)
	#define PH_ASSERT(condition)
#endif