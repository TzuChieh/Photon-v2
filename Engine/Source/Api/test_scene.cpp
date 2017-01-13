#include "Api/test_scene.h"
#include "Common/primitive_type.h"
#include "Actor/AModel.h"
#include "Actor/Geometry/Geometry.h"
#include "Actor/Material/Material.h"
#include "Actor/Geometry/GSphere.h"
#include "Actor/Material/MatteOpaque.h"
#include "Actor/Material/AbradedOpaque.h"
#include "Actor/Material/AbradedTranslucent.h"
#include "World/World.h"
#include "Actor/Geometry/GRectangle.h"
#include "Actor/ModelLoader.h"
#include "Image/CheckerboardTexture.h"
#include "Actor/TextureMapper/SphericalMapper.h"
#include "FileIO/RenderDataFileLoader.h"
#include "Actor/LightSource/AreaSource.h"
#include "Actor/ALight.h"
#include "Actor/Geometry/GCuboid.h"

#include <memory>

namespace ph
{

void loadTestScene(World* const out_world)
{
	RenderDataFileLoader dataFileLoader;
	dataFileLoader.load("./testDataFile.txt");

	loadCbox3ObjScene(out_world);

	/*auto lightGeometry = std::make_shared<GSphere>(0.5f);
	auto lightMaterial = std::make_shared<LightMaterial>();
	auto lightData = std::make_shared<AreaLight>();
	lightMaterial->setEmittedRadiance(1.0f, 0.0f, 0.0f);
	Entity testLightEntity(lightGeometry, lightMaterial);
	testLightEntity.translate(0, 0, 0);
	testLightEntity.setLight(lightData);
	out_world->addEntity(testLightEntity);*/


	//ModelLoader modelLoader;
	//Entity sponzaModel;
	//if(modelLoader.load("../SceneResource/sponza/sponza.obj", &sponzaModel))
	//{
	//	sponzaModel.scale(0.02f);
	//	//sponzaModel.rotate(Vector3f(0, 1, 0), 90);
	//}
	//out_world->addEntity(sponzaModel);

	//auto lightGeometry = std::make_shared<GSphere>(4.0f);
	//auto lightMaterial = std::make_shared<LightMaterial>();
	//lightMaterial->setEmittedRadiance(15.0f, 15.5f, 21.5f);
	//Entity lightEntity(lightGeometry, lightMaterial);
	//lightEntity.translate(-20, 20, 0);
	//out_world->addEntity(lightEntity);


	//Entity cupsModel;
	//if(modelLoader.load("../SceneResource/GlassesAndCups_original.obj", &cupsModel))
	//{
	//	cupsModel.scale(10.0f);
	//	cupsModel.rotate(Vector3f(0, 1, 0), 180);

	//	auto glassMaterial = std::make_shared<AbradedTranslucent>();
	//	glassMaterial->setF0(0.04f, 0.04f, 0.04f);
	//	glassMaterial->setIOR(1.5f);
	//	glassMaterial->setRoughness(0.0f);

	//	auto goldMaterial = std::make_shared<AbradedOpaque>();
	//	goldMaterial->setF0(1.0f, 0.765557f, 0.336057f);
	//	goldMaterial->setRoughness(0.1f);

	//	cupsModel.setMaterial(goldMaterial);
	//	for(auto& model : cupsModel.getChildren())
	//	{
	//		model.setMaterial(goldMaterial);
	//	}
	//}
	//out_world->addEntity(cupsModel);

	//loadCornellBox(out_world, 10.0f);

	/*auto glassMaterial = std::make_shared<AbradedTranslucent>();
	glassMaterial->setF0(0.04f, 0.04f, 0.04f);
	glassMaterial->setRoughness(0.05f);
	glassMaterial->setIOR(1.5f);

	auto sphereGeometry = std::make_shared<GSphere>(2.5f);
	Entity sphereEntity(sphereGeometry, glassMaterial);
	sphereEntity.translate(0, -1, 0);
	out_world->addEntity(sphereEntity);*/


	/*auto groundGeometry = std::make_shared<GRectangle>(100.0f, 100.0f);
	auto groundMaterial = std::make_shared<MatteOpaque>();
	groundMaterial->setAlbedo(0.9f, 0.9f, 0.9f);
	Model groundModel(groundGeometry, groundMaterial);
	groundModel.rotate(Vector3f(1, 0, 0), -90);
	groundModel.translate(0, -5, 0);
	out_world->addEntity(groundModel);

	auto lightGeometry = std::make_shared<GRectangle>(10.0f, 10.0f);
	auto lightMaterial = std::make_shared<LightMaterial>();
	lightMaterial->setEmittedRadiance(1.5f, 1.5f, 1.5f);
	Model lightModel(lightGeometry, lightMaterial);
	lightModel.rotate(Vector3f(1, 0, 0), 90);
	lightModel.rotate(Vector3f(0, 0, 1), -45);
	lightModel.translate(10, 10, 0);
	out_world->addEntity(lightModel);*/

	//ModelLoader modelLoader;
	//auto loadedModel = modelLoader.load("../SceneResource/cube.obj");
	
	//std::vector<AModel> loadedModels1;
	//std::vector<AModel> loadedModels2;
	//bool isLoadingSuccess1 = modelLoader.load("../SceneResource/dragon.obj", &loadedModels1);
	//bool isLoadingSuccess2 = modelLoader.load("../SceneResource/lucy.obj", &loadedModels2);
	////bool isLoadingSuccess = modelLoader.load("../SceneResource/GlassesAndCups.obj", &loadedModel);
	////bool isLoadingSuccess = modelLoader.load("../SceneResource/lalala/lalala.obj", &loadedModel);
	////bool isLoadingSuccess = false;
	//if(isLoadingSuccess1 && isLoadingSuccess2)
	//{
	//	for(auto& model : loadedModels1)
	//	{
	//		model.translate(-2.0f, -5, 0);
	//		model.scale(3.5f);
	//		model.rotate(Vector3f(0, 1, 0), 240);
	//	}

	//	/*loadedModel.translate(0, -5, 0);
	//	loadedModel.scale(5.0f);
	//	loadedModel.rotate(Vector3f(0, 1, 0), 180);*/

	//	for(auto& model : loadedModels2)
	//	{
	//		model.translate(1.9f, -5, -0.2f);
	//		model.scale(4.0f);
	//	}

	//	/*loadedModel.translate(0, -5, 1.5f);
	//	loadedModel.scale(1.1f);
	//	loadedModel.rotate(Vector3f(0, 1, 0), 180);*/

	//	/*loadedModel.translate(-3, -4, 1.5f);
	//	loadedModel.scale(1.5f);
	//	loadedModel.rotate(Vector3f(0, 1, 0), -45);*/

	//	/*loadedModel.setTextureMapper(std::make_shared<SphericalMapper>());
	//	auto loadedModelMaterial = std::make_shared<MatteOpaque>();
	//	loadedModelMaterial->setAlbedo(std::make_shared<CheckerboardTexture>(16.0f, 8.0f, Vector3f(0, 0, 0), Vector3f(1, 1, 1)));
	//	loadedModel.setMaterial(loadedModelMaterial);*/
	//	
	//	auto goldMaterial = std::make_shared<AbradedOpaque>();
	//	goldMaterial->setF0(Vector3f(1.0f, 0.765557f, 0.336057f));
	//	goldMaterial->setRoughness(0.3f);

	//	auto silverMaterial = std::make_shared<AbradedOpaque>();
	//	silverMaterial->setF0(Vector3f(0.971519f, 0.959915f, 0.915324f));
	//	silverMaterial->setRoughness(0.1f);

	//	auto glassMaterial = std::make_shared<AbradedTranslucent>();
	//	glassMaterial->setF0(0.04f, 0.04f, 0.04f);
	//	glassMaterial->setIOR(1.5f);
	//	glassMaterial->setRoughness(0.0f);

	//	auto matteMaterial = std::make_shared<MatteOpaque>();
	//	matteMaterial->setAlbedo(0.6f, 0.6f, 0.6f);

	//	for(auto& model : loadedModels1)
	//	{
	//		model.setMaterial(goldMaterial);
	//	}

	//	for(auto& model : loadedModels2)
	//	{
	//		model.setMaterial(silverMaterial);
	//	}

	//	for(auto& model : loadedModels1)
	//	{
	//		out_world->addActor(std::make_unique<AModel>(model));
	//	}

	//	for(auto& model : loadedModels2)
	//	{
	//		out_world->addActor(std::make_unique<AModel>(model));
	//	}
	//}

	//auto sphereMaterial = std::make_shared<AbradedTranslucent>();
	//sphereMaterial->setF0(0.04f, 0.04f, 0.04f);
	//sphereMaterial->setRoughness(0.0f);
	//sphereMaterial->setIOR(1.5f);
	///*auto sphereMaterial = std::make_shared<MatteOpaque>();
	//sphereMaterial->setAlbedo(0.6f, 0.6f, 0.6f);*/

	//auto sphereGeometry = std::make_shared<GSphere>(1.8f);
	//AModel sphereModel(sphereGeometry, sphereMaterial);
	//sphereModel.rotate(Vector3f(1, 1, 0).normalize(), 45);
	//sphereModel.translate(0.5f, -3.2f, 3);
}

void loadCornellBox(World* const out_world, const float32 boxSize)
{
	const float32 halfBoxSize = boxSize * 0.5f;

	auto unitRectangleGeom = std::make_shared<GRectangle>(1.0f, 1.0f);

	auto chromiumMaterial = std::make_shared<AbradedOpaque>();
	chromiumMaterial->setF0(Vector3f(0.549585f, 0.556114f, 0.554256f));
	chromiumMaterial->setRoughness(0.3f);

	ALight topLight;
	AModel& lightModel = topLight.getModel();
	//lightModel.rotate(Vector3f(1, 0, 0), 90);
	//lightModel.scale(boxSize * 0.05f);
	//lightModel.scale(boxSize * 0.3f);
	//lightModel.scale(boxSize * 0.5f);
	lightModel.scale(boxSize * 0.9f);
	//lightModel.translate(0, halfBoxSize - halfBoxSize * 0.05f, 0);
	lightModel.translate(0, 0, -(halfBoxSize - halfBoxSize * 0.05f));

	auto topLightMaterial = std::make_shared<MatteOpaque>();
	topLightMaterial->setAlbedo(0.5f, 0.5f, 0.5f);
	lightModel.setGeometry(unitRectangleGeom);
	lightModel.setMaterial(topLightMaterial);

	//auto topLightSource = std::make_shared<AreaSource>(Vector3f(3.4f, 3.4f, 3.4f));
	//auto topLightSource = std::make_shared<AreaSource>("../SceneResource/image/debugTexture.png");
	//auto topLightSource = std::make_shared<AreaSource>("../SceneResource/image/bigPlatform.png");
	//auto topLightSource = std::make_shared<AreaSource>("../SceneResource/image/mountains.jpg");
	//auto topLightSource = std::make_shared<AreaSource>("../SceneResource/image/vm.png");
	//auto topLightSource = std::make_shared<AreaSource>("../SceneResource/image/human.jpg");
	auto topLightSource = std::make_shared<AreaSource>("../SceneResource/image/scenery.jpg");
	//auto topLightSource = std::make_shared<AreaSource>(Vector3f(34, 34, 34));
	//auto topLightSource = std::make_shared<AreaSource>(Vector3f(1, 1, 1));
	//auto topLightSource = std::make_shared<AreaSource>(Vector3f(0.4f, 0.4f, 0.4f));
	//auto topLightSource = std::make_shared<AreaSource>(Vector3f(0.2f, 0.2f, 0.2f));
	topLight.setLightSource(topLightSource);
	out_world->addActor(std::make_unique<ALight>(topLight));

	//auto lightMatl = std::make_shared<LightMaterial>();
	//lightMatl->setEmittedRadiance(3.4f, 3.4f, 3.4f);
	////lightMatl->setEmittedRadiance(1.8f, 1.8f, 1.8f);
	////lightMatl->setEmittedRadiance(1.0f, 1.0f, 1.0f);
	//AModel lightModel(unitRectangleGeom, lightMatl);
	//lightModel.rotate(Vector3f(1, 0, 0), 90);
	//lightModel.scale(boxSize * 0.3f);
	//lightModel.translate(0, halfBoxSize - halfBoxSize * 0.05f, 0);
	//out_world->addActor(std::make_unique<AModel>(lightModel));

	auto leftWallMatl = std::make_shared<MatteOpaque>();
	leftWallMatl->setAlbedo(0.85f, 0.3f, 0.3f);
	AModel leftWallModel(unitRectangleGeom, leftWallMatl);
	leftWallModel.rotate(Vector3f(0, 1, 0), 90);
	leftWallModel.scale(boxSize);
	leftWallModel.translate(-halfBoxSize, 0, 0);
	out_world->addActor(std::make_unique<AModel>(leftWallModel));

	auto rightWallMatl = std::make_shared<MatteOpaque>();
	rightWallMatl->setAlbedo(0.3f, 0.3f, 0.85f);
	AModel rightWallModel(unitRectangleGeom, rightWallMatl);
	rightWallModel.rotate(Vector3f(0, 1, 0), -90);
	rightWallModel.scale(boxSize);
	rightWallModel.translate(halfBoxSize, 0, 0);
	out_world->addActor(std::make_unique<AModel>(rightWallModel));

	auto backWallMatl = std::make_shared<MatteOpaque>();
	backWallMatl->setAlbedo(0.7f, 0.7f, 0.7f);
	AModel backWallModel(unitRectangleGeom, backWallMatl);
	backWallModel.scale(boxSize);
	backWallModel.translate(0, 0, -halfBoxSize);
	out_world->addActor(std::make_unique<AModel>(backWallModel));

	auto topWallMatl = std::make_shared<MatteOpaque>();
	backWallMatl->setAlbedo(0.7f, 0.7f, 0.7f);
	AModel topWallModel(unitRectangleGeom, topWallMatl);
	topWallModel.rotate(Vector3f(1, 0, 0), 90);
	topWallModel.scale(boxSize);
	topWallModel.translate(0, halfBoxSize, 0);
	out_world->addActor(std::make_unique<AModel>(topWallModel));

	auto groundWallMatl = std::make_shared<MatteOpaque>();
	groundWallMatl->setAlbedo(0.7f, 0.7f, 0.7f);
	//AModel groundWallModel(unitRectangleGeom, groundWallMatl);
	AModel groundWallModel(unitRectangleGeom, chromiumMaterial);
	groundWallModel.rotate(Vector3f(1, 0, 0), -90);
	groundWallModel.scale(boxSize);
	groundWallModel.translate(0, -halfBoxSize, 0);
	out_world->addActor(std::make_unique<AModel>(groundWallModel));
}

void loadCbox3ObjScene(World* const out_world)
{
	loadCornellBox(out_world, 10.0f);

	auto goldMaterial = std::make_shared<AbradedOpaque>();
	goldMaterial->setF0(Vector3f(1.0f, 0.765557f, 0.336057f));
	goldMaterial->setRoughness(0.2f);
	//goldMaterial->setRoughness(0.001f);

	auto silverMaterial = std::make_shared<AbradedOpaque>();
	silverMaterial->setF0(Vector3f(0.971519f, 0.959915f, 0.915324f));
	silverMaterial->setRoughness(0.1f);

	auto glassMaterial = std::make_shared<AbradedTranslucent>();
	glassMaterial->setF0(0.04f, 0.04f, 0.04f);
	//glassMaterial->setF0(0.1836735f, 0.1836735f, 0.1836735f);
	glassMaterial->setIOR(1.5f);
	//glassMaterial->setIOR(2.5f);
	//glassMaterial->setIOR(1.0f);
	//glassMaterial->setIOR(1.0f);
	//glassMaterial->setRoughness(0.2f);
	glassMaterial->setRoughness(0.0f);

	auto matteMaterial = std::make_shared<MatteOpaque>();
	matteMaterial->setAlbedo(0.6f, 0.6f, 0.6f);

	/*ModelLoader modelLoader;

	std::vector<AModel> dragonModels;
	std::vector<AModel> lucyModels;
	if(modelLoader.load("../SceneResource/dragon.obj", &dragonModels) && modelLoader.load("../SceneResource/lucy.obj", &lucyModels))
	{
		for(auto& model : dragonModels)
		{
			model.translate(-2.0f, -5, 0);
			model.scale(3.5f);
			model.rotate(Vector3f(0, 1, 0), 240);

			model.setMaterial(goldMaterial);

			out_world->addActor(std::make_unique<AModel>(model));
		}

		for(auto& model : lucyModels)
		{
			model.translate(1.9f, -5, -0.2f);
			model.scale(4.0f);

			model.setMaterial(silverMaterial);

			out_world->addActor(std::make_unique<AModel>(model));
		}
	}*/

	/*auto sphereMaterial = std::make_shared<AbradedTranslucent>();
	sphereMaterial->setF0(0.04f, 0.04f, 0.04f);
	sphereMaterial->setRoughness(0.0f);
	sphereMaterial->setIOR(1.5f);*/
	/*auto sphereMaterial = std::make_shared<MatteOpaque>();
	sphereMaterial->setAlbedo(0.6f, 0.6f, 0.6f);*/

	/*auto sphereGeometry = std::make_shared<GSphere>(1.8f);
	AModel sphereModel(sphereGeometry, glassMaterial);
	sphereModel.rotate(Vector3f(1, 1, 0).normalize(), 45);
	sphereModel.translate(0.5f, -3.2f, 3);*/


	//auto sphereGeometry = std::make_shared<GSphere>(2.7f);
	//AModel sphereModel(sphereGeometry, glassMaterial);
	////AModel sphereModel(sphereGeometry, goldMaterial);
	//sphereModel.rotate(Vector3f(1, 1, 0).normalize(), 45);
	////sphereModel.translate(0.5f, -2.0f, 3);
	//sphereModel.translate(0.9f, -2.3f, 1.3f);

	//out_world->addActor(std::make_unique<AModel>(sphereModel));


	//auto cubeGeometry = std::make_shared<GCuboid>(4.5f, 4.5f, 4.5f);
	//AModel cubeModel(cubeGeometry, glassMaterial);
	//cubeModel.rotate(Vector3f(1, 1, 0).normalize(), 30);
	////cubeModel.translate(0, -1.2f, 0);
	//cubeModel.translate(0, -1.5f, 0);
	//out_world->addActor(std::make_unique<AModel>(cubeModel));

	ModelLoader modelLoader;

	std::vector<AModel> dragonModels;
	if(modelLoader.load("../SceneResource/dragon.obj", &dragonModels))
	{
		for(auto& model : dragonModels)
		{
			model.translate(0, -5, 0);
			model.scale(5.0f);
			model.rotate(Vector3f(0, 1, 0), 180);

			model.setMaterial(glassMaterial);

			out_world->addActor(std::make_unique<AModel>(model));
		}
	}
}

void load5bScene(World* const out_world)
{
	const float32 wallRadius = 1000.0f;
	const float32 boxHalfSize = 5.0f;

	//auto leftWallBall = std::make_shared<GSphere>(wallRadius);
	//auto leftWallMatl = std::make_shared<MatteOpaque>();
	//leftWallMatl->setAlbedo(0.9f, 0.0f, 0.0f);
	////auto leftWallMatl = std::make_shared<AbradedOpaque>();
	////leftWallMatl->setRoughness(1.0f);
	////leftWallMatl->setF0(Vector3f(1.0f, 0.765557f, 0.336057f));// gold
	//Model leftWallModel(leftWallBall, leftWallMatl);
	//leftWallModel.translate(-wallRadius - boxHalfSize, 0, 0);
	//out_world->addModel(leftWallModel);

	//auto rightWallBall = std::make_shared<GSphere>(wallRadius);
	//auto rightWallMatl = std::make_shared<MatteOpaque>();
	//rightWallMatl->setAlbedo(0.0f, 0.0f, 0.9f);
	//Model rightWallModel(rightWallBall, rightWallMatl);
	//rightWallModel.translate(wallRadius + boxHalfSize, 0, 0);
	//out_world->addModel(rightWallModel);

	//auto backWallBall = std::make_shared<GSphere>(wallRadius);
	//auto backWallMatl = std::make_shared<MatteOpaque>();
	//backWallMatl->setAlbedo(0.9f, 0.9f, 0.9f);
	//Model backWallModel(backWallBall, backWallMatl);
	//backWallModel.translate(0, 0, -wallRadius - boxHalfSize - 10.0f);
	//out_world->addModel(backWallModel);

	//auto groundWallBall = std::make_shared<GSphere>(wallRadius);
	//auto groundWallMatl = std::make_shared<MatteOpaque>();
	//groundWallMatl->setAlbedo(0.9f, 0.9f, 0.9f);
	//Model groundWallModel(groundWallBall, groundWallMatl);
	//groundWallModel.translate(0, -wallRadius - boxHalfSize, 0);
	//out_world->addModel(groundWallModel);

	//auto topWallBall = std::make_shared<GSphere>(wallRadius);
	//auto topWallMatl = std::make_shared<LightMaterial>();
	////auto topWallMatl = std::make_shared<MatteOpaque>();
	////topWallMatl->setEmittedRadiance(1.5f, 1.5f, 1.5f);
	//topWallMatl->setEmittedRadiance(1.0f, 1.0f, 1.0f);
	//Model topWallModel(topWallBall, topWallMatl);
	//topWallModel.translate(0, wallRadius + boxHalfSize, 0);
	//out_world->addModel(topWallModel);

	//auto frontWallBall = std::make_shared<GSphere>(wallRadius);
	//auto frontWallMatl = std::make_shared<MatteOpaque>();
	//frontWallMatl->setAlbedo(0.9f, 0.9f, 0.9f);
	//Model frontWallModel(frontWallBall, frontWallMatl);
	//frontWallModel.translate(0, 0, wallRadius + boxHalfSize);
	//out_world->addModel(frontWallModel);

	// scene objects

	auto sphere1Geometry = std::make_shared<GSphere>(0.25f);
	auto sphere1Matl = std::make_shared<MatteOpaque>();
	sphere1Matl->setAlbedo(0.3f, 0.3f, 1.0f);
	AModel sphere1Model(sphere1Geometry, sphere1Matl);
	sphere1Model.translate(-boxHalfSize + 0.25f, -boxHalfSize + 0.25f, -10.0f);
	out_world->addActor(std::make_unique<AModel>(sphere1Model));

	auto sphere2Geometry = std::make_shared<GSphere>(0.5f);
	auto sphere2Matl = std::make_shared<MatteOpaque>();
	sphere2Matl->setAlbedo(0.3f, 1.0f, 0.3f);
	AModel sphere2Model(sphere2Geometry, sphere2Matl);
	sphere2Model.translate(-boxHalfSize + 1.5f, -boxHalfSize + 0.5f, -10.0f);
	out_world->addActor(std::make_unique<AModel>(sphere2Model));

	auto sphere3Geometry = std::make_shared<GSphere>(1.0f);
	auto sphere3Matl = std::make_shared<MatteOpaque>();
	sphere3Matl->setAlbedo(1.0f, 0.3f, 0.3f);
	AModel sphere3Model(sphere3Geometry, sphere3Matl);
	sphere3Model.translate(-boxHalfSize + 4.0f, -boxHalfSize + 1.0f, -10.0f);
	out_world->addActor(std::make_unique<AModel>(sphere3Model));

	auto sphere4Geometry = std::make_shared<GSphere>(3.0f);
	//auto sphere4Matl = std::make_shared<MatteOpaque>();
	//sphere4Matl->setAlbedo(1.0f, 1.0f, 1.0f);
	auto sphere4Matl = std::make_shared<AbradedOpaque>();
	sphere4Matl->setRoughness(0.1f);
	//sphere4Matl->setF0(Vector3f(1.0f, 1.0f, 1.0f));
	AModel sphere4Model(sphere4Geometry, sphere4Matl);
	sphere4Model.translate(boxHalfSize - 3.0f, -boxHalfSize + 3.0f, -boxHalfSize - 10.0f + 3.0f);
	out_world->addActor(std::make_unique<AModel>(sphere4Model));

	auto sphere5Geometry = std::make_shared<GSphere>(0.8f);
	auto sphere5Matl = std::make_shared<MatteOpaque>();
	sphere5Matl->setAlbedo(1.0f, 1.0f, 1.0f);
	AModel sphere5Model(sphere5Geometry, sphere5Matl);
	sphere5Model.translate(boxHalfSize - 2.0f, -boxHalfSize + 0.8f, -8.5f);
	out_world->addActor(std::make_unique<AModel>(sphere5Model));

	/*auto lightGeometry = std::make_shared<GSphere>(0.2f);
	auto lightMatl = std::make_shared<LightMaterial>();
	lightMatl->setEmittedRadiance(1600000, 2000000, 1000000);
	Model lightModel(lightGeometry, lightMatl);
	lightModel.translate(-2.5f, 2.0f, -5.0f);
	out_world->addModel(lightModel);*/
}

}// end namespace ph