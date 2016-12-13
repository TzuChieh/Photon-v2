#include "ph_test.h"
#include "Image/HDRFrame.h"
#include "World/World.h"
#include "Model/Geometry/GSphere.h"
#include "Model/Geometry/GTriangle.h"
#include "Camera/DefaultCamera.h"
#include "Core/MtImportanceRenderer.h"
#include "Math/random_number.h"
#include "Model/Material/MatteOpaque.h"
#include "Model/Material/LightMaterial.h"
#include "Model/Model.h"
#include "Model/Material/PerfectMirror.h"
#include "Model/Material/AbradedOpaque.h"
#include "Core/SampleGenerator/PixelJitterSampleGenerator.h"
#include "Image/Film/Film.h"
#include "Model/ModelLoader.h"
#include "Api/test_scene.h"
#include "Model/Geometry/GRectangle.h"
#include "Image/CheckerboardTexture.h"
#include "Model/TextureMapper/SphericalMapper.h"

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
		std::cout << genRandomFloat32_0_1_uniform() << std::endl;
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

	//auto renderer = std::make_shared<BruteForceRenderer>();
	//ImportanceRenderer renderer;
	MtImportanceRenderer renderer;
	PixelJitterSampleGenerator sampleGenerator(32);
	World world;
	DefaultCamera camera;
	Film film(widthPx, heightPx);

	renderer.setSampleGenerator(&sampleGenerator);
	camera.setFilm(&film);
	//camera.setPosition(Vector3f(0, 0, 4));
	camera.setPosition(Vector3f(0, 0, 16));
	

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
	renderer.render(world, camera);
	film.developFilm(&testHdrFrame);

	// stop timer
	QueryPerformanceCounter(&t2);

	// compute and print the elapsed time in millisec
	elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;

	std::cout << "time elapsed: " << elapsedTime << " ms" << std::endl;

	*out_data = testHdrFrame.getPixelData();
	*out_widthPx = widthPx;
	*out_heightPx = heightPx;
}