#include "ph_test.h"
#include "Frame/HDRFrame.h"
#include "Core/World.h"
#include "Model/Geometry/GSphere.h"
#include "Camera/DefaultCamera.h"
#include "Core/PathTracer.h"
#include "Math/random_number.h"
#include "Model/Material/MatteOpaque.h"
#include "Model/Material/LightMaterial.h"
#include "Model/Model.h"

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

	HDRFrame hdrFrame(1280, 720);

	//Data data;
	//func(data);

	//OwnedData<Data> data2(2, 7.77f);
	//data2->printSomething();

	for(int i = 0; i < 20; i++)
	{
		std::cout << genRandomFloat32_0_1_uniform() << std::endl;
	}

	/*World world;
	DefaultCamera camera;

	world.addPrimitive(std::make_shared<Sphere>(Vector3f(2, 0, -10), 1.5f));

	PathTracer pathTracer;
	pathTracer.trace(camera, world, &hdrFrame);*/
}

static ph::HDRFrame testHdrFrame(1280, 720);

void genTestHdrFrame(const PHfloat32** out_data, PHuint32* out_widthPx, PHuint32* out_heightPx)
{
	using namespace ph;

	World world;
	DefaultCamera camera;

	

	auto sphereMaterial = std::make_shared<MatteOpaque>();
	auto sphereGeometry = std::make_shared<GSphere>(Vector3f(2, 0, -10), 1.5f);
	world.addModel(Model(sphereGeometry, sphereMaterial));

	auto lightMaterial = std::make_shared<LightMaterial>();
	auto lightGeometry = std::make_shared<GSphere>(Vector3f(-2, 0, -10), 0.5f);
	world.addModel(Model(lightGeometry, lightMaterial));


	world.cook();

	PathTracer pathTracer;
	pathTracer.trace(camera, world, &testHdrFrame);

	*out_data = testHdrFrame.getPixelData();
	*out_widthPx = 1280;
	*out_heightPx = 720;
}