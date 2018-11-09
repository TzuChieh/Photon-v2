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
	void asyncIncrementNumPasses();

	std::size_t asyncGetNumTracedPhotons() const;
	std::size_t asyncGetNumPasses() const;

private:
	std::atomic_uint64_t m_numTracedPhotons;
	std::atomic_uint32_t m_numPasses;
};

// In-header Implementations:

inline PMStatistics::PMStatistics()
{
	zero();
}

inline void PMStatistics::zero()
{
	m_numTracedPhotons = 0;
	m_numPasses        = 0;
}

inline void PMStatistics::asyncAddNumTracedPhotons(const std::size_t num)
{
	m_numTracedPhotons.fetch_add(static_cast<std::uint64_t>(num), std::memory_order_relaxed);
}

inline void PMStatistics::asyncIncrementNumPasses()
{
	m_numPasses.fetch_add(1, std::memory_order_relaxed);
}

inline std::size_t PMStatistics::asyncGetNumTracedPhotons() const
{
	return static_cast<std::size_t>(m_numTracedPhotons.load(std::memory_order_relaxed));
}

inline std::size_t PMStatistics::asyncGetNumPasses() const
{
	return static_cast<std::size_t>(m_numPasses.load(std::memory_order_relaxed));
}

}// end namespace ph