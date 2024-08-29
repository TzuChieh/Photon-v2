#pragma once

#include <cstddef>
#include <array>

namespace ph
{

/*! @brief A fixed size stack backed by an array.
The container inherits the properties of a fixed size array of type `T`. The container keeps all `N`
elements alive until its lifetime ends, even if elements are popped/removed from the stack. It is
guaranteed that no dynamic memory allocation takes place.
*/
template<typename T, std::size_t N>
class TArrayStack final
{
public:
	/*! Creates empty stack.
	*/
	TArrayStack();

	/*! @brief Adds an item to the stack.
	The item originally at the target index will be overwritten.
	*/
	template<typename U>
	void push(U&& item);

	/*! @brief Removes the top item from the stack.
	The item originally at the target index is still alive after this call.
	*/
	void pop();

	T& top();
	const T& top() const;
	std::size_t height() const;
	void clear();
	bool isEmpty() const;

	T& operator [] (std::size_t index);
	const T& operator [] (std::size_t index) const;

private:
	// Must be signed type
	using Index = int;

	std::array<T, N> m_data;
	Index            m_currentIndex;
};

}// end namespace ph

#include "Utility/TArrayStack.ipp"
