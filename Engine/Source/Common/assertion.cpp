#include "Common/assertion.h"
#include "Common/os.h"

#include <cstdlib>
#include <iostream>

#if defined(PH_DEBUG) && defined(PH_PRINT_STACK_TRACE_ON_ASSERTION_FAILED)

	#if defined(PH_OPERATING_SYSTEM_IS_WINDOWS)

		#include <StackWalker.h>

		namespace ph
		{
			class ConsoleOutputStackWalker : public StackWalker
			{
			public:
				ConsoleOutputStackWalker() : 
					StackWalker() 
				{}

			protected:
				void OnOutput(LPCSTR szText) override
				{
					StackWalker::OnOutput(szText);
					std::cerr << szText;
				}
			};
		}

	#elif defined(PH_OPERATING_SYSTEM_IS_LINUX) || defined(PH_OPERATING_SYSTEM_IS_OSX)
		#include <execinfo.h>
	#endif

#endif

namespace ph::detail
{

void output_assertion_message(
	const std::string& fileName,
	const std::string& lineNumber,
	const std::string& condition,
	const std::string& message)
{
	std::cerr << "assertion failed at <" << fileName << ">: "
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

#if defined(PH_OPERATING_SYSTEM_IS_WINDOWS)

	ConsoleOutputStackWalker stackWalker;
	stackWalker.ShowCallstack();

#elif defined(PH_OPERATING_SYSTEM_IS_LINUX) || defined(PH_OPERATING_SYSTEM_IS_OSX)

	const int ENTRY_BUFFER_SIZE = 64;

	int numEntries;
	void* entryBuffer[ENTRY_BUFFER_SIZE];
	numEntries = backtrace(entryBuffer, ENTRY_BUFFER_SIZE);

	std::cerr << numEntries << " entries recorded" << std::endl;
	std::cerr << "showing most recent call first:" << std::endl;

	char** symbolStrings = backtrace_symbols(entryBuffer, numEntries);
	for(int i = 0; i < numEntries; ++i)
	{
		std::cerr << symbolStrings[i] << std::endl;
	}

#endif

#endif

#ifdef PH_ABORT_ON_ASSERTION_FAILED
	std::abort();
#endif
}

}// end namespace ph::detail
