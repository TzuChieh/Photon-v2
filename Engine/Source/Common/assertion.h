#pragma once

#include "Common/config.h"

#ifdef PH_DEBUG
	#include <iostream>
	#define PH_ASSERT(condition)\
	do\
	{\
		if(!(condition))\
		{\
			std::cerr << "assertion failed at <" << __FILE__ << ">: "\
			          << "line " << __LINE__\
			          << ", condition: <" << #condition << ">" << std::endl;\
		}\
	} while(0)
#else
	#define PH_ASSERT(condition)
#endif