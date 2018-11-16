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
	void asyncIncrementNumIterations();

	std::size_t asyncGetNumTracedPhotons() const;
	std::size_t asyncGetNumIterations() const;

private:
	std::atomic_uint64_t m_numTracedPhotons;
	std::atomic_uint32_t m_numIterations;
};

// In-header Implementations:

inline PMStatistics::PMStatistics()
{
	zero();
}

inline void PMStatistics::zero()
{
	m_numTracedPhotons = 0;
	m_numIterations    = 0;
}

inline void PMStatistics::asyncAddNumTracedPhotons(const std::size_t num)
{
	m_numTracedPhotons.fetch_add(static_cast<std::uint64_t>(num), std::memory_order_relaxed);
}

inline void PMStatistics::asyncIncrementNumIterations()
{
	m_numIterations.fetch_add(1, std::memory_order_relaxed);
}

inline std::size_t PMStatistics::asyncGetNumTracedPhotons() const
{
	return static_cast<std::size_t>(m_numTracedPhotons.load(std::memory_order_relaxed));
}

inline std::size_t PMStatistics::asyncGetNumIterations() const
{
	return static_cast<std::size_t>(m_numIterations.load(std::memory_order_relaxed));
}

}// end namespace ph