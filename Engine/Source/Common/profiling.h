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

/*!
Tracy requires the named frame mark to have unique string pointer, see Tracy manual 3.1.2: Unique pointers.
`PH_DEFINE_PROFILE_UNIT_NAME()` meet the requirements.
*/
#define PH_PROFILE_LOOP_MARK(unitName)\
	FrameMarkNamed(internal_impl_profile_unit_name_##unitName)

#else

#define PH_PROFILE_LOOP_MARK(unitName) PH_NO_OP()

#endif
