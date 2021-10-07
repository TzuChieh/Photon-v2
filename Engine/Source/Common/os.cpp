#include "Common/os.h"

#ifdef PH_OPERATING_SYSTEM_IS_WINDOWS

#include <VersionHelpers.h>

#endif

namespace ph::os
{

inline bool is_windows()
{
#ifdef PH_OPERATING_SYSTEM_IS_WINDOWS
	return true;
#else
	return false;
#endif
}

bool is_windows_7_or_greater()
{
	
}

}// end namespace ph::os
