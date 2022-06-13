#include <Utility/TSparseIntVector.h>
#include <Common/primitive_type.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(TSparseIntVectorTest, Creation)
{
	// Default creation
	{
		TSparseIntVector<int32> int32Vec;
		TSparseIntVector<int64> int64Vec;
		TSparseIntVector<uint32> uint32Vec;
		TSparseIntVector<uint64> uint64Vec;
	}

	// With initial capacity
	{
		TSparseIntVector<int32> int32Vec(22);
		TSparseIntVector<int64> int64Vec(44);
		TSparseIntVector<uint32> uint32Vec(66);
		TSparseIntVector<uint64> uint64Vec(88);
	}
}
