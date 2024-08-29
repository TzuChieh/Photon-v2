#pragma once

#include <cstddef>
#include <array>

namespace ph
{

/*! @brief A fixed size vector backed by an array.
The container inherits the properties of a fixed size array of type `T`. The container keeps all `N`
elements alive until its lifetime ends, even if elements are popped/removed from the vector. It is
guaranteed that no dynamic memory allocation takes place.
*/
template<typename T, std::size_t N>
class TArrayVector final
{
public:
	TArrayVector();

	/*! @brief Add an item to the back of the vector.
	The item originally at the target index will be overwritten.
	*/
	template<typename U>
	void pushBack(U&& item);

	/*! @brief Removes an item from the back of the vector.
	The item originally at the target index is still alive after this call.
	*/
	void popBack();

	T& front();
	const T& front() const;
	T& back();
	const T& back() const;
	std::size_t size() const;
	void clear();
	bool isEmpty() const;
	bool isFull() const;

	T* get(std::size_t index);
	const T* get(std::size_t index) const;

	T& operator [] (std::size_t index);
	const T& operator [] (std::size_t index) const;

	/*! @name Iterators

	Iterators for stored objects.
	*/
	///@{
	typename std::array<T, N>::iterator       begin() noexcept;
	typename std::array<T, N>::const_iterator begin() const noexcept;
	typename std::array<T, N>::iterator       end()   noexcept;
	typename std::array<T, N>::const_iterator end()   const noexcept;
	///@}

private:
	std::array<T, N> m_data;
	std::size_t      m_size;// TODO: size type based on N?
};

}// end namespace ph

#include "Utility/TArrayVector.ipp"
