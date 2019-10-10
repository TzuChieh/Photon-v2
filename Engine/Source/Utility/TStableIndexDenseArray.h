#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <vector>
#include <utility>
#include <iostream>

namespace ph
{

/*! @brief A container offering stable index and efficient iteration. 

Notes on this container:

- Does not preserve order, i.e., the order for iteration may not be the same as
  how you added the objects.

- Inserted objects are guaranteed being contiguous in virtual memory.

- User accessible indices are guaranteed to be stable, i.e., they are invariant
  to any modification to the container and can be used to retrieve the object
  previously added until removal.

- Add, remove and retrieve object are all O(1) operations.
*/
template<typename T>
class TStableIndexDenseArray
{
public:
	/*! @brief An index that indicates an invalid state.

	The value is the maximum value a std::size_t can hold. Due to current stable
	index dispatching implementation and physical memory limit, this value
	should be nearly impossible to reach.
	*/
	static const std::size_t INVALID_STABLE_INDEX = static_cast<std::size_t>(-1);

public:
	/*! @brief An array that holds nothing.
	*/
	TStableIndexDenseArray();

	/*! @brief An array that reserves memory space for @p initialCapacity objects.
	*/
	explicit TStableIndexDenseArray(std::size_t initialCapacity);

	/*! @brief Adds an object and returns a stable index for it.
	*/
	std::size_t add(const T& object);

	/*! @brief Similar to add(const T&), expect the object is moved.
	*/
	std::size_t add(T&& object);

	/*! @brief Remove an object by its stable index.
	@return `true` if and only if @p stableIndex is valid and object is
	correctly removed.
	*/
	bool remove(std::size_t stableIndex);

	/*! @brief Remove all objects.
	*/
	void removeAll();

	/*! @brief Returns how many objects are there in this container.
	*/
	std::size_t length() const;

	/*! @brief Get the next stable index that will be returned by add().

	This array should remain unchanged between this call and add(); otherwise
	the result is unreliable.
	*/
	std::size_t nextStableIndex() const;

	/*! @brief Checks whether the stable index represent a valid object or not.
	@return `true` if @stableIndex maps to an object, otherwise `false`.
	*/
	bool isStableIndexValid(std::size_t stableIndex) const;

	/*! @name Iterators

	Iterators for stored objects.
	*/
	///@{
	typename std::vector<T>::iterator       begin() noexcept;
	typename std::vector<T>::const_iterator begin() const noexcept;
	typename std::vector<T>::iterator       end()   noexcept;
	typename std::vector<T>::const_iterator end()   const noexcept;
	///@}

	/*! @name Direct Accessors

	Retrieve object without index validity check.
	*/
	///@{
	T& operator [] (std::size_t stableIndex);
	const T& operator [] (std::size_t stableIndex) const;
	///@}

	/*! @name Checked Accessors
	
	Retrieve object with index validity check.
	@return `nullptr` if @p stableIndex is invalid.
	*/
	///@{
	T* get(std::size_t stableIndex);
	const T* get(std::size_t stableIndex) const;
	///@}

private:
	std::vector<T>                            m_objects;
	std::vector<std::size_t>                  m_objectToIndexMap;
	std::vector<std::pair<std::size_t, bool>> m_indexToObjectMapValidityPairs;
	std::vector<std::size_t>                  m_freeIndices;

	std::size_t dispatchStableIndex();
};

// Implementations:

template<typename T>
const std::size_t TStableIndexDenseArray<T>::INVALID_STABLE_INDEX;

template<typename T>
TStableIndexDenseArray<T>::TStableIndexDenseArray() = default;

template<typename T>
TStableIndexDenseArray<T>::TStableIndexDenseArray(const std::size_t initialCapacity) : 
	TStableIndexDenseArray()
{
	m_objects.reserve(initialCapacity);
	m_objectToIndexMap.reserve(initialCapacity);
	m_indexToObjectMapValidityPairs.reserve(initialCapacity);
}

template<typename T>
std::size_t TStableIndexDenseArray<T>::add(const T& object)
{
	const std::size_t stableIndex = dispatchStableIndex();

	m_objects.push_back(std::move(object));
	return stableIndex;
}

template<typename T>
std::size_t TStableIndexDenseArray<T>::add(T&& object)
{
	const std::size_t stableIndex = dispatchStableIndex();
	
	m_objects.push_back(std::move(object));
	return stableIndex;
}

template<typename T>
bool TStableIndexDenseArray<T>::remove(const std::size_t stableIndex)
{
	if(!isStableIndexValid(stableIndex))
	{
		std::cerr << "at remove(), invalid stableIndex detected" << std::endl;
		return false;
	}

	PH_ASSERT(stableIndex < m_indexToObjectMapValidityPairs.size());
	const std::size_t objectIndex = m_indexToObjectMapValidityPairs[stableIndex].first;
	const std::size_t lastIndex = length() - 1;

	// Swap the target object with the last object; also copy the last object's stable
	// index to new location.
	PH_ASSERT(objectIndex < m_objects.size()          && lastIndex < m_objects.size());
	PH_ASSERT(objectIndex < m_objectToIndexMap.size() && lastIndex < m_objectToIndexMap.size());
	std::swap(m_objects[objectIndex], m_objects[lastIndex]);
	m_objectToIndexMap[objectIndex] = m_objectToIndexMap[lastIndex];

	// Update target object's validity.
	m_indexToObjectMapValidityPairs[stableIndex].second = false;

	// Update swapped object's stable index mapping.
	PH_ASSERT(m_objectToIndexMap[objectIndex] < m_indexToObjectMapValidityPairs.size());
	m_indexToObjectMapValidityPairs[m_objectToIndexMap[objectIndex]].first = objectIndex;

	// Add freed stable index for later use.
	m_freeIndices.push_back(stableIndex);

	m_objects.pop_back();
	m_objectToIndexMap.pop_back();

	return true;
}

template<typename T>
void TStableIndexDenseArray<T>::removeAll()
{
	m_objects.clear();
	m_objectToIndexMap.clear();
	m_indexToObjectMapValidityPairs.clear();
	m_freeIndices.clear();
}

template<typename T>
bool TStableIndexDenseArray<T>::isStableIndexValid(const std::size_t stableIndex) const
{
	if(stableIndex >= m_indexToObjectMapValidityPairs.size())
	{
		return false;
	}

	return m_indexToObjectMapValidityPairs[stableIndex].second;
}

template<typename T>
std::size_t TStableIndexDenseArray<T>::nextStableIndex() const
{
	return m_freeIndices.empty() ? m_objects.size() : m_freeIndices.back();
}

template<typename T>
T& TStableIndexDenseArray<T>::operator [] (const std::size_t stableIndex)
{
	PH_ASSERT(stableIndex < m_indexToObjectMapValidityPairs.size());
	PH_ASSERT(m_indexToObjectMapValidityPairs[stableIndex].first < m_objects.size());

	return m_objects[m_indexToObjectMapValidityPairs[stableIndex].first];
}

template<typename T>
const T& TStableIndexDenseArray<T>::operator [] (const std::size_t stableIndex) const
{
	PH_ASSERT(stableIndex < m_indexToObjectMapValidityPairs.size());
	PH_ASSERT(m_indexToObjectMapValidityPairs[stableIndex].first < m_objects.size());

	return m_objects[m_indexToObjectMapValidityPairs[stableIndex].first];
}

template<typename T>
T* TStableIndexDenseArray<T>::get(const std::size_t stableIndex)
{
	return isStableIndexValid(stableIndex) ? &((*this)[stableIndex]) : nullptr;
}

template<typename T>
const T* TStableIndexDenseArray<T>::get(const std::size_t stableIndex) const
{
	return isStableIndexValid(stableIndex) ? &((*this)[stableIndex]) : nullptr;
}

template<typename T>
std::size_t TStableIndexDenseArray<T>::length() const
{
	return m_objects.size();
}

template<typename T>
typename std::vector<T>::iterator TStableIndexDenseArray<T>::begin() noexcept
{
	return m_objects.begin();
}

template<typename T>
typename std::vector<T>::const_iterator TStableIndexDenseArray<T>::begin() const noexcept
{
	return m_objects.begin();
}

template<typename T>
typename std::vector<T>::iterator TStableIndexDenseArray<T>::end() noexcept
{
	return m_objects.end();
}

template<typename T>
typename std::vector<T>::const_iterator TStableIndexDenseArray<T>::end() const noexcept
{
	return m_objects.end();
}

template<typename T>
std::size_t TStableIndexDenseArray<T>::dispatchStableIndex()
{
	std::size_t stableIndex;

	if(m_freeIndices.empty())
	{
		stableIndex = m_objects.size();
		m_indexToObjectMapValidityPairs.push_back(std::make_pair(stableIndex, true));
	}
	else
	{
		stableIndex = m_freeIndices.back();
		m_freeIndices.pop_back();

		PH_ASSERT(stableIndex < m_indexToObjectMapValidityPairs.size());

		m_indexToObjectMapValidityPairs[stableIndex].first = m_objects.size();
		m_indexToObjectMapValidityPairs[stableIndex].second = true;
	}

	m_objectToIndexMap.push_back(stableIndex);

	return stableIndex;
}

}// end namespace ph
