#pragma once

#include <Common/config.h>
#include <Common/utility.h>
#include <Common/primitive_type.h>

#include <gtest/gtest.h>

#include <type_traits>
#include <string>

#if PH_USE_DOUBLE_REAL
	static_assert(std::is_same_v<::ph::real, double>,
		"Macro PH_EXPECT_REAL_EQ() expect real type to be double.");	

	#define PH_EXPECT_REAL_EQ(a, b) EXPECT_DOUBLE_EQ(a, b)
#else
	#define PH_EXPECT_REAL_EQ(a, b) EXPECT_FLOAT_EQ(a, b)
#endif

#define PH_EXPECT_STRING_EQ(a, b) EXPECT_STREQ(std::string(a).c_str(), std::string(b).c_str())
