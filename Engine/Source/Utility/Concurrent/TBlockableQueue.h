#pragma once

#include <moodycamel/blockingconcurrentqueue.h>

#include <cstddef>

namespace ph
{

/*! @brief A multi-producer, multi-consumer, blockable version of the lock-free concurrent queue.
For single-thread uses, it is a FIFO queue. For multi-thread uses, it is *mostly* a FIFO queue.
Specifically, items put in by a given producer will all come out in the same order (FIFO). But
there is no coordination between items from other producers--two items put in by two different
producers will come out in an undefined order relative to each other (the interleaving between
different streams of items from different producers is undefined, even with external synchronization).
It is possible some items will starve in the queue if more items are enqueued than dequeued.
*/
template<typename T>
class TBlockableQueue final
{
public:
	TBlockableQueue();
	explicit TBlockableQueue(std::size_t initialCapacity);

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
	Dequeue fails if there is no item. Dequeue can fail spuriously if there is another consumer.
	@return `true` if an item is dequeued. `false` otherwise (even if the queue is not empty).
	@note Thread-safe.
	*/
	bool tryDequeue(T* out_item);

	/*! @brief Blocks the current thread until there is something to dequeue.
	@note Thread-safe.
	*/
	void waitDequeue(T* out_item);

	/*! @brief Approximated size of the queue.
	@return Number of items in the queue. The esimation is only accurate if all memory writes to the queue
	is guaranteed to be visible. Note that 0 may be returned even if the queue is, in fact, not empty.
	@note Thread-safe.
	*/
	std::size_t estimatedSize() const;

private:
	moodycamel::BlockingConcurrentQueue<T> m_queue;
};

}// end namespace ph

#include "Utility/Concurrent/TBlockableQueue.ipp"
