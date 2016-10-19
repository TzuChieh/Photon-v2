#include "ph_test.h"
#include "Frame/HdrFrame.h"
#include "Utility/OwnedData.h"
#include "Core/World.h"
#include "Model/Primitive/Sphere.h"
#include "Camera/DefaultCamera.h"
#include "Core/PathTracer.h"

#include <iostream>
#include <memory>

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

	HdrFrame hdrFrame(1280, 720);

	//Data data;
	//func(data);

	OwnedData<Data> data2(2, 7.77f);
	data2->printSomething();

	World world;
	DefaultCamera camera;

	world.addPrimitive(std::make_shared<Sphere>(Vector3f(2, 0, -10), 1.5f));

	PathTracer pathTracer;
	pathTracer.trace(camera, world, &hdrFrame);
}

static ph::HdrFrame testHdrFrame(1280, 720);

void genTestHdrFrame(const PHfloat32** out_data, PHuint32* out_widthPx, PHuint32* out_heightPx)
{
	using namespace ph;

	World world;
	DefaultCamera camera;

	world.addPrimitive(std::make_shared<Sphere>(Vector3f(2, 0, -10), 1.5f));

	PathTracer pathTracer;
	pathTracer.trace(camera, world, &testHdrFrame);

	*out_data = testHdrFrame.getPixelData();
}