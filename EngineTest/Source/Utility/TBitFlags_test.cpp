#include <Utility/TBitFlags.h>

#include <gtest/gtest.h>

#include <cstdint>

typedef ph::TBitFlags<int>          IntFlags;
typedef ph::TBitFlags<unsigned int> UintFlags;

enum class ETest : uint32_t
{
	A = uint32_t(1) << 0,
	B = uint32_t(1) << 1,
	C = uint32_t(1) << 20,
	D = uint32_t(1) << 31
};

typedef ph::TBitFlags<int32_t, ETest> EnumFlags;

TEST(TBitFlagsTest, Constructs)
{
	IntFlags flags1;
	EXPECT_TRUE(flags1.isEmpty());

	UintFlags flags2;
	EXPECT_TRUE(flags2.isEmpty());

	EnumFlags flags3;
	EXPECT_TRUE(flags3.isEmpty());
}

TEST(TBitFlagsTest, Union)
{
	IntFlags flags1a, flags1b;
	flags1a.unionWith({int(1) << 1, int(1) << 3, int(1) << 5});
	flags1b.unionWith({int(1) << 1});
	flags1b.unionWith({int(1) << 3});
	flags1b.unionWith({int(1) << 5});

	EXPECT_TRUE(flags1b.equals(flags1b));

	UintFlags flags2a, flags2b;
	flags2a.unionWith({static_cast<unsigned int>(1) << 2, static_cast<unsigned int>(1) << 2});
	flags2b.unionWith({static_cast<unsigned int>(1) << 2});

	EXPECT_TRUE(flags2a.equals(flags2b));

	EnumFlags flags3a, flags3b;
	flags3a.unionWith({ETest::A, ETest::C, ETest::A, ETest::A});
	flags3b.unionWith({ETest::C});
	flags3b.unionWith({ETest::A});

	EXPECT_TRUE(flags3a.equals(flags3b));
	flags3b.unionWith({ETest::D});
	EXPECT_FALSE(flags3a.equals(flags3b));
}

TEST(TBitFlagsTest, Intersect)
{
	EnumFlags flags1a, flags1b, flags1c;
	flags1a.set({ETest::A, ETest::C, ETest::B, ETest::B});
	flags1a.intersectWith({ETest::C});
	flags1b.set({ETest::C});
	flags1c.set({ETest::A});

	EXPECT_TRUE(flags1a.equals(flags1b));
	EXPECT_FALSE(flags1a.equals(flags1c));
}

TEST(TBitFlagsTest, HasAll)
{
	EnumFlags flags1;
	flags1.set({ETest::A, ETest::D, ETest::B});

	EXPECT_TRUE(flags1.hasAll({ETest::A}));
	EXPECT_TRUE(flags1.hasAll({ETest::B}));
	EXPECT_TRUE(flags1.hasAll({ETest::A, ETest::D}));
	EXPECT_TRUE(flags1.hasAll({ETest::A, ETest::B, ETest::D}));
	EXPECT_FALSE(flags1.hasAll({ETest::C}));
}

TEST(TBitFlagsTest, HasExactly)
{
	EnumFlags flags1;
	flags1.set({ETest::A});

	EXPECT_TRUE(flags1.hasExactly({ETest::A}));
	EXPECT_FALSE(flags1.hasExactly({ETest::C}));
	EXPECT_FALSE(flags1.hasExactly({ETest::A, ETest::B}));
}

TEST(TBitFlagsTest, HasAtLeastOne)
{
	EnumFlags flags1;
	flags1.set({ETest::A, ETest::B});

	EXPECT_TRUE(flags1.hasAtLeastOne({ETest::A}));
	EXPECT_TRUE(flags1.hasAtLeastOne({ETest::A, ETest::D, ETest::C}));
	EXPECT_FALSE(flags1.hasAtLeastOne({ETest::C}));
}

TEST(TBitFlagsTest, HasNone)
{
	EnumFlags flags1;
	flags1.set({ETest::A, ETest::B, ETest::C});

	EXPECT_TRUE(flags1.hasNone({ETest::D}));
	EXPECT_FALSE(flags1.hasNone({ETest::A}));
	EXPECT_FALSE(flags1.hasNone({ETest::B}));
	EXPECT_FALSE(flags1.hasNone({ETest::C, ETest::D}));
}