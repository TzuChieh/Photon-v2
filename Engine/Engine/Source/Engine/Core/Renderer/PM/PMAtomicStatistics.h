#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

namespace ph
{

/*! @brief Thread safe statictics tracking utilities.
*/
class PMAtomicStatistics final
{
public:
	PMAtomicStatistics();

	/*! @brief Resets all internal states.
	*/
	void zero();

	void addNumTracedPhotons(std::size_t num);
	void incrementNumProcessedSteps();

	std::size_t numTracedPhotons() const;
	std::size_t numProcessedSteps() const;

private:
	std::atomic_uint64_t m_numTracedPhotons;
	std::atomic_uint32_t m_numProcessedSteps;
};

// In-header Implementations:

inline PMAtomicStatistics::PMAtomicStatistics()
{
	zero();
}

inline void PMAtomicStatistics::zero()
{
	m_numTracedPhotons  = 0;
	m_numProcessedSteps = 0;
}

inline void PMAtomicStatistics::addNumTracedPhotons(const std::size_t num)
{
	m_numTracedPhotons.fetch_add(static_cast<std::uint64_t>(num), std::memory_order_relaxed);
}

inline void PMAtomicStatistics::incrementNumProcessedSteps()
{
	m_numProcessedSteps.fetch_add(1, std::memory_order_relaxed);
}

inline std::size_t PMAtomicStatistics::numTracedPhotons() const
{
	return static_cast<std::size_t>(m_numTracedPhotons.load(std::memory_order_relaxed));
}

inline std::size_t PMAtomicStatistics::numProcessedSteps() const
{
	return static_cast<std::size_t>(m_numProcessedSteps.load(std::memory_order_relaxed));
}

}// end namespace ph
