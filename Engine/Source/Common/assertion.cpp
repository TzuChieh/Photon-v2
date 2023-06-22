#include "Common/assertion.h"
#include "Common/os.h"
#include "Common/debug.h"

#include <cstdlib>
#include <iostream>

namespace ph::detail
{

void output_assertion_message(
	const std::string& filename,
	const std::string& lineNumber,
	const std::string& condition,
	const std::string& message)
{
	std::cerr << "assertion failed at <" << filename << ">: "
	          << "line " << lineNumber
	          << ", condition: <" << condition << ">";

	if(!message.empty())
	{
		std::cerr << "; message: <" << message << ">";
	}

	std::cerr << std::endl;
}

void on_assertion_failed()
{
#ifdef PH_PRINT_STACK_TRACE_ON_ASSERTION_FAILED
	std::cerr << obtain_stack_trace() << std::endl;
#endif

	PH_DEBUG_BREAK();

#ifdef PH_ABORT_ON_ASSERTION_FAILED
	std::abort();
#endif
}

}// end namespace ph::detail
