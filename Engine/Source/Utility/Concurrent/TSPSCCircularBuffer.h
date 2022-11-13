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
#ifdef PH_DEBUG
		bool             isBetweenProduceBeginAndEnd;
		bool             isBetweenConsumeBeginAndEnd;
#endif

		inline Item()
			: storedItem                 ()
			, isSealedForConsume         ()
#ifdef PH_DEBUG
			, isBetweenProduceBeginAndEnd(false)
			, isBetweenConsumeBeginAndEnd(false)
#endif
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

	/*!
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

#ifdef PH_DEBUG
		getCurrentProducerItem().isBetweenProduceBeginAndEnd = true;
#endif
	}

	/*!
	@note Producer thread only.
	*/
	inline void endProduce()
	{
		PH_ASSERT(isProducerThread());

#ifdef PH_DEBUG
		getCurrentProducerItem().isBetweenProduceBeginAndEnd = false;
#endif

		// Mark that current item is now set and is ready for being consumed
		Item& currentItem = getCurrentProducerItem();
		const bool hasAlreadySealed = currentItem.isSealedForConsume.test_and_set(std::memory_order_release);
		PH_ASSERT(!hasAlreadySealed);

		// `item` can no longer be modified by producer

		// Notify the potentially waiting consumer so it can start consuming this item
		currentItem.isSealedForConsume.notify_one();

		advanceProduceHead();
	}

	/*!
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

#ifdef PH_DEBUG
		getCurrentConsumerItem().isBetweenConsumeBeginAndEnd = true;
#endif
	}

	/*!
	@note Consumer thread only.
	*/
	inline void endConsume()
	{
		PH_ASSERT(isConsumerThread());

#ifdef PH_DEBUG
		getCurrentConsumerItem().isBetweenConsumeBeginAndEnd = false;
#endif

		Item& currentItem = getCurrentConsumerItem();
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
		PH_ASSERT(getCurrentProducerItem().isBetweenProduceBeginAndEnd);

		return getCurrentProducerItem().storedItem;
	}

	inline const T& getBufferForProducer() const
	{
		PH_ASSERT(getCurrentProducerItem().isBetweenProduceBeginAndEnd);

		return getCurrentProducerItem().storedItem;
	}
	///@}

	/*!
	@note Consumer thread only.
	*/
	///@{
	inline T& getBufferForConsumer()
	{
		PH_ASSERT(getCurrentConsumerItem().isBetweenConsumeBeginAndEnd);

		return getCurrentConsumerItem().storedItem;
	}

	inline const T& getBufferForConsumer() const
	{
		PH_ASSERT(getCurrentConsumerItem().isBetweenConsumeBeginAndEnd);

		return getCurrentConsumerItem().storedItem;
	}
	///@}

	/*!
	@note Producer thread only.
	*/
	inline std::size_t getProduceHead() const
	{
		PH_ASSERT(getCurrentProducerItem().isBetweenProduceBeginAndEnd);

		return m_produceHead;
	}

	/*!
	@note Consumer thread only.
	*/
	inline std::size_t getConsumeHead() const
	{
		PH_ASSERT(getCurrentConsumerItem().isBetweenConsumeBeginAndEnd);

		return m_consumeHead;
	}

	/*!
	@note Producer thread only.
	*/
	inline bool mayWaitForProduce() const
	{
#ifdef PH_DEBUG
		markProducerThread();
#endif

		return getCurrentProducerItem().isSealedForConsume(std::memory_order_relaxed);
	}

	/*!
	@note Consumer thread only.
	*/
	inline bool mayWaitForConsume() const
	{
#ifdef PH_DEBUG
		markConsumerThread();
#endif

		return !getCurrentConsumerItem().isSealedForConsume(std::memory_order_relaxed);
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

	inline void markProducerThread()
	{
		// Lazily set producer thread ID
		m_producerThreadID = m_producerThreadID != std::thread::id() ?
			m_producerThreadID : std::this_thread::get_id();
	}

	inline void markConsumerThread()
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

	std::array<Item, N> m_items;
	std::size_t         m_produceHead;
	std::size_t         m_consumeHead;
#ifdef PH_DEBUG
	// Though they are lazily set, no need to synchronize them since if everything is used correctly,
	// each of them should be loaded/stored from their own thread. Seeing uninitialized or corrupted 
	// value generally will cause the comparison to current thread ID to fail which is also what we want.
	std::thread::id     m_producerThreadID;
	std::thread::id     m_consumerThreadID;
#endif
};

}// end namespace ph
