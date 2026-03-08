#pragma once

#include <nanobind/nanobind.h>

#include <format>

namespace ph::py
{

/*! @brief Throw a Python `TypeError` with a formatted message.
*/
template<typename... Args>
[[noreturn]]
inline void throw_nanobind_type_error(const std::format_string<Args...> msgFormat, Args&&... args)
{
	throw nanobind::type_error(
		std::vformat(msgFormat.get(), std::make_format_args(args...)).c_str());
}

/*! @brief Throw a Python `ValueError` with a formatted message.
*/
template<typename... Args>
[[noreturn]]
inline void throw_nanobind_value_error(const std::format_string<Args...> msgFormat, Args&&... args)
{
	throw nanobind::value_error(
		std::vformat(msgFormat.get(), std::make_format_args(args...)).c_str());
}

}// end namespace ph::py
