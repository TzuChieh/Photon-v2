#include "Utility/Concurrent/TBlockableAtomicQueue.h"
#include "Common/assertion.h"

#include <utility>
#include <stdexcept>

namespace ph
{

template<typename T>
inline TBlockableAtomicQueue<T>::TBlockableAtomicQueue() :
	m_queue()
{}

template<typename T>
inline TBlockableAtomicQueue<T>::TBlockableAtomicQueue(const std::size_t initialCapacity) :
	m_queue(initialCapacity)
{}

template<typename T>
template<typename U>
inline void TBlockableAtomicQueue<T>::enqueue(U&& item)
{
	if(!m_queue.enqueue(std::forward<U>(item)))
	{
		throw std::runtime_error(
			"Cannot enqueue an item to TBlockableAtomicQueue. Max subqueue size reached.");
	}
}

template<typename T>
template<std::input_iterator Iterator>
inline void TBlockableAtomicQueue<T>::enqueueBulk(Iterator firstItem, const std::size_t numItems)
{
	if(!m_queue.enqueue_bulk(firstItem, numItems))
	{
		throw std::runtime_error(
			"Cannot enqueue an item to TBlockableAtomicQueue. Max subqueue size reached.");
	}
}

template<typename T>
template<typename U>
inline bool TBlockableAtomicQueue<T>::tryEnqueue(U&& item)
{
	return m_queue.try_enqueue(std::forward<U>(item));
}

template<typename T>
inline bool TBlockableAtomicQueue<T>::tryDequeue(T* const out_item)
{
	PH_ASSERT(out_item);
	return m_queue.try_dequeue(*out_item);
}

template<typename T>
inline void TBlockableAtomicQueue<T>::waitDequeue(T* const out_item)
{
	PH_ASSERT(out_item);
	m_queue.wait_dequeue(*out_item);
}

template<typename T>
inline std::size_t TBlockableAtomicQueue<T>::estimatedSize() const
{
	return m_queue.size_approx();
}

}// end namespace ph
