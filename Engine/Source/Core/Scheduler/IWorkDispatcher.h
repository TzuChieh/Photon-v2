#pragma once

#include "Core/Scheduler/WorkUnit.h"

namespace ph
{

/*! @brief A manager that distributes work.

The dispatcher will deliver some amount of work on demand. The sizes of each 
delivered work and the total amount of work is implementation defined.
*/
class IWorkDispatcher
{
public:
	virtual ~IWorkDispatcher() = default;

	/*! @brief Get some amount of work.

	The meaning of the work dimensions is depending on the use case.
	@return True if work has been dispatched.
	*/
	virtual bool dispatch(WorkUnit* out_workUnit) = 0;
};

}// end namespace ph
