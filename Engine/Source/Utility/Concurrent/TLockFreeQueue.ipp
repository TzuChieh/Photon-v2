#include "Utility/Concurrent/TLockFreeQueue.h"
#include "Common/assertion.h"

#include <utility>

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
inline bool TLockFreeQueue<T>::enqueue(U&& item)
{
	return m_queue.enqueue(std::forward<U>(item));
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
