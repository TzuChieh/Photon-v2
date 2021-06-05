#include <DataIO/SDL/InputPacket.h>
#include <DataIO/SDL/ValueClause.h>
#include <DataIO/SDL/SceneDescription.h>

#include <gtest/gtest.h>

#include <vector>

using namespace ph;

// TODO: test more types
TEST(InputPacketTest, GetData)
{
	{
		SceneDescription scene;
		std::vector<ValueClause> clauses{
			ValueClause(R"(string  s1 value1  )"),
			ValueClause(R"(string  s2 "value2")"),
			ValueClause(R"(integer i1 123     )"),
			ValueClause(R"(real    r1 3.0     )")
		};

		InputPacket packet(clauses, &scene, Path());

		EXPECT_STREQ(packet.getString("s1").c_str(), "value1");
		EXPECT_STREQ(packet.getString("s2").c_str(), "value2");
		EXPECT_EQ(packet.getInteger("i1"), 123);
		EXPECT_EQ(packet.getReal("r1"), 3.0_r);
	}
}

// TODO: test more types
TEST(InputPacketTest, CheckDataExistence)
{
	{
		SceneDescription scene;
		std::vector<ValueClause> clauses{
			ValueClause(R"(string  str1 aaaaa)"),
			ValueClause(R"(string  str2 "bbb")"),
			ValueClause(R"(integer x    -3   )"),
			ValueClause(R"(real    y    999.0)")
		};

		InputPacket packet(clauses, &scene, Path());

		EXPECT_TRUE(packet.hasString("str1"));
		EXPECT_TRUE(packet.hasString("str2"));
		EXPECT_TRUE(packet.hasInteger("x"));
		EXPECT_TRUE(packet.hasReal("y"));

		EXPECT_FALSE(packet.hasString("str3"));
		EXPECT_FALSE(packet.hasReal("z"));
		EXPECT_FALSE(packet.hasReal(""));
		EXPECT_FALSE(packet.hasInteger("3"));
	}
}
