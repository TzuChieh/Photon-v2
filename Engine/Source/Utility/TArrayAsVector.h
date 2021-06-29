#pragma once

#include <cstddef>
#include <array>

namespace ph
{

// TODO: dtor call policy? currently no dtor call on element remove, rely on life-time of vector
// TODO: iterators

template<typename T, std::size_t N>
class TArrayAsVector final
{
public:
	TArrayAsVector();

	template<typename U>
	void pushBack(U&& item);

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
	typename std::array<T>::iterator       begin() noexcept;
	typename std::array<T>::const_iterator begin() const noexcept;
	typename std::array<T>::iterator       end()   noexcept;
	typename std::array<T>::const_iterator end()   const noexcept;
	///@}

private:
	std::array<T, N> m_data;
	std::size_t      m_size;// TODO: size type based on N?
};

}// end namespace ph

#include "Utility/TArrayAsVector.ipp"
