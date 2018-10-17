#include <Core/Intersectable/IndexedKdtree/TIndexedKdtreeNode.h>
#include <Math/math.h>

#include <gtest/gtest.h>

#include <vector>

template<typename Index>
void indexed_node_test()
{
	using namespace ph;

	std::vector<Index> itemIndices = {0, 1, 777};
	std::vector<Index> indexBuffer;
	const auto leafNode = TIndexedKdtreeNode<Index>::makeLeaf(itemIndices.data(), itemIndices.size(), indexBuffer);

	EXPECT_TRUE(leafNode.isLeaf());
	EXPECT_EQ(leafNode.itemIndexOffset(), 0);
	EXPECT_EQ(leafNode.numItems(), 3);

	const auto innerNode = TIndexedKdtreeNode<Index>::makeInner(-1, math::Y_AXIS, 999);

	EXPECT_FALSE(innerNode.isLeaf());
	EXPECT_EQ(innerNode.positiveChildIndex(), 999);
	EXPECT_EQ(innerNode.splitAxisIndex(), math::Y_AXIS);
	EXPECT_EQ(innerNode.splitPos(), -1);
}

TEST(IndexedKdtreeNodeTest, SignedIndexNode)
{
	indexed_node_test<int>();
	indexed_node_test<long int>();
	indexed_node_test<long long int>();
	indexed_node_test<short int>();
}

TEST(IndexedKdtreeNodeTest, UnsignedIndexNode)
{
	indexed_node_test<unsigned int>();
	indexed_node_test<unsigned long int>();
	indexed_node_test<unsigned long long int>();
	indexed_node_test<unsigned short int>();
}