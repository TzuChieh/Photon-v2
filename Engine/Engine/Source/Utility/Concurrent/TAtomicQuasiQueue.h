#pragma once

#include <Common/config.h>

#include <moodycamel/concurrentqueue.h>

#include <cstddef>
#include <iterator>

namespace ph
{

/*! @brief A multi-producer, multi-consumer, lock-free concurrent queue-like structure.
For single-thread uses, it is a FIFO queue. For multi-thread uses, it is *mostly* a FIFO queue.
Specifically, items put in by a given producer will all come out in the same order (FIFO). But
there is no coordination between items from other producers--two items put in by two different
producers will come out in an undefined order relative to each other (the interleaving between
different streams of items from different producers is undefined, even with external synchronization).
It is possible some items will starve in the queue if more items are enqueued than dequeued.
Guarantees aquire-release semantics for items that are enqueued/dequeued.
*/
template<typename T>
class TAtomicQuasiQueue final
{
public:
	TAtomicQuasiQueue();
	explicit TAtomicQuasiQueue(std::size_t initialCapacity);

	/*! @brief Enqueue an item. Allocate memory if required.
	Basic aquire-release semantics are guaranteed. This ensures that all the effects of work done 
	by a thread before it enqueues an item will be visible on another thread after it dequeues 
	that item. See tryDequeue(T*) for how to ensure the item can be dequeued on another thread. 
	@note Thread-safe.
	*/
	template<typename U>
	void enqueue(U&& item);

	/*! @brief Enqueue multiple items at once. Similar to `enqueue(1)`.
	Use `std::make_move_iterator` if the items should be moved instead of copied.
	*/
	template<std::input_iterator Iterator>
	void enqueueBulk(Iterator firstItem, std::size_t numItems);

	/*! @brief Try to enqueue an item. Never allocate memory.
	@return `true` if the item is enqueued. `false` otherwise.
	@note Thread-safe.
	*/
	template<typename U>
	bool tryEnqueue(U&& item);

	/*! @brief Try to dequeue an item.
	While there is no contention, dequeue fails if there is no item or the memory effect of enqueue
	is not visible. Dequeue can also fail spuriously if there is another consumer (under contention).
	To guarantee all enqueued items eventually got dequeued, memory effects must be made visible to
	the dequeuing threads.
	@return `true` if an item is dequeued. `false` otherwise (even if the queue is not empty).
	@note Thread-safe.
	*/
	bool tryDequeue(T* out_item);

	template<std::output_iterator<T> Iterator>
	std::size_t tryDequeueBulk(Iterator out_firstItem, std::size_t numItems);

	/*! @brief Approximated size of the queue.
	@return Number of items in the queue. The esimation is only accurate if all memory writes to the queue
	is guaranteed to be visible. Note that 0 may be returned even if the queue is, in fact, not empty.
	@note Thread-safe.
	*/
	std::size_t estimatedSize() const;

private:

#if PH_ENSURE_LOCKFREE_ALGORITHMS_ARE_LOCKLESS
	static_assert(moodycamel::ConcurrentQueue<T>::is_lock_free());
#endif

	/*
	`moodycamel::ConcurrentQueue` is lock free and the queue itself is thread-safe, meaning that using
	the queue under multiple producers and multiple consumers will not break the internal state of the
	queue, and all operations generally will not block in any way. However, there are several implications
	due to its design, the most important ones are:

	* Items enqueued by each thread will come out in the same order as they were enqueued, however, there
	is no defined item order between threads even with external synchronization. It is FIFO when viewed
	from any single thread, but not FIFO when viewed across threads. Normally this is not an issue, as if
	a total item order is desired, you need some sort of synchronization and might as well just queue into
	a synchronized queue from a single thread.
	See [1] ConcurrentQueue not a FIFO
	        https://github.com/cameron314/concurrentqueue/issues/52
	    [2] I can't understand how this queue ensure ordering between multiple producer...
	        https://github.com/cameron314/concurrentqueue/issues/262
	    [3] FIFO guarantees for SPMC usage?
		    https://github.com/cameron314/concurrentqueue/issues/309

	* Items enqueued on one thread may not be successfully dequeued in another thread, unless there are 
	external means to ensure the enqueue operation is visible on the dequeue thread.
	See [1] [SPMC] externally synchronized try_dequeue fails with items in queue
	        https://github.com/cameron314/concurrentqueue/issues/265
	    [2] try_enqueue() returns true but approx_size() is always 0 and try_dequeue() fails
	        https://github.com/cameron314/concurrentqueue/issues/285

	* All the memory effects done by a thread before it enqueues an item will be visible on another
	thread after it dequeues that item. Basic aquire-release semantics are guaranteed. This ensures 
	that all the effects of work done by a thread before it enqueues an element will be visible on 
	another thread after it dequeues that element.
	See [1] sequential consistent
	        https://github.com/cameron314/concurrentqueue/issues/73

	Most performant case: single-producer multi-consumer.
	*/
	moodycamel::ConcurrentQueue<T> m_queue;
};

}// end namespace ph

#include "Utility/Concurrent/TAtomicQuasiQueue.ipp"
