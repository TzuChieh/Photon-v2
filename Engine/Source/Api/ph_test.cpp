#include "ph_test.h"
#include "Image/HDRFrame.h"
#include "World/World.h"
#include "Model/Geometry/GSphere.h"
#include "Model/Geometry/GTriangle.h"
#include "Camera/DefaultCamera.h"
#include "Core/BruteForceRenderer.h"
#include "Core/ImportanceRenderer.h"
#include "Core/MtImportanceRenderer.h"
#include "Math/random_number.h"
#include "Model/Material/MatteOpaque.h"
#include "Model/Material/LightMaterial.h"
#include "Model/Model.h"
#include "Model/Material/PerfectMirror.h"
#include "Model/Material/AbradedOpaque.h"
#include "Core/StandardSampleGenerator.h"
#include "Image/Film.h"
#include "Model/ModelLoader.h"
#include "Api/test_scene.h"

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

static const ph::uint32 widthPx = 1280;
static const ph::uint32 heightPx = 720;
static ph::HDRFrame testHdrFrame(widthPx, heightPx);

void genTestHdrFrame(const PHfloat32** out_data, PHuint32* out_widthPx, PHuint32* out_heightPx)
{
	using namespace ph;

	//auto renderer = std::make_shared<BruteForceRenderer>();
	//ImportanceRenderer renderer;
	MtImportanceRenderer renderer;
	StandardSampleGenerator sampleGenerator(16);
	World world;
	DefaultCamera camera;
	Film film(widthPx, heightPx);

	renderer.setSampleGenerator(&sampleGenerator);
	camera.setFilm(&film);
	camera.setPosition(Vector3f(0, 0, 4));

	load5bScene(&world);

	//auto lightMaterial = std::make_shared<LightMaterial>();
	//auto lightGeometry = std::make_shared<GSphere>(Vector3f(-1, 2, -9), 0.7f);
	//lightMaterial->setEmittedRadiance(1.0f, 1.0f, 0.2f);
	//world.addModel(Model(lightGeometry, lightMaterial));

	////auto sphereMaterial = std::make_shared<PerfectMirror>();
	//auto sphereMaterial = std::make_shared<MatteOpaque>();
	//auto sphereGeometry = std::make_shared<GSphere>(Vector3f(2, 0, -10), 1.5f);
	//sphereMaterial->setAlbedo(0.8f, 1.0f, 0.9f);
	//world.addModel(Model(sphereGeometry, sphereMaterial));

	//auto triangleMaterial = std::make_shared<PerfectMirror>();
	////auto triangleMaterial = std::make_shared<MatteOpaque>();
	//auto triangleGeometry = std::make_shared<GTriangle>(Vector3f(-4, 0, -10), Vector3f(0, 0, -10), Vector3f(-3, 5, -10));
	//world.addModel(Model(triangleGeometry, triangleMaterial));

	/*auto sphere2Material = std::make_shared<PerfectMirror>();
	auto sphere2Geometry = std::make_shared<GSphere>(Vector3f(-1, 0, -10), 0.8f);
	world.addModel(Model(sphere2Geometry, sphere2Material));*/

	ModelLoader modelLoader;
	//auto loadedModel = modelLoader.load("../SceneResource/cube.obj");
	Model loadedModel;
	bool isLoadingSuccess = modelLoader.load("../SceneResource/dragon.obj", &loadedModel);
	if(isLoadingSuccess)
	{
		loadedModel.translate(0, -2, -3);
		loadedModel.scale(2);
		loadedModel.rotate(Vector3f(0, 1, 0), 180);
		world.addModel(loadedModel);
	}

	std::cout << "cooking world" << std::endl;
	world.cook();

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
	*out_widthPx = 1280;
	*out_heightPx = 720;
}