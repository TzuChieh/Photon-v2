#pragma once

#include <Common/exceptions.h>

#include <string>

namespace ph
{

/*! @brief General exception thrown on cook error.
*/
class CookException : public RuntimeException
{
public:
	using RuntimeException::RuntimeException;
};

/*! @brief General exception thrown on cook error for actors.
*/
class ActorCookException : public CookException
{
public:
	using CookException::CookException;
};

}// end namespace ph
