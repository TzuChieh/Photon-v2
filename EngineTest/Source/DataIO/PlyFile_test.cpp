#include "config.h"

#include <DataIO/PlyFile.h>
#include <DataIO/FileSystem/Path.h>
#include <Common/primitive_type.h>

#include <gtest/gtest.h>

#include <cstring>

using namespace ph;

TEST(PlyFileTest, LoadSimpleAscii)
{
	// ASCII property-only
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
		EXPECT_FALSE(element->properties[0].isList());

		EXPECT_STREQ(element->properties[1].name.c_str(), "bb");
		EXPECT_EQ(element->properties[1].dataType, EPlyDataType::PPT_float32);
		EXPECT_FALSE(element->properties[1].isList());

		EXPECT_STREQ(element->properties[2].name.c_str(), "ccc");
		EXPECT_EQ(element->properties[2].dataType, EPlyDataType::PPT_float64);
		EXPECT_FALSE(element->properties[2].isList());
		
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

	// ASCII list-only
	{
		PlyIOConfig config;
		config.bIgnoreComments = false;

		PlyFile file(Path(PH_TEST_RESOURCE_PATH("PLY/ascii_list.ply")), config);

		EXPECT_EQ(file.getFormat(), EPlyFileFormat::ASCII);
		EXPECT_EQ(file.getVersion(), SemanticVersion(1, 0, 0));
		EXPECT_EQ(file.numComments(), 0);
		EXPECT_EQ(file.numElements(), 1);

		const PlyElement* const element = file.findElement("myList");
		ASSERT_TRUE(element);
		EXPECT_TRUE(element->isLoaded());
		EXPECT_STREQ(element->name.c_str(), "myList");
		EXPECT_EQ(element->numElements, 3);
		EXPECT_EQ(element->properties.size(), 1);

		EXPECT_STREQ(element->properties[0].name.c_str(), "myNumbers");
		EXPECT_EQ(element->properties[0].dataType, EPlyDataType::PPT_int32);
		EXPECT_TRUE(element->properties[0].isList());
		EXPECT_TRUE(element->properties[0].isFixedSizeList());
		EXPECT_EQ(element->properties[0].fixedListSize, 3);
		
		const std::byte* bytes = element->properties[0].rawListBuffer.data();
		EXPECT_EQ(element->properties[0].rawListBuffer.size(), 3 * (3 * 4));

		for(int32 i = 0; i <= 8; ++i)
		{
			int32 int32Value;
			std::memcpy(&int32Value, bytes, sizeof(int32Value));
			bytes += sizeof(int32Value);

			EXPECT_EQ(int32Value, i);
		}
	}
}

TEST(PlyFileTest, LoadSimpleAsciiWithHelperClasses)
{
	// ASCII property-only
	{
		PlyFile file(Path(PH_TEST_RESOURCE_PATH("PLY/ascii_property.ply")));

		PlyElement* const element = file.findElement("val");
		ASSERT_TRUE(element);

		auto aValues = element->propertyValues(element->findProperty("a"));
		ASSERT_TRUE(aValues);
		EXPECT_EQ(aValues.size(), 3);
		EXPECT_EQ(aValues.get(0), 0);
		EXPECT_EQ(aValues.get(1), 3);
		EXPECT_EQ(aValues.get(2), 6);

		auto bValues = element->propertyValues(element->findProperty("bb"));
		ASSERT_TRUE(bValues);
		EXPECT_EQ(bValues.size(), 3);
		EXPECT_EQ(bValues.get(0), 1);
		EXPECT_EQ(bValues.get(1), 4);
		EXPECT_EQ(bValues.get(2), 7);

		auto cValues = element->propertyValues(element->findProperty("ccc"));
		ASSERT_TRUE(cValues);
		EXPECT_EQ(cValues.size(), 3);
		EXPECT_EQ(cValues.get(0), 2);
		EXPECT_EQ(cValues.get(1), 5);
		EXPECT_EQ(cValues.get(2), 8);
	}

	// ASCII list-only
	{
		PlyFile file(Path(PH_TEST_RESOURCE_PATH("PLY/ascii_list.ply")));

		PlyElement* const element = file.findElement("myList");
		ASSERT_TRUE(element);

		auto listValues = element->listPropertyValues(element->findProperty("myNumbers"));
		ASSERT_TRUE(listValues);
		EXPECT_TRUE(listValues.isFixedSizeList());
		EXPECT_EQ(listValues.size(), 3);

		for(std::size_t li = 0; li < listValues.size(); ++li)
		{
			EXPECT_EQ(listValues.listSize(li), 3);
			EXPECT_EQ(listValues.fixedListSize(), 3);

			for(std::size_t i = 0; i < listValues.fixedListSize(); ++i)
			{
				EXPECT_EQ(listValues.get(li, i), li * 3 + i);
				EXPECT_EQ(listValues.get(li, i), li * 3 + i);
				EXPECT_EQ(listValues.get(li, i), li * 3 + i);
			}
		}
	}
}
