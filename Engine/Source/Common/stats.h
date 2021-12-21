#pragma once

#include <atomic>

namespace ph
{

namespace detail::stats
{

struct TimeCounter final
{
	std::atomic_uint64_t totalMS;
	std::atomic_uint64_t count;

	TimeCounter();
};

struct ScopedTimer final
{
	// TODO: ref to time counter
};

}// end namespace detail::stats

}// end namespace ph
