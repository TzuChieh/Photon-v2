#include "Utility/Concurrent/TBlockableQueue.h"
#include "Common/assertion.h"

#include <utility>
#include <stdexcept>

namespace ph
{

template<typename T>
inline TBlockableQueue<T>::TBlockableQueue() :
	m_queue()
{}

template<typename T>
inline TBlockableQueue<T>::TBlockableQueue(const std::size_t initialCapacity) :
	m_queue(initialCapacity)
{}

template<typename T>
template<typename U>
inline void TBlockableQueue<T>::enqueue(U&& item)
{
	if(!m_queue.enqueue(std::forward<U>(item)))
	{
		throw std::runtime_error(
			"Cannot enqueue an item to TBlockableQueue. Max subqueue size reached.");
	}
}

template<typename T>
template<typename U>
inline bool TBlockableQueue<T>::tryEnqueue(U&& item)
{
	return m_queue.try_enqueue(std::forward<U>(item));
}

template<typename T>
inline bool TBlockableQueue<T>::tryDequeue(T* const out_item)
{
	PH_ASSERT(out_item);
	return m_queue.try_dequeue(*out_item);
}

template<typename T>
inline void TBlockableQueue<T>::waitDequeue(T* const out_item)
{
	PH_ASSERT(out_item);
	m_queue.wait_dequeue(*out_item);
}

template<typename T>
inline std::size_t TBlockableQueue<T>::estimatedSize() const
{
	return m_queue.size_approx();
}

}// end namespace ph
