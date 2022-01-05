#include "Utility/Concurrent/TBlockingQueue.h"
#include "Common/assertion.h"

#include <utility>
#include <stdexcept>

namespace ph
{

template<typename T>
inline TBlockingQueue<T>::TBlockingQueue() :
	m_queue()
{}

template<typename T>
inline TBlockingQueue<T>::TBlockingQueue(const std::size_t initialCapacity) :
	m_queue(initialCapacity)
{}

template<typename T>
template<typename U>
inline void TBlockingQueue<T>::enqueue(U&& item)
{
	if(!m_queue.enqueue(std::forward<U>(item)))
	{
		throw std::runtime_error(
			"Cannot enqueue an item to TBlockingQueue. Max subqueue size reached.");
	}
}

template<typename T>
template<typename U>
inline bool TBlockingQueue<T>::tryEnqueue(U&& item)
{
	return m_queue.try_enqueue(std::forward<U>(item));
}

template<typename T>
inline bool TBlockingQueue<T>::tryDequeue(T* const out_item)
{
	PH_ASSERT(out_item);
	return m_queue.try_dequeue(*out_item);
}

template<typename T>
inline void TBlockingQueue<T>::waitDequeue(T* const out_item)
{
	PH_ASSERT(out_item);
	m_queue.wait_dequeue(*out_item);
}

template<typename T>
inline std::size_t TBlockingQueue<T>::estimatedSize() const
{
	return m_queue.size_approx();
}

}// end namespace ph
