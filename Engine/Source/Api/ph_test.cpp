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
#include "Core/RenderTask.h"
#include "Model/ModelLoader.h"

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

void loadClassicCornellBoxScene(ph::World* out_world);

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

	loadClassicCornellBoxScene(&world);

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
	auto loadedModel = modelLoader.load("../SceneResource/dragon.obj");
	if(loadedModel != nullptr)
	{
		world.addModel(*loadedModel);
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
	RenderTask renderTask(&renderer, &world, &camera);
	renderTask.run();
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

void loadClassicCornellBoxScene(ph::World* out_world)
{
	using namespace ph;

	const float32 wallRadius = 1000.0f;
	const float32 boxHalfSize = 5.0f;

	auto leftWallBall = std::make_shared<GSphere>(Vector3f(-wallRadius - boxHalfSize, 0, 0), wallRadius);
	auto leftWallMatl = std::make_shared<MatteOpaque>();
	leftWallMatl->setAlbedo(0.9f, 0.0f, 0.0f);
	//auto leftWallMatl = std::make_shared<AbradedOpaque>();
	//leftWallMatl->setRoughness(1.0f);
	//leftWallMatl->setF0(Vector3f(1.0f, 0.765557f, 0.336057f));// gold
	out_world->addModel(Model(leftWallBall, leftWallMatl));

	auto rightWallBall = std::make_shared<GSphere>(Vector3f(wallRadius + boxHalfSize, 0, 0), wallRadius);
	auto rightWallMatl = std::make_shared<MatteOpaque>();
	rightWallMatl->setAlbedo(0.0f, 0.0f, 0.9f);
	out_world->addModel(Model(rightWallBall, rightWallMatl));

	auto backWallBall = std::make_shared<GSphere>(Vector3f(0, 0, -wallRadius - boxHalfSize - 10.0f), wallRadius);
	auto backWallMatl = std::make_shared<MatteOpaque>();
	backWallMatl->setAlbedo(0.9f, 0.9f, 0.9f);
	out_world->addModel(Model(backWallBall, backWallMatl));

	auto groundWallBall = std::make_shared<GSphere>(Vector3f(0, -wallRadius - boxHalfSize, 0), wallRadius);
	auto groundWallMatl = std::make_shared<MatteOpaque>();
	groundWallMatl->setAlbedo(0.9f, 0.9f, 0.9f);
	out_world->addModel(Model(groundWallBall, groundWallMatl));

	auto topWallBall = std::make_shared<GSphere>(Vector3f(0, wallRadius + boxHalfSize, 0), wallRadius);
	auto topWallMatl = std::make_shared<LightMaterial>();
	//topWallMatl->setEmittedRadiance(1.5f, 1.5f, 1.5f);
	topWallMatl->setEmittedRadiance(1.0f, 1.0f, 1.0f);
	out_world->addModel(Model(topWallBall, topWallMatl));

	auto frontWallBall = std::make_shared<GSphere>(Vector3f(0, 0, wallRadius + boxHalfSize), wallRadius);
	auto frontWallMatl = std::make_shared<MatteOpaque>();
	frontWallMatl->setAlbedo(0.9f, 0.9f, 0.9f);
	out_world->addModel(Model(frontWallBall, frontWallMatl));

	// scene objects

	auto sphere1Geometry = std::make_shared<GSphere>(Vector3f(-boxHalfSize + 0.25f, -boxHalfSize + 0.25f, -10.0f), 0.25f);
	auto sphere1Matl = std::make_shared<MatteOpaque>();
	sphere1Matl->setAlbedo(0.3f, 0.3f, 1.0f);
	out_world->addModel(Model(sphere1Geometry, sphere1Matl));

	auto sphere2Geometry = std::make_shared<GSphere>(Vector3f(-boxHalfSize + 1.5f, -boxHalfSize + 0.5f, -10.0f), 0.5f);
	auto sphere2Matl = std::make_shared<MatteOpaque>();
	sphere2Matl->setAlbedo(0.3f, 1.0f, 0.3f);
	out_world->addModel(Model(sphere2Geometry, sphere2Matl));

	auto sphere3Geometry = std::make_shared<GSphere>(Vector3f(-boxHalfSize + 4.0f, -boxHalfSize + 1.0f, -10.0f), 1.0f);
	auto sphere3Matl = std::make_shared<MatteOpaque>();
	sphere3Matl->setAlbedo(1.0f, 0.3f, 0.3f);
	out_world->addModel(Model(sphere3Geometry, sphere3Matl));

	auto sphere4Geometry = std::make_shared<GSphere>(Vector3f(boxHalfSize - 3.0f, -boxHalfSize + 3.0f, -boxHalfSize - 10.0f + 3.0f), 3.0f);
	//auto sphere4Matl = std::make_shared<MatteOpaque>();
	//sphere4Matl->setAlbedo(1.0f, 1.0f, 1.0f);
	auto sphere4Matl = std::make_shared<AbradedOpaque>();
	sphere4Matl->setRoughness(0.1f);
	//sphere4Matl->setF0(Vector3f(1.0f, 1.0f, 1.0f));
	out_world->addModel(Model(sphere4Geometry, sphere4Matl));

	auto sphere5Geometry = std::make_shared<GSphere>(Vector3f(boxHalfSize - 2.0f, -boxHalfSize + 0.8f, -8.5f), 0.8f);
	auto sphere5Matl = std::make_shared<MatteOpaque>();
	sphere5Matl->setAlbedo(1.0f, 1.0f, 1.0f);
	out_world->addModel(Model(sphere5Geometry, sphere5Matl));
}