#include "Api/test_scene.h"
#include "Common/primitive_type.h"
#include "Entity/Entity.h"
#include "Entity/Geometry/Geometry.h"
#include "Entity/Material/Material.h"
#include "Entity/Geometry/GSphere.h"
#include "Entity/Material/MatteOpaque.h"
#include "Entity/Material/AbradedOpaque.h"
#include "Entity/Material/AbradedTranslucent.h"
#include "Entity/Material/LightMaterial.h"
#include "Entity/Material/PerfectMirror.h"
#include "World/World.h"
#include "Entity/Geometry/GRectangle.h"
#include "Entity/ModelLoader.h"
#include "Image/CheckerboardTexture.h"
#include "Entity/TextureMapper/SphericalMapper.h"
#include "FileIO/RenderDataFileLoader.h"

#include <memory>

namespace ph
{

void loadTestScene(World* const out_world)
{
	RenderDataFileLoader dataFileLoader;
	dataFileLoader.load("./testDataFile.txt");


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
	//	glassMaterial->setRoughness(0.4f);

	//	auto goldMaterial = std::make_shared<AbradedOpaque>();
	//	goldMaterial->setF0(1.0f, 0.765557f, 0.336057f);
	//	goldMaterial->setRoughness(0.1f);

	//	cupsModel.setMaterial(glassMaterial);
	//	for(auto& model : cupsModel.getChildren())
	//	{
	//		model.setMaterial(glassMaterial);
	//	}
	//}
	//out_world->addEntity(cupsModel);

	loadCornellBox(out_world, 10.0f);

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

	ModelLoader modelLoader;
	//auto loadedModel = modelLoader.load("../SceneResource/cube.obj");
	Entity loadedModel;
	
	bool isLoadingSuccess = modelLoader.load("../SceneResource/dragon.obj", &loadedModel);
	//bool isLoadingSuccess = modelLoader.load("../SceneResource/lucy.obj", &loadedModel);
	//bool isLoadingSuccess = modelLoader.load("../SceneResource/GlassesAndCups.obj", &loadedModel);
	//bool isLoadingSuccess = modelLoader.load("../SceneResource/lalala/lalala.obj", &loadedModel);
	//bool isLoadingSuccess = false;
	if(isLoadingSuccess)
	{
		/*loadedModel.translate(-1.6f, -5, 0);
		loadedModel.scale(3.5f);
		loadedModel.rotate(Vector3f(0, 1, 0), 230);*/

		loadedModel.translate(0, -5, 0);
		loadedModel.scale(5.0f);
		loadedModel.rotate(Vector3f(0, 1, 0), 180);

		/*loadedModel.translate(0, -5, 0);
		loadedModel.scale(4.0f);*/
		//loadedModel.rotate(Vector3f(0, 1, 0), 180);

		/*loadedModel.translate(0, -5, 1.5f);
		loadedModel.scale(1.1f);
		loadedModel.rotate(Vector3f(0, 1, 0), 180);*/

		/*loadedModel.translate(-3, -4, 1.5f);
		loadedModel.scale(1.5f);
		loadedModel.rotate(Vector3f(0, 1, 0), -45);*/

		/*loadedModel.setTextureMapper(std::make_shared<SphericalMapper>());
		auto loadedModelMaterial = std::make_shared<MatteOpaque>();
		loadedModelMaterial->setAlbedo(std::make_shared<CheckerboardTexture>(16.0f, 8.0f, Vector3f(0, 0, 0), Vector3f(1, 1, 1)));
		loadedModel.setMaterial(loadedModelMaterial);*/

		auto goldMaterial = std::make_shared<AbradedOpaque>();
		goldMaterial->setF0(Vector3f(1.0f, 0.765557f, 0.336057f));
		goldMaterial->setRoughness(0.3f);

		auto glassMaterial = std::make_shared<AbradedTranslucent>();
		glassMaterial->setF0(0.04f, 0.04f, 0.04f);
		glassMaterial->setIOR(1.5f);
		glassMaterial->setRoughness(0.0f);

		/*auto dragonMaterial = std::make_shared<AbradedTranslucent>();
		dragonMaterial->setF0(0.04f, 0.04f, 0.04f);
		dragonMaterial->setRoughness(0.0f);
		dragonMaterial->setIOR(1.5f);*/

		//auto dragonMaterial = std::make_shared<PerfectMirror>();

		loadedModel.setMaterial(goldMaterial);
		for(auto& model : loadedModel.getChildren())
		{
			model.setMaterial(goldMaterial);
		}

		out_world->addEntity(loadedModel);
	}

	/*auto sphereMaterial = std::make_shared<AbradedTranslucent>();
	sphereMaterial->setF0(0.04f, 0.04f, 0.04f);
	sphereMaterial->setRoughness(0.0f);
	sphereMaterial->setIOR(1.5f);
	auto sphereGeometry = std::make_shared<GSphere>(1.5f);
	Entity sphereModel(sphereGeometry, sphereMaterial);
	sphereModel.rotate(Vector3f(1, 1, 0).normalize(), 45);
	sphereModel.translate(2, -3.5f, 0);
	out_world->addEntity(sphereModel);*/

	//auto sphereChildMaterial = std::make_shared<MatteOpaque>();
	//auto sphereChildGeometry = std::make_shared<GSphere>(1.0f);
	//sphereChildMaterial->setAlbedo(0.9f, 0.9f, 0.9f);
	//Model sphereChildModel(sphereChildGeometry, sphereChildMaterial);
	//sphereChildModel.translate(0, 2, 0);

	//auto sphereMaterial = std::make_shared<MatteOpaque>();
	//auto sphereGeometry = std::make_shared<GSphere>(1.0f);
	////sphereMaterial->setAlbedo(0.9f, 0.9f, 0.9f);
	//sphereMaterial->setAlbedo(std::make_shared<CheckerboardTexture>(128.0f, 64.0f, Vector3f(0, 0, 0), Vector3f(1, 1, 1)));
	//Model sphereModel(sphereGeometry, sphereMaterial);
	//sphereModel.setTextureMapper(std::make_shared<SphericalMapper>());
	////sphereModel.rotate(Vector3f(1, 1, 0).normalize(), 45);
	//sphereModel.translate(0, -4, 0);
	//sphereModel.addChild(sphereChildModel);
	//out_world->addModel(sphereModel);

	/*auto lightMaterial = std::make_shared<LightMaterial>();
	auto lightGeometry = std::make_shared<GTriangle>(Vector3f(40, 10, -40), Vector3f(0, 10, 40), Vector3f(-40, 10, -40));
	lightMaterial->setEmittedRadiance(1.0f, 1.0f, 1.0f);
	world.addModel(Model(lightGeometry, lightMaterial));

	auto recMaterial = std::make_shared<MatteOpaque>();
	auto recGeometry = std::make_shared<GRectangle>(1.0f, 1.0f);
	world.addModel(Model(recGeometry, recMaterial));*/

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
}

void loadCornellBox(World* const out_world, const float32 boxSize)
{
	const float32 halfBoxSize = boxSize * 0.5f;

	auto unitRectangleGeom = std::make_shared<GRectangle>(1.0f, 1.0f);

	auto lightMatl = std::make_shared<LightMaterial>();
	lightMatl->setEmittedRadiance(3.4f, 3.4f, 3.4f);
	//lightMatl->setEmittedRadiance(1.8f, 1.8f, 1.8f);
	//lightMatl->setEmittedRadiance(1.0f, 1.0f, 1.0f);
	Entity lightModel(unitRectangleGeom, lightMatl);
	lightModel.rotate(Vector3f(1, 0, 0), 90);
	lightModel.scale(boxSize * 0.3f);
	lightModel.translate(0, halfBoxSize - halfBoxSize * 0.05f, 0);
	out_world->addEntity(lightModel);

	auto leftWallMatl = std::make_shared<MatteOpaque>();
	leftWallMatl->setAlbedo(0.85f, 0.3f, 0.3f);
	Entity leftWallModel(unitRectangleGeom, leftWallMatl);
	leftWallModel.rotate(Vector3f(0, 1, 0), 90);
	leftWallModel.scale(boxSize);
	leftWallModel.translate(-halfBoxSize, 0, 0);
	out_world->addEntity(leftWallModel);

	auto rightWallMatl = std::make_shared<MatteOpaque>();
	rightWallMatl->setAlbedo(0.3f, 0.3f, 0.85f);
	Entity rightWallModel(unitRectangleGeom, rightWallMatl);
	rightWallModel.rotate(Vector3f(0, 1, 0), -90);
	rightWallModel.scale(boxSize);
	rightWallModel.translate(halfBoxSize, 0, 0);
	out_world->addEntity(rightWallModel);

	auto backWallMatl = std::make_shared<MatteOpaque>();
	backWallMatl->setAlbedo(0.7f, 0.7f, 0.7f);
	Entity backWallModel(unitRectangleGeom, backWallMatl);
	backWallModel.scale(boxSize);
	backWallModel.translate(0, 0, -halfBoxSize);
	out_world->addEntity(backWallModel);

	auto topWallMatl = std::make_shared<MatteOpaque>();
	backWallMatl->setAlbedo(0.7f, 0.7f, 0.7f);
	Entity topWallModel(unitRectangleGeom, topWallMatl);
	topWallModel.rotate(Vector3f(1, 0, 0), 90);
	topWallModel.scale(boxSize);
	topWallModel.translate(0, halfBoxSize, 0);
	out_world->addEntity(topWallModel);

	auto groundWallMatl = std::make_shared<MatteOpaque>();
	groundWallMatl->setAlbedo(0.7f, 0.7f, 0.7f);
	Entity groundWallModel(unitRectangleGeom, groundWallMatl);
	groundWallModel.rotate(Vector3f(1, 0, 0), -90);
	groundWallModel.scale(boxSize);
	groundWallModel.translate(0, -halfBoxSize, 0);
	out_world->addEntity(groundWallModel);
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
	Entity sphere1Model(sphere1Geometry, sphere1Matl);
	sphere1Model.translate(-boxHalfSize + 0.25f, -boxHalfSize + 0.25f, -10.0f);
	out_world->addEntity(sphere1Model);

	auto sphere2Geometry = std::make_shared<GSphere>(0.5f);
	auto sphere2Matl = std::make_shared<MatteOpaque>();
	sphere2Matl->setAlbedo(0.3f, 1.0f, 0.3f);
	Entity sphere2Model(sphere2Geometry, sphere2Matl);
	sphere2Model.translate(-boxHalfSize + 1.5f, -boxHalfSize + 0.5f, -10.0f);
	out_world->addEntity(sphere2Model);

	auto sphere3Geometry = std::make_shared<GSphere>(1.0f);
	auto sphere3Matl = std::make_shared<MatteOpaque>();
	sphere3Matl->setAlbedo(1.0f, 0.3f, 0.3f);
	Entity sphere3Model(sphere3Geometry, sphere3Matl);
	sphere3Model.translate(-boxHalfSize + 4.0f, -boxHalfSize + 1.0f, -10.0f);
	out_world->addEntity(sphere3Model);

	auto sphere4Geometry = std::make_shared<GSphere>(3.0f);
	//auto sphere4Matl = std::make_shared<MatteOpaque>();
	//sphere4Matl->setAlbedo(1.0f, 1.0f, 1.0f);
	auto sphere4Matl = std::make_shared<AbradedOpaque>();
	sphere4Matl->setRoughness(0.1f);
	//sphere4Matl->setF0(Vector3f(1.0f, 1.0f, 1.0f));
	Entity sphere4Model(sphere4Geometry, sphere4Matl);
	sphere4Model.translate(boxHalfSize - 3.0f, -boxHalfSize + 3.0f, -boxHalfSize - 10.0f + 3.0f);
	out_world->addEntity(sphere4Model);

	auto sphere5Geometry = std::make_shared<GSphere>(0.8f);
	auto sphere5Matl = std::make_shared<MatteOpaque>();
	sphere5Matl->setAlbedo(1.0f, 1.0f, 1.0f);
	Entity sphere5Model(sphere5Geometry, sphere5Matl);
	sphere5Model.translate(boxHalfSize - 2.0f, -boxHalfSize + 0.8f, -8.5f);
	out_world->addEntity(sphere5Model);

	/*auto lightGeometry = std::make_shared<GSphere>(0.2f);
	auto lightMatl = std::make_shared<LightMaterial>();
	lightMatl->setEmittedRadiance(1600000, 2000000, 1000000);
	Model lightModel(lightGeometry, lightMatl);
	lightModel.translate(-2.5f, 2.0f, -5.0f);
	out_world->addModel(lightModel);*/
}

}// end namespace ph