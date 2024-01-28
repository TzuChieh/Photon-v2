#pragma once

#include "Common/Log/logger_fwd.h"

namespace ph
{

enum class ELogLevel
{
	Debug,
	Note,
	Warning,
	Error,
	DebugOnce,
	NoteOnce,
	WarningOnce,
	ErrorOnce
};

inline constexpr bool is_once(const ELogLevel logLevel)
{
	switch(logLevel)
	{
	case ELogLevel::DebugOnce:
	case ELogLevel::NoteOnce:
	case ELogLevel::WarningOnce:
	case ELogLevel::ErrorOnce:
		return true;

	default:
		return false;
	}
}

}// end namespace ph