#include "Utility/Concurrent/TAtomicQueue.h"
#include "Common/assertion.h"

#include <utility>
#include <stdexcept>

namespace ph
{

template<typename T>
inline TAtomicQueue<T>::TAtomicQueue() :
	m_queue()
{}

template<typename T>
inline TAtomicQueue<T>::TAtomicQueue(const std::size_t initialCapacity) :
	m_queue(initialCapacity)
{}

template<typename T>
template<typename U>
inline void TAtomicQueue<T>::enqueue(U&& item)
{
	if(!m_queue.enqueue(std::forward<U>(item)))
	{
		throw std::runtime_error(
			"Cannot enqueue an item to TAtomicQueue. Max subqueue size reached.");
	}
}

template<typename T>
template<std::input_iterator Iterator>
inline void TAtomicQueue<T>::enqueueBulk(Iterator firstItem, const std::size_t numItems)
{
	if(!m_queue.enqueue_bulk(firstItem, numItems))
	{
		throw std::runtime_error(
			"Cannot enqueue an item to TAtomicQueue. Max subqueue size reached.");
	}
}

template<typename T>
template<typename U>
inline bool TAtomicQueue<T>::tryEnqueue(U&& item)
{
	return m_queue.try_enqueue(std::forward<U>(item));
}

template<typename T>
inline bool TAtomicQueue<T>::tryDequeue(T* const out_item)
{
	PH_ASSERT(out_item);
	return m_queue.try_dequeue(*out_item);
}

template<typename T>
inline std::size_t TAtomicQueue<T>::estimatedSize() const
{
	return m_queue.size_approx();
}

}// end namespace ph
