#pragma once

namespace ph
{

/*! @brief Marks the derived class as move only.

Deriving from this class helps to prevent accidentally copying the instance.
This class should never be used as a base type that points to derived class
instances.

Note that one might think a "noncopyable" type like boost::noncopyable will
also do the job; however, when an implementation only defines deleted copy
ctor and deleted copy assignment operator, it also prevents move operations.
This can be unexpected. We made this type for marking classes as move-only
by explicitly define move operations as default and define copy operations
as delete.

Reference:
https://stackoverflow.com/questions/35743586/noncopyable-and-nonmovable-together
*/
class IMoveOnly
{
protected:
	inline IMoveOnly() = default;
	inline ~IMoveOnly() = default;

	inline IMoveOnly(IMoveOnly&& other) = default;
	inline IMoveOnly& operator = (IMoveOnly&& rhs) = default;

	IMoveOnly(const IMoveOnly& other) = delete;
	IMoveOnly& operator = (const IMoveOnly& rhs) = delete;
};

}// end namespace ph
