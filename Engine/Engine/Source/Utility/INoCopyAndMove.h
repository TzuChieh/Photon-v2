#pragma once

namespace ph
{

/*! @brief Marks the derived class as non-copyable and non-movable.

Deriving from this class helps to prevent accidentally copying or moving
the instance. This class should never be used as a base type that points 
to derived class instances.

Note while defining copy ctor and copy assignment operator as deleted will
also make the class non-movable, we explicitly mark move operations as deleted
so the intent is obvious. See IMoveOnly for more details.
*/
class INoCopyAndMove
{
protected:
	inline INoCopyAndMove() = default;
	inline ~INoCopyAndMove() = default;

	INoCopyAndMove(INoCopyAndMove&& other) = delete;
	INoCopyAndMove& operator = (INoCopyAndMove&& rhs) = delete;

	INoCopyAndMove(const INoCopyAndMove& other) = delete;
	INoCopyAndMove& operator = (const INoCopyAndMove& rhs) = delete;
};

}// end namespace ph
