#pragma once

//#include "Common/logging.h"
#include "Common/primitive_type.h"

#include <vector>
#include <utility>
#include <iostream>

//DECLARE_LOG_SENDER_EXTERN(TStableIndexDenseArray);

namespace ph
{

// Notes on this container:
//
// 1. Does not preserve order, i.e., the order for iteration may not be the same
//    as how you added the objects.
//
// 2. Inserted objects are guaranteed being contiguous in virtual memory.
//
// 3. User accessible indices are guaranteed to be stable, i.e., they are invariant 
//    to any modification to the container and can be used to retrieve the object
//    previously added until removal.
//
// 4. Add, remove and retrieve object are all O(1) operations.

// TODO: validity check on get methods

template<typename T>
class TStableIndexDenseArray
{
public:
	// Set to maximum value a std::size_t can hold. Due to current stable index dispatching implementation
	// and physical memory limit, this value should be nearly impossible to reach.
	static const std::size_t INVALID_STABLE_INDEX = static_cast<std::size_t>(-1);

public:
	TStableIndexDenseArray();

	// Construct with reserved memory spaces for initialCapacity T's.
	TStableIndexDenseArray(const std::size_t initialCapacity);

	// Add an object and returns a stable index.
	std::size_t add(const T& object);
	std::size_t add(const T&& object);

	// Remove an object by its stable index.
	bool remove(const std::size_t stableIndex);

	// Returns how many objects are there in this container.
	std::size_t length() const;

	// Get the next stable index that will be returned by add().
	// Note: This array should remain unchanged between current method call and add(); otherwise the
	// result is unreliable.
	std::size_t nextStableIndex() const;

	// Check whether the stable index represent a valid object or not.
	bool isStableIndexValid(const std::size_t stableIndex) const;

	typename std::vector<T>::iterator       begin() noexcept;
	typename std::vector<T>::const_iterator begin() const noexcept;
	typename std::vector<T>::iterator       end()   noexcept;
	typename std::vector<T>::const_iterator end()   const noexcept;

	// Retrieve object (no index validity check).
	T& operator [] (const std::size_t stableIndex);
	const T& operator [] (const std::size_t stableIndex) const;

private:
	std::vector<T> m_objects;
	std::vector<std::size_t> m_objectToIndexMap;
	std::vector<std::pair<std::size_t, bool>> m_indexToObjectMapValidityPairs;
	std::vector<std::size_t> m_freeIndices;

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

	m_objects.push_back(object);
	return stableIndex;
}

template<typename T>
std::size_t TStableIndexDenseArray<T>::add(const T&& object)
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
		//ENGINE_LOG(TStableIndexDenseArray, LogLevel::NOTE_WARNING, "at remove(), invalid stableIndex detected");
		std::cerr << "at remove(), invalid stableIndex detected" << std::endl;
		return false;
	}

	const std::size_t objectIndex = m_indexToObjectMapValidityPairs[stableIndex].first;
	const std::size_t lastIndex = length() - 1;

	// Swap the target object with the last object; also copy the last object's stable
	// index to new location.
	std::swap(m_objects[objectIndex], m_objects[lastIndex]);
	m_objectToIndexMap[objectIndex] = m_objectToIndexMap[lastIndex];

	// Update target object's validity.
	m_indexToObjectMapValidityPairs[stableIndex].second = false;

	// Update swapped object's stable index mapping.
	m_indexToObjectMapValidityPairs[m_objectToIndexMap[objectIndex]].first = objectIndex;

	// Add freed stable index for later use.
	m_freeIndices.push_back(stableIndex);

	m_objects.pop_back();
	m_objectToIndexMap.pop_back();

	return true;
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
	return m_objects[m_indexToObjectMapValidityPairs[stableIndex].first];
}

template<typename T>
const T& TStableIndexDenseArray<T>::operator [] (const std::size_t stableIndex) const
{
	return m_objects[m_indexToObjectMapValidityPairs[stableIndex].first];
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

		m_indexToObjectMapValidityPairs[stableIndex].first = m_objects.size();
		m_indexToObjectMapValidityPairs[stableIndex].second = true;
	}

	m_objectToIndexMap.push_back(stableIndex);

	return stableIndex;
}

}// end namespace ph