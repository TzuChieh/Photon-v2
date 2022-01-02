#pragma once

#include <moodycamel/concurrentqueue.h>

#include <cstddef>

namespace ph
{

/*! @brief A lock-free queue.
For single-thread uses, it is a FIFO queue. For multi-thread uses, it is "mostly" a FIFO queue (the items
first went in the queue will generally come out first, strict FIFO is not guaranteed for performance
reasons). It is guaranteed that items will not be left in the queue indefinitely.
*/
template<typename T>
class TLockFreeQueue final
{
public:
	TLockFreeQueue();
	explicit TLockFreeQueue(std::size_t initialCapacity);

	/*! @brief Enqueue an item.
	@note This is thread-safe.
	*/
	template<typename U>
	bool enqueue(U&& item);

	/*! @brief Try enqueue an item.
	@return `true` if the item is enqueued. `false` otherwise.
	@note This is thread-safe.
	*/
	template<typename U>
	bool tryEnqueue(U&& item);

	/*! @brief Try dequeue an item.
	@return `true` if an item is dequeued. `false` otherwise.
	@note This is thread-safe.
	*/
	bool tryDequeue(T* out_item);

	/*! @brief Approximated size of the queue.
	@return Number of items in the queue. The esimation is only accurate if all memory writes to the queue
	is guaranteed to be visible. Note that 0 may be returned even if the queue is, in face, not empty.
	@note This is thread-safe.
	*/
	std::size_t estimatedSize() const;

private:
	static_assert(moodycamel::ConcurrentQueue<T>::is_lock_free());

	moodycamel::ConcurrentQueue<T> m_queue;
};

}// end namespace ph

#include "Utility/Concurrent/TLockFreeQueue.ipp"
