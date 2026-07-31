#include <Engine/Actor/Material/BinaryMixedSurfaceMaterial.h>
#include <Engine/Actor/Material/MatteOpaque.h>
#include <Engine/SDL/ISdlResource.h>
#include <Engine/SDL/SdlDependencyResolver.h>
#include <Engine/SDL/TSdl.h>

#include <gtest/gtest.h>

#include <cstddef>
#include <vector>

using namespace ph;

TEST(SdlDependencyResolverTest, OrdersDependentMaterials)
{
	auto material0 = TSdl<MatteOpaque>::makeResource();
	auto material1 = TSdl<MatteOpaque>::makeResource();
	auto mixedMaterial = TSdl<BinaryMixedSurfaceMaterial>::makeResource();
	mixedMaterial->setMaterials(material0, material1);

	const std::vector<const ISdlResource*> resources = {
		mixedMaterial.get(),
		material0.get(),
		material1.get()};

	SdlDependencyResolver resolver;
	resolver.analyze(resources);

	// Either one could be first in DAG, but here they should obey input order
	EXPECT_EQ(resolver.next(), material0.get());
	EXPECT_EQ(resolver.next(), material1.get());
	// Both referenced materials must be returned before `mixedMaterial`
	EXPECT_EQ(resolver.next(), mixedMaterial.get());
	EXPECT_EQ(resolver.next(), nullptr);
}

TEST(SdlDependencyResolverTest, DependenciesTakePrecedenceOverPriority)
{
	auto material0 = TSdl<MatteOpaque>::makeResource();
	auto material1 = TSdl<MatteOpaque>::makeResource();
	auto mixedMaterial = TSdl<BinaryMixedSurfaceMaterial>::makeResource();
	mixedMaterial->setMaterials(material0, material1);

	const std::vector<const ISdlResource*> resources = {
		material0.get(),
		material1.get(),
		mixedMaterial.get()};
	// `mixedMaterial` has the highest priority but depends on the other two materials
	const std::vector<std::size_t> priorities = {2, 1, 0};

	SdlDependencyResolver resolver;
	resolver.analyze(resources, {.resourcePriorities = priorities});

	EXPECT_EQ(resolver.next(), material1.get());
	EXPECT_EQ(resolver.next(), material0.get());
	EXPECT_EQ(resolver.next(), mixedMaterial.get());
	EXPECT_EQ(resolver.next(), nullptr);
}

TEST(SdlDependencyResolverTest, UsesInputOrderForEqualPriorities)
{
	auto material0 = TSdl<MatteOpaque>::makeResource();
	auto material1 = TSdl<MatteOpaque>::makeResource();

	const std::vector<const ISdlResource*> resources = {
		material1.get(),
		material0.get()};
	const std::vector<std::size_t> priorities = {1, 1};

	SdlDependencyResolver resolver;
	resolver.analyze(resources, {.resourcePriorities = priorities});

	EXPECT_EQ(resolver.next(), material1.get());
	EXPECT_EQ(resolver.next(), material0.get());
	EXPECT_EQ(resolver.next(), nullptr);
}
