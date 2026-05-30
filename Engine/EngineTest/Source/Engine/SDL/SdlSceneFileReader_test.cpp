#include "engine_test_util.h"
#include "Engine/SDL/Introspect/TSdlReferenceArray_test.h"

#include <Engine/DataIO/FileSystem/TResourcePath.h>
#include <Engine/SDL/Introspect/SdlClass.h>
#include <Engine/SDL/SceneDescription.h>
#include <Engine/SDL/SdlSceneFileReader.h>

#include <gtest/gtest.h>

#include <array>

using namespace ph;

namespace
{

SdlSceneFileReader make_import_test_reader(const Path& sceneFile)
{
	static const std::array<const SdlClass*, 1> classes = {
		TSdlReferenceArray_test::TestResource::getSdlClass()};

	return SdlSceneFileReader(
		TSpanView<const SdlClass*>(classes.data(), classes.size()),
		sceneFile,
		sceneFile.getParent());
}

}// end anonymous namespace

TEST(SdlSceneFileReaderTest, ImportExpandsInPlace)
{
	SceneDescription scene;
	auto reader = make_import_test_reader(
		EngineTestResource("SDL_parser_import/direct_main.p2"));

	reader.read(&scene);

	EXPECT_EQ(reader.numParseErrors(), 0);
	EXPECT_TRUE(scene.has("direct-main"));
	EXPECT_TRUE(scene.has("direct-fragment"));
}

TEST(SdlSceneFileReaderTest, NestedImportResolvesRelativeToSceneWorkingDirectory)
{
	SceneDescription scene;
	auto reader = make_import_test_reader(
		EngineTestResource("SDL_parser_import/nested_main.p2"));

	reader.read(&scene);

	EXPECT_EQ(reader.numParseErrors(), 0);
	EXPECT_TRUE(scene.has("nested-main"));
	EXPECT_TRUE(scene.has("nested-child"));
	EXPECT_TRUE(scene.has("nested-leaf"));
}

TEST(SdlSceneFileReaderTest, ImportPathCanContainSpaces)
{
	SceneDescription scene;
	auto reader = make_import_test_reader(
		EngineTestResource("SDL_parser_import/space_path_main.p2"));

	reader.read(&scene);

	EXPECT_EQ(reader.numParseErrors(), 0);
	EXPECT_TRUE(scene.has("space-path-main"));
	EXPECT_TRUE(scene.has("space-path-fragment"));
}

TEST(SdlSceneFileReaderTest, MissingImportReportsParseError)
{
	SceneDescription scene;
	auto reader = make_import_test_reader(
		EngineTestResource("SDL_parser_import/missing_main.p2"));

	reader.read(&scene);

	EXPECT_GT(reader.numParseErrors(), 0);
}

TEST(SdlSceneFileReaderTest, UnknownDirectiveReportsParseError)
{
	SceneDescription scene;
	auto reader = make_import_test_reader(
		EngineTestResource("SDL_parser_import/unknown_directive.p2"));

	reader.read(&scene);

	EXPECT_GT(reader.numParseErrors(), 0);
}
