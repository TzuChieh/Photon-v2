#include "Utility/Concurrent/TBlockableAtomicQuasiQueue.h"

#include <Common/assertion.h>

#include <utility>
#include <stdexcept>

namespace ph
{

template<typename T>
inline TBlockableAtomicQuasiQueue<T>::TBlockableAtomicQuasiQueue() :
	m_queue()
{}

template<typename T>
inline TBlockableAtomicQuasiQueue<T>::TBlockableAtomicQuasiQueue(const std::size_t initialCapacity) :
	m_queue(initialCapacity)
{}

template<typename T>
template<typename U>
inline void TBlockableAtomicQuasiQueue<T>::enqueue(U&& item)
{
	if(!m_queue.enqueue(std::forward<U>(item)))
	{
		throw std::runtime_error(
			"Cannot enqueue an item to TBlockableAtomicQuasiQueue. Max subqueue size reached.");
	}
}

template<typename T>
template<std::input_iterator Iterator>
inline void TBlockableAtomicQuasiQueue<T>::enqueueBulk(Iterator firstItem, const std::size_t numItems)
{
	if(!m_queue.enqueue_bulk(firstItem, numItems))
	{
		throw std::runtime_error(
			"Cannot enqueue an item to TBlockableAtomicQuasiQueue. Max subqueue size reached.");
	}
}

template<typename T>
template<typename U>
inline bool TBlockableAtomicQuasiQueue<T>::tryEnqueue(U&& item)
{
	return m_queue.try_enqueue(std::forward<U>(item));
}

template<typename T>
inline bool TBlockableAtomicQuasiQueue<T>::tryDequeue(T* const out_item)
{
	PH_ASSERT(out_item);
	return m_queue.try_dequeue(*out_item);
}

template<typename T>
inline void TBlockableAtomicQuasiQueue<T>::waitDequeue(T* const out_item)
{
	PH_ASSERT(out_item);
	m_queue.wait_dequeue(*out_item);
}

template<typename T>
template<std::output_iterator<T> Iterator>
inline std::size_t TBlockableAtomicQuasiQueue<T>::waitDequeueBulk(Iterator out_firstItem, std::size_t numItems)
{
	return m_queue.wait_dequeue_bulk(out_firstItem, numItems);
}

template<typename T>
inline std::size_t TBlockableAtomicQuasiQueue<T>::estimatedSize() const
{
	return m_queue.size_approx();
}

}// end namespace ph
