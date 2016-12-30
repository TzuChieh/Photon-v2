#pragma once

//#define PH_DEBUG

#ifdef PH_DEBUG
	#include <cassert>
	#define PH_ASSERT(condition)\
	{\
		if(!(condition))\
		{\
			std::cerr << "assertion failed at " << __FILE__ << ": line " << __LINE__;\
			std::cerr << "; condition: " << #condition;\
			abort();\
		}\
	}
#else
	#define PH_ASSERT(condition)
#endif