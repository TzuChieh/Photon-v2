#pragma once

#include <Common/config.h>

#include <gtest/gtest.h>

#ifndef PH_USE_DOUBLE_REAL
	#define UTIL_EXPECT_REAL_EQ(a, b) EXPECT_FLOAT_EQ(a, b)
#else
	#define UTIL_EXPECT_REAL_EQ(a, b) EXPECT_DOUBLE_EQ(a, b)
#endif
