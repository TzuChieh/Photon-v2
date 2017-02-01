#include "ph_test.h"
#include "Filmic/HDRFrame.h"
#include "World/World.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Geometry/GTriangle.h"
#include "Camera/PinholeCamera.h"
#include "Core/Renderer.h"
#include "Math/random_number.h"
#include "Actor/Material/MatteOpaque.h"
#include "Actor/AModel.h"
#include "Actor/Material/AbradedOpaque.h"
#include "Core/SampleGenerator/PixelJitterSampleGenerator.h"
#include "Filmic/Film.h"
#include "Actor/ModelLoader.h"
#include "Api/test_scene.h"
#include "Actor/Geometry/GRectangle.h"
#include "Actor/Texture/CheckerboardTexture.h"
#include "Actor/TextureMapper/SphericalMapper.h"

#include <iostream>
#include <memory>
#include <windows.h>

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
		std::cout << genRandomReal_0_1_uniform() << std::endl;
	}

	/*World world;
	DefaultCamera camera;

	world.addPrimitive(std::make_shared<Sphere>(Vector3f(2, 0, -10), 1.5f));

	PathTracer pathTracer;
	pathTracer.trace(camera, world, &hdrFrame);*/
}

static const ph::uint32 widthPx = 900;
static const ph::uint32 heightPx = 900;
static ph::HDRFrame testHdrFrame(widthPx, heightPx);

void genTestHdrFrame(const PHfloat32** out_data, PHuint32* out_widthPx, PHuint32* out_heightPx)
{
	using namespace ph;

	Renderer renderer(4);
	PixelJitterSampleGenerator sampleGenerator(32);
	World world;
	PinholeCamera camera;
	Film film(widthPx, heightPx);

	//renderer.setSampleGenerator(&sampleGenerator);
	camera.setFilm(&film);
	//camera.setPosition(Vector3f(0, 0, 4));
	camera.setPosition(Vector3R(0, 0, 16));
	

	//load5bScene(&world);
	loadTestScene(&world);

	std::cout << "cooking world" << std::endl;
	// HACK
	world.update(0.0f);

	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER t1, t2;           // ticks
	double elapsedTime;

	// get ticks per second
	QueryPerformanceFrequency(&frequency);

	// start timer
	QueryPerformanceCounter(&t1);

	// do something
	std::cout << "rendering world" << std::endl;
	/*renderer.render(world, camera);
	film.developFilm(&testHdrFrame);*/

	// stop timer
	QueryPerformanceCounter(&t2);

	// compute and print the elapsed time in millisec
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;

	std::cout << "time elapsed: " << elapsedTime << " ms" << std::endl;

	*out_data = testHdrFrame.getPixelData();
	*out_widthPx = widthPx;
	*out_heightPx = heightPx;
}