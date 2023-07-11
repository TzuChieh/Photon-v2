#pragma once

#include "Common/assertion.h"
#include "Common/config.h"
#include "Math/math.h"
#include "Utility/INoCopyAndMove.h"
#include "Utility/utility.h"

#include <cstddef>
#include <type_traits>
#include <array>
#include <thread>
#include <atomic>

namespace ph
{

template<typename T, std::size_t N>
class TSPSCRingBuffer final
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
	inline TSPSCRingBuffer()
		: m_items           ()
		, m_produceHead     (0)
		, m_consumeHead     (0)
		, m_headDistance    (0)
#if PH_DEBUG
		, m_producerThreadID()
		, m_consumerThreadID()
#endif
	{}

	/*! @brief Start producing. Will wait if buffer is unavailable for now.
	@note Producer thread only.
	*/
	inline void beginProduce()
	{
#if PH_DEBUG
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

		// Increment before flag release--we want this addition to be visible on consumer
		m_headDistance.fetch_add(1, std::memory_order_relaxed);

		Item& currentItem = getCurrentProducerItem();
		currentItem.isBetweenProduceBeginAndEnd = false;

		// `item` can no longer be modified by producer
		
		// Mark that current item is now set and is ready for being consumed
		const bool hasAlreadySealed = currentItem.isSealedForConsume.test_and_set(std::memory_order_release);
		PH_ASSERT(!hasAlreadySealed);

		// Notify the potentially waiting consumer so it can start consuming this item
		currentItem.isSealedForConsume.notify_one();

		advanceProduceHead();
	}

	/*! @brief Start consuming. Will wait if buffer is unavailable for now.
	@note Consumer thread only.
	*/
	inline void beginConsume()
	{
#if PH_DEBUG
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

		// Decrement after flag acquire--will never underflow since every consumption happens after a
		// production with memory effects made visible
		m_headDistance.fetch_sub(1, std::memory_order_relaxed);

		Item& currentItem = getCurrentConsumerItem();
		currentItem.isBetweenConsumeBeginAndEnd = false;

		// `item` can no longer be modified by consumer

		// Mark that current item is now consumed and is ready for use by producer again
		currentItem.isSealedForConsume.clear(std::memory_order_release);

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

	/*! @brief Get buffer directly without any thread safety guarantee.
	*/
	///@{
	inline T& getBufferDirectly(const std::size_t index)
	{
		PH_ASSERT_LT(index, N);
		return m_items[index].storedItem;
	}

	inline const T& getBufferDirectly(const std::size_t index) const
	{
		PH_ASSERT_LT(index, N);
		return m_items[index].storedItem;
	}
	///@}

	/*! @brief Get the distance between produce and consume heads without limited thread safety guarantee.
	This method is thread safe, however note that when called during producing/consuming operations the
	result is an approximation to the actual distance. The distance is only exact if the buffer is
	synchronized externally and no one is currently producing/consuming the buffer.
	*/
	inline std::size_t getHeadDistanceDirectly() const
	{
		// Pad produce head to the next cycle first to ensure positive result, then mod it.
		// 
		// `return (m_produceHead + N - m_consumeHead) % N;`
		//
		// Above is the straightforward way of calculating distance, however the result is ambiguous on
		// determining whether the buffer is empty or full--returning 0 on both conditions.

		return lossless_integer_cast<std::size_t>(
			m_headDistance.load(std::memory_order_relaxed));
	}

	/*! @brief Get the buffer index to be accessed by producer thread.
	@note Producer thread only.
	*/
	inline std::size_t getProduceHead() const
	{
		PH_ASSERT(isProducerThread());
		return m_produceHead;
	}

	/*! @brief Get the buffer index to be accessed by consumer thread.
	@note Consumer thread only.
	*/
	inline std::size_t getConsumeHead() const
	{
		PH_ASSERT(isConsumerThread());
		return m_consumeHead;
	}

	/*! @brief Whether the next call to beginProduce() will potentially wait (block).
	@note Producer thread only.
	*/
	inline bool mayWaitToProduce() const
	{
#if PH_DEBUG
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
#if PH_DEBUG
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
#if PH_DEBUG
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
#if PH_DEBUG
		markConsumerThread();
#endif

		PH_ASSERT(isConsumerThread());
		return getCurrentConsumerItem().isBetweenConsumeBeginAndEnd;
	}

public:
	class ProducerGuard final : private INoCopyAndMove
	{
	public:
		inline explicit ProducerGuard(TSPSCRingBuffer& buffer)
			: m_buffer(buffer)
		{
			m_buffer.beginProduce();
		}

		inline ~ProducerGuard()
		{
			m_buffer.endProduce();
		}

	private:
		TSPSCRingBuffer& m_buffer;
	};

	class ConsumerGuard final : private INoCopyAndMove
	{
	public:
		inline explicit ConsumerGuard(TSPSCRingBuffer& buffer)
			: m_buffer(buffer)
		{
			m_buffer.beginConsume();
		}

		inline ~ConsumerGuard()
		{
			m_buffer.endConsume();
		}

	private:
		TSPSCRingBuffer& m_buffer;
	};

	template<typename ProducerFunc>
	inline void guardedProduce(ProducerFunc func)
	{
		static_assert(std::is_invocable_v<ProducerFunc, T&>,
			"ProducerFunc must take (T&).");

		ProducerGuard guard(*this);

		func(getBufferForProducer());
	}

	template<typename ConsumerFunc>
	inline void guardedConsume(ConsumerFunc func)
	{
		static_assert(std::is_invocable_v<ConsumerFunc, T&>,
			"ConsumerFunc must take (T&).");

		ConsumerGuard guard(*this);

		func(getBufferForConsumer());
	}

	/*!
	@note Thread-safe.
	*/
	inline static std::size_t nextProducerConsumerHead(
		const std::size_t currentProducerConsumerHead,
		const std::size_t numAdvancements = 1)
	{
		return math::wrap<std::size_t>(currentProducerConsumerHead + numAdvancements, 0, N - 1);
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
		m_produceHead = nextProducerConsumerHead(m_produceHead);
	}

	/*!
	@note Consumer thread only.
	*/
	inline void advanceConsumeHead()
	{
		PH_ASSERT(isConsumerThread());
		m_consumeHead = nextProducerConsumerHead(m_consumeHead);
	}

#if PH_DEBUG
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

	std::array<Item, N>       m_items;
	std::size_t               m_produceHead;
	std::size_t               m_consumeHead;
	std::atomic_uint_fast64_t m_headDistance;
#if PH_DEBUG
	// Though they are lazily set, no need to synchronize them since if everything is used correctly,
	// each of them should be loaded/stored from their own thread. Seeing uninitialized or corrupted 
	// value generally will cause the comparison to current thread ID to fail which is also what we want.
	mutable std::thread::id   m_producerThreadID;
	mutable std::thread::id   m_consumerThreadID;
#endif
};

}// end namespace ph
