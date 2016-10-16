#include "ph_test.h"

#include "Frame/HdrRectFrame.h"
#include "Utility/SharedData.h"
#include "Utility/OwnedData.h"

#include <iostream>

class Data
{
public:
	Data()
	{
		std::cout << "default ctor" << std::endl;
	}

	Data(const Data& other)
	{
		std::cout << "copy ctor" << std::endl;
	}

	Data(int a, float b)
	{
		std::cout << "int float ctor" << std::endl;
	}

	~Data()
	{
		std::cout << "dtor" << std::endl;
	}

	void printSomething() const
	{
		std::cout << "print print print something" << std::endl;
	}
};

template<typename T>
class Test
{
public:
	Test(T t)
	{
		m_t = t;
	}

private:
	T m_t;
};

template<typename T>
static void func(T&& t)
{

}

void printTestMessage()
{
	std::cout << "Hello, this is some test message." << std::endl;
}

void testRun()
{
	using namespace ph;

	std::cout << "Hello World!" << std::endl;

	HdrRectFrame hdrFrame(1280, 720);

	//Data data;
	//func(data);

	SharedData<Data> data = SharedData<Data>::create(1, 5.2f);
	OwnedData<Data> data2(2, 7.77f);
	data2->printSomething();
}