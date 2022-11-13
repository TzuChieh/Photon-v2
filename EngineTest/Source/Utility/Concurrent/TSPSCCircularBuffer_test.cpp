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
	}
	
	{
		TSPSCCircularBuffer<double, 1> doubleBuffer;
		TSPSCCircularBuffer<double, 2> doubleBuffer2;
		TSPSCCircularBuffer<double, 100> doubleBuffer3;
	}
	
	{
		TSPSCCircularBuffer<std::string, 1> strBuffer;
		TSPSCCircularBuffer<std::string, 2> strBuffer2;
		TSPSCCircularBuffer<std::string, 100> strBuffer3;
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
					buffer.beginProduce();

					double& number = buffer.getBufferForProducer();
					number = i;

					buffer.endProduce();
				}
			});

		std::thread consumer(
			[&buffer, numIterations]()
			{
				for(int i = 0; i < numIterations; ++i)
				{
					buffer.beginConsume();

					const double& number = buffer.getBufferForConsumer();
					EXPECT_EQ(number, i);

					buffer.endConsume();
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
					numbers.y = i * 2;
					numbers.z = i * 3;

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
					EXPECT_EQ(numbers.y, i * 2);
					EXPECT_EQ(numbers.z, i * 3);

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
