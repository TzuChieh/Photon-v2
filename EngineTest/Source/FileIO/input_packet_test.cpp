#include <FileIO/SDL/InputPacket.h>
#include <FileIO/SDL/ValueClause.h>
#include <FileIO/SDL/NamedResourceStorage.h>

#include <gtest/gtest.h>

#include <vector>

// TODO: test more types
TEST(InputPacketTest, GetData)
{
	using namespace ph;

	NamedResourceStorage storage1;
	std::vector<ValueClause> clauses1{
		ValueClause(R"(string  s1 value1  )"),
		ValueClause(R"(string  s2 "value2")"),
		ValueClause(R"(integer i1 123     )"),
		ValueClause(R"(real    r1 3.0     )")
	};

	InputPacket packet1(clauses1, &storage1, Path());

	EXPECT_STREQ(packet1.getString("s1").c_str(), "value1");
	EXPECT_STREQ(packet1.getString("s2").c_str(), "value2");
	EXPECT_EQ(packet1.getInteger("i1"), 123);
	EXPECT_EQ(packet1.getReal("r1"), 3.0_r);
}

// TODO: test more types
TEST(InputPacketTest, CheckDataExistence)
{
	using namespace ph;

	NamedResourceStorage storage1;
	std::vector<ValueClause> clauses1{
		ValueClause(R"(string  str1 aaaaa)"),
		ValueClause(R"(string  str2 "bbb")"),
		ValueClause(R"(integer x    -3   )"),
		ValueClause(R"(real    y    999.0)")
	};

	InputPacket packet1(clauses1, &storage1, Path());

	EXPECT_TRUE(packet1.hasString("str1"));
	EXPECT_TRUE(packet1.hasString("str2"));
	EXPECT_TRUE(packet1.hasInteger("x"));
	EXPECT_TRUE(packet1.hasReal("y"));

	EXPECT_FALSE(packet1.hasString("str3"));
	EXPECT_FALSE(packet1.hasReal("z"));
	EXPECT_FALSE(packet1.hasReal(""));
	EXPECT_FALSE(packet1.hasInteger("3"));
}