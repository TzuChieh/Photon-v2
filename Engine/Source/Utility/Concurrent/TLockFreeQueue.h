#pragma once

#include "Common/config.h"

#include <moodycamel/concurrentqueue.h>

#include <cstddef>

namespace ph
{

/*! @brief A multi-producer, multi-consumer, lock-free concurrent queue.
For single-thread uses, it is a FIFO queue. For multi-thread uses, it is *mostly* a FIFO queue.
Specifically, items put in by a given producer will all come out in the same order (FIFO). But
there is no coordination between items from other producers--two items put in by two different
producers will come out in an undefined order relative to each other (the interleaving between
different streams of items from different producers is undefined, even with external synchronization).
It is possible some items will starve in the queue if more items are enqueued than dequeued.
*/
template<typename T>
class TLockFreeQueue final
{
public:
	TLockFreeQueue();
	explicit TLockFreeQueue(std::size_t initialCapacity);

	/*! @brief Enqueue an item. Allocate memory if required.
	@note Thread-safe.
	*/
	template<typename U>
	void enqueue(U&& item);

	/*! @brief Try to enqueue an item. Never allocate memory.
	@return `true` if the item is enqueued. `false` otherwise.
	@note Thread-safe.
	*/
	template<typename U>
	bool tryEnqueue(U&& item);

	/*! @brief Try to dequeue an item.
	While there is no contention, dequeue fails if there is no item; otherwise, dequeue can fail 
	spuriously if there is another consumer (under contention).
	@return `true` if an item is dequeued. `false` otherwise (even if the queue is not empty).
	@note Thread-safe.
	*/
	bool tryDequeue(T* out_item);

	/*! @brief Approximated size of the queue.
	@return Number of items in the queue. The esimation is only accurate if all memory writes to the queue
	is guaranteed to be visible. Note that 0 may be returned even if the queue is, in fact, not empty.
	@note Thread-safe.
	*/
	std::size_t estimatedSize() const;

private:

#ifdef PH_ENSURE_LOCKFREE_ALGORITHMS_ARE_LOCKLESS
	static_assert(moodycamel::ConcurrentQueue<T>::is_lock_free());
#endif

	// Most performant case: single-producer multi-consumer
	moodycamel::ConcurrentQueue<T> m_queue;
};

}// end namespace ph

#include "Utility/Concurrent/TLockFreeQueue.ipp"
