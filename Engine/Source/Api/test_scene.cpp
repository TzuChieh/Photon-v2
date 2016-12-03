#include "Api/test_scene.h"
#include "Common/primitive_type.h"
#include "Model/Model.h"
#include "Model/Geometry/Geometry.h"
#include "Model/Material/Material.h"
#include "Model/Geometry/GSphere.h"
#include "Model/Material/MatteOpaque.h"
#include "Model/Material/AbradedOpaque.h"
#include "Model/Material/LightMaterial.h"
#include "Model/Material/PerfectMirror.h"
#include "World/World.h"
#include "Model/Geometry/GRectangle.h"
#include "Model/ModelLoader.h"
#include "Image/CheckerboardTexture.h"
#include "Model/TextureMapper/SphericalMapper.h"

#include <memory>

namespace ph
{

void loadTestScene(World* const out_world)
{
	loadCornellBox(out_world, 10.0f);

	ModelLoader modelLoader;
	//auto loadedModel = modelLoader.load("../SceneResource/cube.obj");
	Model loadedModel;
	bool isLoadingSuccess = modelLoader.load("../SceneResource/dragon.obj", &loadedModel);
	if(isLoadingSuccess)
	{
		loadedModel.translate(0, -5, 0);
		loadedModel.scale(4);
		loadedModel.rotate(Vector3f(0, 1, 0), 180);

		/*loadedModel.setTextureMapper(std::make_shared<SphericalMapper>());
		auto loadedModelMaterial = std::make_shared<MatteOpaque>();
		loadedModelMaterial->setAlbedo(std::make_shared<CheckerboardTexture>(16.0f, 8.0f, Vector3f(0, 0, 0), Vector3f(1, 1, 1)));
		loadedModel.setMaterial(loadedModelMaterial);*/

		auto dragonMaterial = std::make_shared<AbradedOpaque>();
		dragonMaterial->setRoughness(0.2f);
		loadedModel.setMaterial(dragonMaterial);

		out_world->addModel(loadedModel);
	}

	//auto sphereMaterial = std::make_shared<MatteOpaque>();
	//auto sphereGeometry = std::make_shared<GSphere>(3.0f);
	////sphereMaterial->setAlbedo(0.9f, 0.9f, 0.9f);
	//sphereMaterial->setAlbedo(std::make_shared<CheckerboardTexture>(128.0f, 64.0f, Vector3f(0, 0, 0), Vector3f(1, 1, 1)));
	//Model sphereModel(sphereGeometry, sphereMaterial);
	//sphereModel.setTextureMapper(std::make_shared<SphericalMapper>());
	//sphereModel.rotate(Vector3f(1, 1, 0).normalize(), 45);
	//sphereModel.translate(0, -2, 0);
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
	lightMatl->setEmittedRadiance(1.5f, 1.5f, 1.5f);
	Model lightModel(unitRectangleGeom, lightMatl);
	lightModel.rotate(Vector3f(1, 0, 0), 90);
	lightModel.scale(boxSize * 0.3f);
	lightModel.translate(0, halfBoxSize - halfBoxSize * 0.05f, 0);
	out_world->addModel(lightModel);

	auto leftWallMatl = std::make_shared<MatteOpaque>();
	leftWallMatl->setAlbedo(0.9f, 0.0f, 0.0f);
	Model leftWallModel(unitRectangleGeom, leftWallMatl);
	leftWallModel.rotate(Vector3f(0, 1, 0), 90);
	leftWallModel.scale(boxSize);
	leftWallModel.translate(-halfBoxSize, 0, 0);
	out_world->addModel(leftWallModel);

	auto rightWallMatl = std::make_shared<MatteOpaque>();
	rightWallMatl->setAlbedo(0.0f, 0.0f, 0.9f);
	Model rightWallModel(unitRectangleGeom, rightWallMatl);
	rightWallModel.rotate(Vector3f(0, 1, 0), -90);
	rightWallModel.scale(boxSize);
	rightWallModel.translate(halfBoxSize, 0, 0);
	out_world->addModel(rightWallModel);

	auto backWallMatl = std::make_shared<MatteOpaque>();
	backWallMatl->setAlbedo(0.9f, 0.9f, 0.9f);
	Model backWallModel(unitRectangleGeom, backWallMatl);
	backWallModel.scale(boxSize);
	backWallModel.translate(0, 0, -halfBoxSize);
	out_world->addModel(backWallModel);

	auto topWallMatl = std::make_shared<MatteOpaque>();
	backWallMatl->setAlbedo(0.9f, 0.9f, 0.9f);
	Model topWallModel(unitRectangleGeom, topWallMatl);
	topWallModel.rotate(Vector3f(1, 0, 0), 90);
	topWallModel.scale(boxSize);
	topWallModel.translate(0, halfBoxSize, 0);
	out_world->addModel(topWallModel);

	auto groundWallMatl = std::make_shared<MatteOpaque>();
	groundWallMatl->setAlbedo(0.9f, 0.9f, 0.9f);
	Model groundWallModel(unitRectangleGeom, groundWallMatl);
	groundWallModel.rotate(Vector3f(1, 0, 0), -90);
	groundWallModel.scale(boxSize);
	groundWallModel.translate(0, -halfBoxSize, 0);
	out_world->addModel(groundWallModel);
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
	Model sphere1Model(sphere1Geometry, sphere1Matl);
	sphere1Model.translate(-boxHalfSize + 0.25f, -boxHalfSize + 0.25f, -10.0f);
	out_world->addModel(sphere1Model);

	auto sphere2Geometry = std::make_shared<GSphere>(0.5f);
	auto sphere2Matl = std::make_shared<MatteOpaque>();
	sphere2Matl->setAlbedo(0.3f, 1.0f, 0.3f);
	Model sphere2Model(sphere2Geometry, sphere2Matl);
	sphere2Model.translate(-boxHalfSize + 1.5f, -boxHalfSize + 0.5f, -10.0f);
	out_world->addModel(sphere2Model);

	auto sphere3Geometry = std::make_shared<GSphere>(1.0f);
	auto sphere3Matl = std::make_shared<MatteOpaque>();
	sphere3Matl->setAlbedo(1.0f, 0.3f, 0.3f);
	Model sphere3Model(sphere3Geometry, sphere3Matl);
	sphere3Model.translate(-boxHalfSize + 4.0f, -boxHalfSize + 1.0f, -10.0f);
	out_world->addModel(sphere3Model);

	auto sphere4Geometry = std::make_shared<GSphere>(3.0f);
	//auto sphere4Matl = std::make_shared<MatteOpaque>();
	//sphere4Matl->setAlbedo(1.0f, 1.0f, 1.0f);
	auto sphere4Matl = std::make_shared<AbradedOpaque>();
	sphere4Matl->setRoughness(0.1f);
	//sphere4Matl->setF0(Vector3f(1.0f, 1.0f, 1.0f));
	Model sphere4Model(sphere4Geometry, sphere4Matl);
	sphere4Model.translate(boxHalfSize - 3.0f, -boxHalfSize + 3.0f, -boxHalfSize - 10.0f + 3.0f);
	out_world->addModel(sphere4Model);

	auto sphere5Geometry = std::make_shared<GSphere>(0.8f);
	auto sphere5Matl = std::make_shared<MatteOpaque>();
	sphere5Matl->setAlbedo(1.0f, 1.0f, 1.0f);
	Model sphere5Model(sphere5Geometry, sphere5Matl);
	sphere5Model.translate(boxHalfSize - 2.0f, -boxHalfSize + 0.8f, -8.5f);
	out_world->addModel(sphere5Model);

	/*auto lightGeometry = std::make_shared<GSphere>(0.2f);
	auto lightMatl = std::make_shared<LightMaterial>();
	lightMatl->setEmittedRadiance(1600000, 2000000, 1000000);
	Model lightModel(lightGeometry, lightMatl);
	lightModel.translate(-2.5f, 2.0f, -5.0f);
	out_world->addModel(lightModel);*/
}

}// end namespace ph