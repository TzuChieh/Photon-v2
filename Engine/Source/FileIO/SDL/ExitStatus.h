#pragma once

#include <string>

namespace ph
{

class ExitStatus final
{
public:
	enum class State
	{
		SUCCESS,   // operation done successfully
		WARNING,   // operation done, but it may not be what the user expected
		FAILURE,   // operation failed while processing
		BAD_INPUT, // operation ignored because of bad input data
		UNSUPPORTED// operation ignored because it is not supported
	};

	static ExitStatus SUCCESS    (const std::string& message = "");
	static ExitStatus WARNING    (const std::string& message = "");
	static ExitStatus BAD_INPUT  (const std::string& message = "");
	static ExitStatus FAILURE    (const std::string& message = "");
	static ExitStatus UNSUPPORTED(const std::string& message = "");

public:
	State       state;
	std::string message;

	ExitStatus(const State state, const std::string& message);
};

}// end namespace ph
