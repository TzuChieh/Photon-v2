#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

namespace ph
{

class PMStatistics
{
public:
	PMStatistics();

	void zero();
	void asyncAddNumTracedPhotons(std::size_t num);
	std::size_t asyncGetNumTracedPhotons() const;

private:
	std::atomic_uint64_t m_numTracedPhotons;
};

// In-header Implementations:

inline PMStatistics::PMStatistics()
{
	zero();
}

inline void PMStatistics::zero()
{
	m_numTracedPhotons = 0;
}

inline void PMStatistics::asyncAddNumTracedPhotons(const std::size_t num)
{
	m_numTracedPhotons.fetch_add(static_cast<std::uint64_t>(num), std::memory_order_relaxed);
}

inline std::size_t PMStatistics::asyncGetNumTracedPhotons() const
{
	return static_cast<std::size_t>(m_numTracedPhotons.load(std::memory_order_relaxed));
}

}// end namespace ph