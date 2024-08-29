#pragma once

#include <cstddef>
#include <array>
#include <functional>
#include <concepts>

namespace ph
{

/*! @brief A fixed size heap backed by an array.
The container inherits the properties of a fixed size array of type `T`. The container keeps all `N`
elements alive until its lifetime ends, even if elements are popped/removed from the stack. It is
guaranteed that no dynamic memory allocation takes place.
@tparam T Type of the array element.
@tparam N Maximum size of the backing array.
@tparam IsLess Comparator for the array element. The default comparator makes this heap a max heap.
*/
template<typename T, std::size_t N, typename IsLess = std::less<T>>
class TArrayHeap final
{
public:
	/*! Creates empty heap.
	*/
	TArrayHeap() requires std::default_initializable<IsLess>;

	/*! Creates empty heap with user-provided comparator.
	*/
	explicit TArrayHeap(IsLess isLess);

	/*! @brief Adds an item to the heap.
	The item originally at the target index will be overwritten.
	*/
	template<typename U>
	void push(U&& item);

	/*! @brief Removes the top item from the heap.
	The item originally at the target index is still alive after this call.
	*/
	void pop();

	/*! @brief Access the top item of the heap.
	By default, the top item is the maximum item (max heap). If the comparator is reversed, e.g.,
	comparing using `operator >`, then the top item is the minimum item (min heap).
	*/
	///@{
	T& top();
	const T& top() const;
	///@}

	std::size_t size() const;
	void clear();
	bool isEmpty() const;

private:
	// Must be signed type
	using Index = int;

	std::array<T, N> m_data;
	Index            m_currentIndex;

	[[no_unique_address]] IsLess m_isLess;
};

}// end namespace ph

#include "Utility/TArrayHeap.ipp"
