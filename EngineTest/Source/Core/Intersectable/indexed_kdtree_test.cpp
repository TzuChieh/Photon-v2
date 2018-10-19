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

	const auto leafNode1 = TIndexedKdtreeNode<Index>::makeLeaf(itemIndices.data(), itemIndices.size(), indexBuffer);
	EXPECT_TRUE(leafNode1.isLeaf());
	EXPECT_EQ(leafNode1.indexBufferOffset(), 0);
	EXPECT_EQ(leafNode1.numItems(), 3);

	EXPECT_EQ(indexBuffer.size(), 3);

	const auto leafNode2 = TIndexedKdtreeNode<Index>::makeLeaf(itemIndices.data() + 2, 1, indexBuffer);
	EXPECT_TRUE(leafNode2.isLeaf());
	EXPECT_EQ(leafNode2.singleItemDirectIndex(), 777);
	EXPECT_EQ(leafNode2.numItems(), 1);

	EXPECT_EQ(indexBuffer.size(), 3);

	const auto innerNode = TIndexedKdtreeNode<Index>::makeInner(-1, math::Y_AXIS, 999);
	EXPECT_FALSE(innerNode.isLeaf());
	EXPECT_EQ(innerNode.positiveChildIndex(), 999);
	EXPECT_EQ(innerNode.splitAxisIndex(), math::Y_AXIS);
	EXPECT_EQ(innerNode.splitPos(), -1);

	EXPECT_EQ(indexBuffer.size(), 3);

	const auto leafNodeNoSingleItemOpt = TIndexedKdtreeNode<Index, false>::makeLeaf(itemIndices.data() + 1, 1, indexBuffer);
	EXPECT_TRUE(leafNodeNoSingleItemOpt.isLeaf());
	EXPECT_EQ(leafNodeNoSingleItemOpt.indexBufferOffset(), 3);
	EXPECT_EQ(leafNodeNoSingleItemOpt.numItems(), 1);

	EXPECT_EQ(indexBuffer.size(), 4);
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