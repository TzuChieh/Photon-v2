#include <Engine/Math/Algorithm/IndexedKdtree/TIndexedKdtree.h>
#include <Engine/Math/Algorithm/IndexedKdtree/TIndexedKdtreeNode.h>
#include <Engine/Math/math.h>

#include <gtest/gtest.h>

#include <optional>
#include <vector>

using namespace ph;
using namespace ph::math;

template<typename Index>
void indexed_node_test()
{
	std::vector<Index> itemIndices = {0U, 1U, 234U};
	std::vector<Index> indexBuffer;

	const auto leafNode1 = TIndexedKdtreeNode<Index>::makeLeaf(
		{itemIndices.data(), itemIndices.size()}, indexBuffer);
	EXPECT_TRUE(leafNode1.isLeaf());
	EXPECT_EQ(leafNode1.getIndexBufferOffset(), 0);
	EXPECT_EQ(leafNode1.numItems(), 3);

	EXPECT_EQ(indexBuffer.size(), 3);

	const auto leafNode2 = TIndexedKdtreeNode<Index>::makeLeaf(
		{itemIndices.data() + 2, 1}, indexBuffer);
	EXPECT_TRUE(leafNode2.isLeaf());
	EXPECT_EQ(leafNode2.getSingleItemDirectIndex(), 234);
	EXPECT_EQ(leafNode2.numItems(), 1);

	// With single item optimization, index buffer do not increase in size
	EXPECT_EQ(indexBuffer.size(), 3);

	const auto innerNode = TIndexedKdtreeNode<Index>::makeInner(-1, constant::Y_AXIS, 42);
	EXPECT_FALSE(innerNode.isLeaf());
	EXPECT_EQ(innerNode.getPositiveChildIndex(), 42);
	EXPECT_EQ(innerNode.getSplitAxis(), constant::Y_AXIS);
	EXPECT_EQ(innerNode.getSplitPos(), -1);

	// Creating an internal node do not change the size of index buffer
	EXPECT_EQ(indexBuffer.size(), 3);

	const auto leafNodeNoSingleItemOpt = TIndexedKdtreeNode<Index, false>::makeLeaf(
		{itemIndices.data() + 1, 1}, indexBuffer);
	EXPECT_TRUE(leafNodeNoSingleItemOpt.isLeaf());
	EXPECT_EQ(leafNodeNoSingleItemOpt.getIndexBufferOffset(), 3);
	EXPECT_EQ(leafNodeNoSingleItemOpt.numItems(), 1);

	EXPECT_EQ(indexBuffer.size(), 4);
}

TEST(IndexedKdtreeNodeTest, UnsignedIndexNode)
{
	indexed_node_test<unsigned char>();
	indexed_node_test<unsigned short int>();
	indexed_node_test<unsigned int>();
	indexed_node_test<unsigned long int>();
	indexed_node_test<unsigned long long int>();
}

TEST(TIndexedKdtreeTest, OcclusionTraversalStopsOnFirstHit)
{
	const auto indexToItem = [](const uint32 index)
	{
		return index;
	};
	const auto itemToAABB = [](const uint32)
	{
		return AABB3D(Vector3R(0), Vector3R(1));
	};

	IndexedKdtreeParams params;
	params.maxNodeItems = 2;

	const TIndexedKdtree<decltype(indexToItem), decltype(itemToAABB), uint32> tree(
		2, indexToItem, itemToAABB, params);
	const TLineSegment<real> segment(Vector3R(0.5_r, 0.5_r, -1), Vector3R(0, 0, 1), 0, 10);

	std::size_t numTestedItems = 0;
	const bool hasHit = tree.occlusionTraversal(
		segment,
		[&numTestedItems](
			const uint32,
			const TLineSegment<real>& segment)
		-> std::optional<real>
		{
			++numTestedItems;
			return std::make_optional(segment.getMinT());
		});

	EXPECT_TRUE(hasHit);
	EXPECT_EQ(numTestedItems, 1);
}
