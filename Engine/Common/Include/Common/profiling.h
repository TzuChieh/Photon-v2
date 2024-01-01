#pragma once

/*! @file
@brief Profiling functions.
*/

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

#define PH_DEFINE_PROFILE_UNIT_NAME(unitName)

#endif

#if PH_PROFILING && PH_THIRD_PARTY_HAS_TRACY

/*
Note on names used by Tracy: Some Tracy macros require unique names to have unique pointer, see
Tracy manual 3.1.2 "Unique pointers". `PH_DEFINE_PROFILE_UNIT_NAME()` meet the requirement.
Macro parameters named `unitName` require unique names. For any other name (macro parameters with a
`Str` suffix), see Tracy manual 3.1 "Handling text strings" more details (TL;DR: use literal for
names without a size parameter; otherwise the string data will be copied).
*/

#define PH_PROFILE_LOOP_MARK(unitName)\
	FrameMarkNamed(internal_impl_profile_unit_name_##unitName)

#define PH_PROFILE_LOOP_BEGIN(unitName)\
	FrameMarkStart(internal_impl_profile_unit_name_##unitName)

#define PH_PROFILE_LOOP_END(unitName)\
	FrameMarkEnd(internal_impl_profile_unit_name_##unitName)

#define PH_PROFILE_SCOPE()\
	ZoneScoped

#define PH_PROFILE_NAMED_SCOPE(nameStr)\
	ZoneScopedN(nameStr)

#define PH_PROFILE_NAME_THIS_THREAD(threadNameStr)\
	tracy::SetThreadName(threadNameStr)

#else

#define PH_PROFILE_LOOP_MARK(unitName) PH_NO_OP()
#define PH_PROFILE_LOOP_BEGIN(unitName) PH_NO_OP()
#define PH_PROFILE_LOOP_END(unitName) PH_NO_OP()
#define PH_PROFILE_SCOPE() PH_NO_OP()
#define PH_PROFILE_NAMED_SCOPE(nameStr) PH_NO_OP()
#define PH_PROFILE_NAME_THIS_THREAD(threadNameStr) PH_NO_OP()

#endif
