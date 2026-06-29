#include <Engine/Actor/Material/BinaryMixedSurfaceMaterial.h>
#include <Engine/Actor/Material/MatteOpaque.h>
#include <Engine/SDL/ISdlResource.h>
#include <Engine/SDL/SdlDependencyResolver.h>
#include <Engine/SDL/TSdl.h>

#include <gtest/gtest.h>

#include <vector>

using namespace ph;

TEST(SdlDependencyResolverTest, OrdersDependentMaterials)
{
	auto material0 = TSdl<MatteOpaque>::makeResource();
	auto material1 = TSdl<MatteOpaque>::makeResource();
	auto mixedMaterial = TSdl<BinaryMixedSurfaceMaterial>::makeResource();
	mixedMaterial->setMaterials(material0, material1);

	std::vector<const ISdlResource*> inputResources = {
		material1.get(),
		mixedMaterial.get(),
		material0.get()};

	SdlDependencyResolver resolver;
	resolver.analyze(inputResources);

	std::vector<const ISdlResource*> orderedResources;
	while(const ISdlResource* resource = resolver.next())
	{
		orderedResources.push_back(resource);
	}

	ASSERT_EQ(orderedResources.size(), inputResources.size());
	
	// `mixedMaterial` should be cooked last
	EXPECT_EQ(orderedResources.back(), mixedMaterial.get());
}
