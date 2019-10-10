#include "FileIO/SDL/ExitStatus.h"

namespace ph
{

ExitStatus ExitStatus::SUCCESS(const std::string& message)
{
	return ExitStatus(State::SUCCESS, message);
}

ExitStatus ExitStatus::WARNING(const std::string& message)
{
	return ExitStatus(State::WARNING, message);
}

ExitStatus ExitStatus::FAILURE(const std::string& message)
{
	return ExitStatus(State::FAILURE, message);
}

ExitStatus ExitStatus::BAD_INPUT(const std::string& message)
{
	return ExitStatus(State::BAD_INPUT, message);
}

ExitStatus ExitStatus::UNSUPPORTED(const std::string& message)
{
	return ExitStatus(State::UNSUPPORTED, message);
}

ExitStatus::ExitStatus(const State state, const std::string& message) :
	state(state), message(message)
{}

}// end namespace ph
