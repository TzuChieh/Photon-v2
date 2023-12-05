#include "Utility/Concurrent/TAtomicQuasiQueue.h"

#include <Common/assertion.h>

#include <utility>
#include <stdexcept>

namespace ph
{

template<typename T>
inline TAtomicQuasiQueue<T>::TAtomicQuasiQueue() :
	m_queue()
{}

template<typename T>
inline TAtomicQuasiQueue<T>::TAtomicQuasiQueue(const std::size_t initialCapacity) :
	m_queue(initialCapacity)
{}

template<typename T>
template<typename U>
inline void TAtomicQuasiQueue<T>::enqueue(U&& item)
{
	if(!m_queue.enqueue(std::forward<U>(item)))
	{
		throw std::runtime_error(
			"Cannot enqueue an item to TAtomicQuasiQueue. Max subqueue size reached.");
	}
}

template<typename T>
template<std::input_iterator Iterator>
inline void TAtomicQuasiQueue<T>::enqueueBulk(Iterator firstItem, std::size_t numItems)
{
	if(!m_queue.enqueue_bulk(firstItem, numItems))
	{
		throw std::runtime_error(
			"Cannot enqueue an item to TAtomicQuasiQueue. Max subqueue size reached.");
	}
}

template<typename T>
template<typename U>
inline bool TAtomicQuasiQueue<T>::tryEnqueue(U&& item)
{
	return m_queue.try_enqueue(std::forward<U>(item));
}

template<typename T>
inline bool TAtomicQuasiQueue<T>::tryDequeue(T* const out_item)
{
	PH_ASSERT(out_item);
	return m_queue.try_dequeue(*out_item);
}

template<typename T>
template<std::output_iterator<T> Iterator>
inline std::size_t TAtomicQuasiQueue<T>::tryDequeueBulk(Iterator out_firstItem, std::size_t numItems)
{
	return m_queue.try_dequeue_bulk(out_firstItem, numItems);
}

template<typename T>
inline std::size_t TAtomicQuasiQueue<T>::estimatedSize() const
{
	return m_queue.size_approx();
}

}// end namespace ph
