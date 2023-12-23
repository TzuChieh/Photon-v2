#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

namespace ph
{

class PMAtomicStatistics final
{
public:
	PMAtomicStatistics();

	void zero();

	void addNumTracedPhotons(std::size_t num);
	void incrementNumIterations();

	std::size_t getNumTracedPhotons() const;
	std::size_t getNumIterations() const;

private:
	std::atomic_uint64_t m_numTracedPhotons;
	std::atomic_uint32_t m_numIterations;
};

// In-header Implementations:

inline PMAtomicStatistics::PMAtomicStatistics()
{
	zero();
}

inline void PMAtomicStatistics::zero()
{
	m_numTracedPhotons = 0;
	m_numIterations    = 0;
}

inline void PMAtomicStatistics::addNumTracedPhotons(const std::size_t num)
{
	m_numTracedPhotons.fetch_add(static_cast<std::uint64_t>(num), std::memory_order_relaxed);
}

inline void PMAtomicStatistics::incrementNumIterations()
{
	m_numIterations.fetch_add(1, std::memory_order_relaxed);
}

inline std::size_t PMAtomicStatistics::getNumTracedPhotons() const
{
	return static_cast<std::size_t>(m_numTracedPhotons.load(std::memory_order_relaxed));
}

inline std::size_t PMAtomicStatistics::getNumIterations() const
{
	return static_cast<std::size_t>(m_numIterations.load(std::memory_order_relaxed));
}

}// end namespace ph
