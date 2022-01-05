#include "Utility/Concurrent/TLockFreeQueue.h"
#include "Common/assertion.h"

#include <utility>
#include <stdexcept>

namespace ph
{

template<typename T>
inline TLockFreeQueue<T>::TLockFreeQueue() :
	m_queue()
{}

template<typename T>
inline TLockFreeQueue<T>::TLockFreeQueue(const std::size_t initialCapacity) :
	m_queue(initialCapacity)
{}

template<typename T>
template<typename U>
inline void TLockFreeQueue<T>::enqueue(U&& item)
{
	if(!m_queue.enqueue(std::forward<U>(item)))
	{
		throw std::runtime_error(
			"Cannot enqueue an item to TLockFreeQueue. Max subqueue size reached.");
	}
}

template<typename T>
template<typename U>
inline bool TLockFreeQueue<T>::tryEnqueue(U&& item)
{
	return m_queue.try_enqueue(std::forward<U>(item));
}

template<typename T>
inline bool TLockFreeQueue<T>::tryDequeue(T* const out_item)
{
	PH_ASSERT(out_item);
	return m_queue.try_dequeue(*out_item);
}

template<typename T>
inline std::size_t TLockFreeQueue<T>::estimatedSize() const
{
	return m_queue.size_approx();
}

}// end namespace ph
