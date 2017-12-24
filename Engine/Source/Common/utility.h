#pragma once

#include "Common/config.h"

#include <iostream>

#define PH_NOT_IMPLEMENTED_WARNING()\
	do\
	{\
		std::cerr << "code not implemented in <" << __FILE__ << ">: "\
		          << "line " << __LINE__ << std::endl;\
	} while(0)