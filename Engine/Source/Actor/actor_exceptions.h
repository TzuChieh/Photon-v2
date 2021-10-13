#pragma once

#include "Utility/exception.h"

#include <string>

namespace ph
{

/*! @brief General exception thrown on cook error.
*/
class ActorCookException : public RuntimeException
{
public:
	explicit ActorCookException(const std::string& message);
	explicit ActorCookException(const char* message);
};

// In-header Implementations:

inline ActorCookException::ActorCookException(const std::string& message) :
	RuntimeException(message)
{}

inline ActorCookException::ActorCookException(const char* const message) :
	RuntimeException(message)
{}

}// end namespace ph
