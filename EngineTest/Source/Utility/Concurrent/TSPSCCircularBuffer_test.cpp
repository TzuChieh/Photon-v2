#include <Utility/Concurrent/TSPSCCircularBuffer.h>

#include <gtest/gtest.h>

#include <string>
#include <thread>

using namespace ph;

TEST(TSPSCCircularBufferTest, CtorDtor)
{
	{
		TSPSCCircularBuffer<int, 1> intBuffer;
		TSPSCCircularBuffer<int, 2> intBuffer2;
		TSPSCCircularBuffer<int, 100> intBuffer3;

		// No way that they start out just for waiting
		EXPECT_FALSE(intBuffer.mayWaitToProduce());
		EXPECT_FALSE(intBuffer2.mayWaitToProduce());
		EXPECT_FALSE(intBuffer3.mayWaitToProduce());

		// No value was produced, consumer must wait
		EXPECT_TRUE(intBuffer.mayWaitToConsume());
		EXPECT_TRUE(intBuffer2.mayWaitToConsume());
		EXPECT_TRUE(intBuffer3.mayWaitToConsume());
	}
	
	{
		TSPSCCircularBuffer<double, 1> doubleBuffer;
		TSPSCCircularBuffer<double, 2> doubleBuffer2;
		TSPSCCircularBuffer<double, 100> doubleBuffer3;

		// No way that they start out just for waiting
		EXPECT_FALSE(doubleBuffer.mayWaitToProduce());
		EXPECT_FALSE(doubleBuffer2.mayWaitToProduce());
		EXPECT_FALSE(doubleBuffer3.mayWaitToProduce());

		// No value was produced, consumer must wait
		EXPECT_TRUE(doubleBuffer.mayWaitToConsume());
		EXPECT_TRUE(doubleBuffer2.mayWaitToConsume());
		EXPECT_TRUE(doubleBuffer3.mayWaitToConsume());
	}
	
	{
		TSPSCCircularBuffer<std::string, 1> strBuffer;
		TSPSCCircularBuffer<std::string, 2> strBuffer2;
		TSPSCCircularBuffer<std::string, 100> strBuffer3;

		// No way that they start out just for waiting
		EXPECT_FALSE(strBuffer.mayWaitToProduce());
		EXPECT_FALSE(strBuffer2.mayWaitToProduce());
		EXPECT_FALSE(strBuffer3.mayWaitToProduce());

		// No value was produced, consumer must wait
		EXPECT_TRUE(strBuffer.mayWaitToConsume());
		EXPECT_TRUE(strBuffer2.mayWaitToConsume());
		EXPECT_TRUE(strBuffer3.mayWaitToConsume());
	}
}

TEST(TSPSCCircularBufferTest, Running)
{
	// Baseline: simple buffer of size 1
	{
		constexpr int numIterations = 1000;
		
		TSPSCCircularBuffer<double, 1> buffer;

		std::thread producer(
			[&buffer, numIterations]()
			{
				for(int i = 0; i < numIterations; ++i)
				{
					EXPECT_FALSE(buffer.isProducing());
					buffer.beginProduce();

					EXPECT_TRUE(buffer.isProducing());

					double& number = buffer.getBufferForProducer();
					number = i;

					buffer.endProduce();
					EXPECT_FALSE(buffer.isProducing());
				}
			});

		std::thread consumer(
			[&buffer, numIterations]()
			{
				for(int i = 0; i < numIterations; ++i)
				{
					EXPECT_FALSE(buffer.isConsuming());
					buffer.beginConsume();

					EXPECT_TRUE(buffer.isConsuming());

					const double& number = buffer.getBufferForConsumer();
					EXPECT_EQ(number, i);

					buffer.endConsume();
					EXPECT_FALSE(buffer.isConsuming());
				}
			});

		producer.join();
		consumer.join();
	}

	// Baseline: struct buffer of size 3
	{
		constexpr int numIterations = 1000;

		struct Numbers
		{
			int x = -3;
			float y = 1234.567f;
			double z = -999.0;
		};
		
		TSPSCCircularBuffer<Numbers, 3> buffer;

		std::thread producer(
			[&buffer, numIterations]()
			{
				for(int i = 0; i < numIterations; ++i)
				{
					buffer.beginProduce();

					Numbers& numbers = buffer.getBufferForProducer();
					numbers.x = i;
					numbers.y = i * 2.0f;
					numbers.z = i * 3.0;

					buffer.endProduce();
				}
			});

		std::thread consumer(
			[&buffer, numIterations]()
			{
				for(int i = 0; i < numIterations; ++i)
				{
					buffer.beginConsume();

					const Numbers& numbers = buffer.getBufferForConsumer();
					EXPECT_EQ(numbers.x, i);
					EXPECT_EQ(numbers.y, i * 2.0f);
					EXPECT_EQ(numbers.z, i * 3.0);

					buffer.endConsume();
				}
			});

		producer.join();
		consumer.join();
	}

	// string buffer of size 5
	{
		constexpr int numIterations = 1000;
		
		TSPSCCircularBuffer<std::string, 5> buffer;

		std::thread producer(
			[&buffer, numIterations]()
			{
				for(int i = 0; i < numIterations; ++i)
				{
					buffer.beginProduce();

					std::string& str = buffer.getBufferForProducer();
					str = std::to_string(i) + "hello";

					buffer.endProduce();
				}
			});

		std::thread consumer(
			[&buffer, numIterations]()
			{
				for(int i = 0; i < numIterations; ++i)
				{
					buffer.beginConsume();

					const std::string& str = buffer.getBufferForConsumer();
					EXPECT_STREQ(str.c_str(), (std::to_string(i) + "hello").c_str());

					buffer.endConsume();
				}
			});

		producer.join();
		consumer.join();
	}
}
