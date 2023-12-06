#include <Utility/TArrayAsStack.h>

#include <gtest/gtest.h>

using namespace ph;

TEST(TArrayAsStackTest, InitIntStackToZero)
{
	TArrayAsStack<int, 3> stack;
	EXPECT_TRUE(stack[0] == 0);
	EXPECT_TRUE(stack[1] == 0);
	EXPECT_TRUE(stack[2] == 0);
}

TEST(TArrayAsStackTest, InitPtrStackToNullptr)
{
	TArrayAsStack<int*, 3> stack;
	EXPECT_TRUE(stack[0] == nullptr);
	EXPECT_TRUE(stack[1] == nullptr);
	EXPECT_TRUE(stack[2] == nullptr);
}

TEST(TArrayAsStackTest, PushAndPop)
{
	TArrayAsStack<int, 3> stack;
	EXPECT_TRUE(stack.height() == 0);

	stack.push(-1);
	// stack state: [-1, X, X]
	EXPECT_TRUE(stack.top() == -1);
	EXPECT_TRUE(stack.height() == 1);

	stack.push(0);
	// stack state: [-1, 0, X]
	EXPECT_TRUE(stack.top() == 0);
	EXPECT_TRUE(stack.height() == 2);

	stack.pop();
	// stack state: [-1, X, X]
	EXPECT_TRUE(stack.top() == -1);
	EXPECT_TRUE(stack.height() == 1);

	stack.push(1);
	// stack state: [-1, 1, X]
	EXPECT_TRUE(stack.top() == 1);
	EXPECT_TRUE(stack.height() == 2);

	stack.push(2);
	// stack state: [-1, 1, 2]
	EXPECT_TRUE(stack.top() == 2);
	EXPECT_TRUE(stack.height() == 3);

	stack.pop();
	// stack state: [-1, 1, X]
	EXPECT_TRUE(stack.top() == 1);
	EXPECT_TRUE(stack.height() == 2);

	stack.pop();
	// stack state: [-1, X, X]
	EXPECT_TRUE(stack.top() == -1);
	EXPECT_TRUE(stack.height() == 1);

	stack.pop();
	// stack state: [X, X, X]
	EXPECT_TRUE(stack.height() == 0);
}
