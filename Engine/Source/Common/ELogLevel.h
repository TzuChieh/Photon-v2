#pragma once

namespace ph
{

enum class ELogLevel
{
	NOTE_MIN,
	NOTE_MED,
	NOTE_MAX,

	WARNING_MIN,
	WARNING_MED,
	WARNING_MAX,

	DEBUG_MIN,
	DEBUG_MED,
	DEBUG_MAX,

	RECOVERABLE_ERROR,
	SEVERE_ERROR,
	FATAL_ERROR
};

}// end namespace ph