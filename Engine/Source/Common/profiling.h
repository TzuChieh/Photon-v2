#pragma once

#include "Common/config.h"
#include "Common/utility.h"

#include "Common/ThirdParty/lib_tracy.h"

#if PH_PROFILING

/*! @brief Defines a name for a profile unit.
@param unitName Name to be defined. Must be unique (treat it like a global with external linkage).
*/
#define PH_DEFINE_PROFILE_UNIT_NAME(unitName)\
	inline constexpr const char* internal_impl_profile_unit_name_##unitName = #unitName

#else

#define PH_PROFILE_UNIT_NAME(unitName)

#endif

#if PH_PROFILING && PH_THIRD_PARTY_HAS_TRACY

/*
Note on names used by Tracy: Tracy requires unique names to have unique pointer, see Tracy manual 3.1.2
"Unique pointers". `PH_DEFINE_PROFILE_UNIT_NAME()` meet the requirement.
*/

#define PH_PROFILE_LOOP_MARK(unitName)\
	FrameMarkNamed(internal_impl_profile_unit_name_##unitName)

#define PH_PROFILE_LOOP_BEGIN(unitName)\
	FrameMarkStart(internal_impl_profile_unit_name_##unitName)

#define PH_PROFILE_LOOP_END(unitName)\
	FrameMarkEnd(internal_impl_profile_unit_name_##unitName)

#define PH_PROFILE_SCOPE()\
	ZoneScoped

#define PH_PROFILE_NAME_THIS_THREAD(threadName)\
	tracy::SetThreadName(threadName)

#else

#define PH_PROFILE_LOOP_MARK(unitName) PH_NO_OP()
#define PH_PROFILE_LOOP_BEGIN(unitName) PH_NO_OP()
#define PH_PROFILE_LOOP_END(unitName) PH_NO_OP()
#define PH_PROFILE_SCOPE() PH_NO_OP()
#define PH_PROFILE_NAME_THIS_THREAD(threadName) PH_NO_OP()

#endif
