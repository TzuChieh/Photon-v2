#include "config.h"

#include <DataIO/PlyFile.h>
#include <DataIO/FileSystem/Path.h>
#include <Common/primitive_type.h>

#include <gtest/gtest.h>

#include <cstring>

using namespace ph;

TEST(PlyFileTest, LoadSimpleAscii)
{
	{
		PlyIOConfig config;
		config.bIgnoreComments = false;

		PlyFile file(Path(PH_TEST_RESOURCE_PATH("PLY/ascii_property.ply")), config);

		EXPECT_EQ(file.getFormat(), EPlyFileFormat::ASCII);
		EXPECT_EQ(file.getVersion(), SemanticVersion(1, 0, 0));

		EXPECT_EQ(file.numComments(), 2);
		EXPECT_TRUE(file.getComment(0) == "hello");
		EXPECT_TRUE(file.getComment(1) == "hello2");

		EXPECT_EQ(file.numElements(), 1);

		const PlyElement* const element = file.findElement("val");
		ASSERT_TRUE(element);
		EXPECT_TRUE(element->isLoaded());
		EXPECT_STREQ(element->name.c_str(), "val");
		EXPECT_EQ(element->numElements, 3);
		EXPECT_EQ(element->properties.size(), 3);

		EXPECT_STREQ(element->properties[0].name.c_str(), "a");
		EXPECT_EQ(element->properties[0].dataType, EPlyDataType::PPT_int32);
		EXPECT_EQ(element->properties[0].isList(), false);

		EXPECT_STREQ(element->properties[1].name.c_str(), "bb");
		EXPECT_EQ(element->properties[1].dataType, EPlyDataType::PPT_float32);
		EXPECT_EQ(element->properties[1].isList(), false);

		EXPECT_STREQ(element->properties[2].name.c_str(), "ccc");
		EXPECT_EQ(element->properties[2].dataType, EPlyDataType::PPT_float64);
		EXPECT_EQ(element->properties[2].isList(), false);
		
		const std::byte* bytes = element->rawBuffer.data();
		EXPECT_EQ(element->rawBuffer.size(), 3 * (4 + 4 + 8));

		int32 int32Value;
		float32 float32Value;
		float64 float64Value;

		// 1st element
		std::memcpy(&int32Value, bytes, sizeof(int32Value)); bytes += sizeof(int32Value);
		EXPECT_EQ(int32Value, 0);
		std::memcpy(&float32Value, bytes, sizeof(float32Value)); bytes += sizeof(float32Value);
		EXPECT_FLOAT_EQ(float32Value, 1);
		std::memcpy(&float64Value, bytes, sizeof(float64Value)); bytes += sizeof(float64Value);
		EXPECT_DOUBLE_EQ(float64Value, 2);

		// 2nd element
		std::memcpy(&int32Value, bytes, sizeof(int32Value)); bytes += sizeof(int32Value);
		EXPECT_EQ(int32Value, 3);
		std::memcpy(&float32Value, bytes, sizeof(float32Value)); bytes += sizeof(float32Value);
		EXPECT_FLOAT_EQ(float32Value, 4);
		std::memcpy(&float64Value, bytes, sizeof(float64Value)); bytes += sizeof(float64Value);
		EXPECT_DOUBLE_EQ(float64Value, 5);

		// 3rd element
		std::memcpy(&int32Value, bytes, sizeof(int32Value)); bytes += sizeof(int32Value);
		EXPECT_EQ(int32Value, 6);
		std::memcpy(&float32Value, bytes, sizeof(float32Value)); bytes += sizeof(float32Value);
		EXPECT_FLOAT_EQ(float32Value, 7);
		std::memcpy(&float64Value, bytes, sizeof(float64Value)); bytes += sizeof(float64Value);
		EXPECT_DOUBLE_EQ(float64Value, 8);
	}
}
