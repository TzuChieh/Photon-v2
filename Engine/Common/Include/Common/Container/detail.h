#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <functional>

namespace ph::detail
{

// References: 
// [1] https://www.cppstories.com/2021/heterogeneous-access-cpp20/
// [2] https://en.cppreference.com/w/cpp/container/unordered_map/find
struct HeterogeneousStringHash
{
	using is_transparent = void;

	[[nodiscard]]
	std::size_t operator () (const char* txt) const
	{
		return std::hash<std::string_view>{}(txt);
	}

	[[nodiscard]]
	std::size_t operator () (std::string_view txt) const
	{
		return std::hash<std::string_view>{}(txt);
	}

	[[nodiscard]]
	std::size_t operator () (const std::string& txt) const
	{
		return std::hash<std::string>{}(txt);
	}
};

}// end namespace ph::detail
