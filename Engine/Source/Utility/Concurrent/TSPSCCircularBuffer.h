#pragma once

#include "Common/assertion.h"
#include "Common/config.h"
#include "Math/math.h"

#include <cstddef>
#include <type_traits>
#include <array>
#include <thread>
#include <atomic>

namespace ph
{

template<typename T, std::size_t N>
class TSPSCCircularBuffer final
{
	static_assert(std::is_default_constructible_v<T>);
	static_assert(N != 0);

private:
	struct Item final
	{
		T                storedItem;
		std::atomic_flag isSealedForConsume;

		// modify by `beginProcued()` & `endProduce()`, read-only otherwise
		bool isBetweenProduceBeginAndEnd;

		// modify by `beginConsume()` & `endConsume()`, read-only otherwise
		bool isBetweenConsumeBeginAndEnd;

		inline Item()
			: storedItem                 ()
			, isSealedForConsume         ()
			, isBetweenProduceBeginAndEnd(false)
			, isBetweenConsumeBeginAndEnd(false)
		{}
	};

public:
	inline TSPSCCircularBuffer()
		: m_items           ()
		, m_produceHead     (0)
		, m_consumeHead     (0)
#ifdef PH_DEBUG
		, m_producerThreadID()
		, m_consumerThreadID()
#endif
	{}

	/*! @brief Start producing. Will wait if buffer is unavailable for now.
	@note Producer thread only.
	*/
	inline void beginProduce()
	{
#ifdef PH_DEBUG
		markProducerThread();
#endif

		PH_ASSERT(isProducerThread());

		// Wait until current item is available for producer
		Item& currentItem = getCurrentProducerItem();
		currentItem.isSealedForConsume.wait(true, std::memory_order_acquire);

		// `item` can now be modified by producer

		currentItem.isBetweenProduceBeginAndEnd = true;
	}

	/*! @brief Stop producing. Consumer will unwait and start to consume.
	@note Producer thread only.
	*/
	inline void endProduce()
	{
		PH_ASSERT(isProducerThread());

		Item& currentItem = getCurrentProducerItem();
		currentItem.isBetweenProduceBeginAndEnd = false;

		// Mark that current item is now set and is ready for being consumed
		const bool hasAlreadySealed = currentItem.isSealedForConsume.test_and_set(std::memory_order_release);
		PH_ASSERT(!hasAlreadySealed);

		// `item` can no longer be modified by producer

		// Notify the potentially waiting consumer so it can start consuming this item
		currentItem.isSealedForConsume.notify_one();

		advanceProduceHead();
	}

	/*! @brief Start consuming. Will wait if buffer is unavailable for now.
	@note Consumer thread only.
	*/
	inline void beginConsume()
	{
#ifdef PH_DEBUG
		markConsumerThread();
#endif

		PH_ASSERT(isConsumerThread());

		// Wait until being notified there is new item that can be consumed
		Item& currentItem = getCurrentConsumerItem();
		currentItem.isSealedForConsume.wait(false, std::memory_order_acquire);

		// `item` can now be modified by consumer

		currentItem.isBetweenConsumeBeginAndEnd = true;
	}

	/*! @brief Stop consuming. Producer will unwait and start to produce.
	@note Consumer thread only.
	*/
	inline void endConsume()
	{
		PH_ASSERT(isConsumerThread());

		Item& currentItem = getCurrentConsumerItem();
		currentItem.isBetweenConsumeBeginAndEnd = false;

		// Mark that current item is now consumed and is ready for use by producer again
		currentItem.isSealedForConsume.clear(std::memory_order_release);

		// `item` can no longer be modified by consumer

		// Notify the potentially waiting producer so it can start producing
		currentItem.isSealedForConsume.notify_one();

		advanceConsumeHead();
	}

	/*!
	@note Producer thread only.
	*/
	///@{
	inline T& getBufferForProducer()
	{
		PH_ASSERT(isProducing());
		return getCurrentProducerItem().storedItem;
	}

	inline const T& getBufferForProducer() const
	{
		PH_ASSERT(isProducing());
		return getCurrentProducerItem().storedItem;
	}
	///@}

	/*!
	@note Consumer thread only.
	*/
	///@{
	inline T& getBufferForConsumer()
	{
		PH_ASSERT(isConsuming());
		return getCurrentConsumerItem().storedItem;
	}

	inline const T& getBufferForConsumer() const
	{
		PH_ASSERT(isConsuming());
		return getCurrentConsumerItem().storedItem;
	}
	///@}

	/*!
	@note Producer thread only.
	*/
	inline std::size_t getProduceHead() const
	{
		PH_ASSERT(isProducing());
		return m_produceHead;
	}

	/*!
	@note Consumer thread only.
	*/
	inline std::size_t getConsumeHead() const
	{
		PH_ASSERT(isConsuming());
		return m_consumeHead;
	}

	/*! @brief Whether the next call to beginProduce() will potentially wait (block).
	@note Producer thread only.
	*/
	inline bool mayWaitToProduce() const
	{
#ifdef PH_DEBUG
		markProducerThread();
#endif

		PH_ASSERT(isProducerThread());
		return getCurrentProducerItem().isSealedForConsume.test(std::memory_order_acquire);
	}

	/*!@brief Whether the next call to beginConsume() will potentially wait (block).
	@note Consumer thread only.
	*/
	inline bool mayWaitToConsume() const
	{
#ifdef PH_DEBUG
		markConsumerThread();
#endif

		PH_ASSERT(isConsumerThread());
		return !getCurrentConsumerItem().isSealedForConsume.test(std::memory_order_acquire);
	}

	/*! @brief Whether this thread is producing to the buffer made current.
	@note Producer thread only.
	*/
	inline bool isProducing() const
	{
#ifdef PH_DEBUG
		markProducerThread();
#endif

		PH_ASSERT(isProducerThread());
		return getCurrentProducerItem().isBetweenProduceBeginAndEnd;
	}

	/*! @brief Whether this thread is consuming from the buffer made current.
	@note Consumer thread only.
	*/
	inline bool isConsuming() const
	{
#ifdef PH_DEBUG
		markConsumerThread();
#endif

		PH_ASSERT(isConsumerThread());
		return getCurrentConsumerItem().isBetweenConsumeBeginAndEnd;
	}

private:
	/*!
	@note Producer threads only.
	*/
	///@{
	inline const Item& getCurrentProducerItem() const
	{
		PH_ASSERT(isProducerThread());
		return m_items[m_produceHead];
	}

	inline Item& getCurrentProducerItem()
	{
		PH_ASSERT(isProducerThread());
		return m_items[m_produceHead];
	}
	///@}

	/*!
	@note Consumer thread only.
	*/
	///@{
	inline const Item& getCurrentConsumerItem() const
	{
		PH_ASSERT(isConsumerThread());
		return m_items[m_consumeHead];
	}

	inline Item& getCurrentConsumerItem()
	{
		PH_ASSERT(isConsumerThread());
		return m_items[m_consumeHead];
	}
	///@}

	/*!
	@note Producer thread only.
	*/
	inline void advanceProduceHead()
	{
		PH_ASSERT(isProducerThread());
		m_produceHead = getNextProducerConsumerHead(m_produceHead);
	}

	/*!
	@note Consumer thread only.
	*/
	inline void advanceConsumeHead()
	{
		PH_ASSERT(isConsumerThread());
		m_consumeHead = getNextProducerConsumerHead(m_consumeHead);
	}

#ifdef PH_DEBUG
	/*! @brief Check if this thread is producer thread.
	@note Thread-safe.
	*/
	inline bool isProducerThread() const
	{
		PH_ASSERT(m_producerThreadID != std::thread::id());
		return std::this_thread::get_id() == m_producerThreadID;
	}

	/*! @brief Check if this thread is consumer thread.
	@note Thread-safe.
	*/
	inline bool isConsumerThread() const
	{
		PH_ASSERT(m_consumerThreadID != std::thread::id());
		return std::this_thread::get_id() == m_consumerThreadID;
	}

	inline void markProducerThread() const
	{
		// Lazily set producer thread ID
		m_producerThreadID = m_producerThreadID != std::thread::id() ?
			m_producerThreadID : std::this_thread::get_id();
	}

	inline void markConsumerThread() const
	{
		// Lazily set consumer thread ID
		m_consumerThreadID = m_consumerThreadID != std::thread::id() ?
			m_consumerThreadID : std::this_thread::get_id();
	}
#endif

	/*!
	@note Thread-safe.
	*/
	inline static std::size_t getNextProducerConsumerHead(const std::size_t currentProducerConsumerHead)
	{
		return math::wrap<std::size_t>(currentProducerConsumerHead + 1, 0, N - 1);
	}

	std::array<Item, N>     m_items;
	std::size_t             m_produceHead;
	std::size_t             m_consumeHead;
#ifdef PH_DEBUG
	// Though they are lazily set, no need to synchronize them since if everything is used correctly,
	// each of them should be loaded/stored from their own thread. Seeing uninitialized or corrupted 
	// value generally will cause the comparison to current thread ID to fail which is also what we want.
	mutable std::thread::id m_producerThreadID;
	mutable std::thread::id m_consumerThreadID;
#endif
};

}// end namespace ph
