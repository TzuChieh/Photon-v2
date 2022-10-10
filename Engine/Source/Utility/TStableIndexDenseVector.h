#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"

#include <cstddef>
#include <vector>
#include <utility>
#include <optional>
#include <limits>
#include <concepts>

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

@tparam SIndex The type of the stable index. Limits the maximum number of objects the vector can store.
*/
template<typename T, std::integral SIndex = std::size_t>
class TStableIndexDenseVector final
{
public:
	/*! @brief An index that indicates an invalid state.
	The value is the maximum value the @p Index type can hold. 
	*/
	constexpr static auto INVALID_STABLE_INDEX = std::numeric_limits<SIndex>::max();

public:
	/*! @brief An vector that holds nothing.
	*/
	TStableIndexDenseVector();

	/*! @brief An vector that reserves memory space for @p initialCapacity objects.
	*/
	explicit TStableIndexDenseVector(std::size_t initialCapacity);

	/*! @brief Adds an object and returns a stable index for it.
	*/
	template<typename U>
	SIndex add(U&& object);

	/*! @brief Remove an object by its stable index.
	@return `true` if and only if @p stableIndex is valid and object is
	correctly removed.
	*/
	bool remove(SIndex stableIndex);

	/*! @brief Remove all objects.
	*/
	void removeAll();

	/*! @brief Returns how many objects are there in this container.
	*/
	std::size_t size() const;

	/*! @brief Get the next stable index that will be returned by add().
	This vector should remain unchanged between this call and add(); otherwise
	the result is unreliable.
	*/
	SIndex nextStableIndex() const;

	/*! @brief Checks whether the stable index represent a valid object or not.
	@return `true` if @stableIndex maps to an object, otherwise `false`.
	*/
	bool isStableIndexValid(SIndex stableIndex) const;

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
	T& operator [] (SIndex stableIndex);
	const T& operator [] (SIndex stableIndex) const;
	///@}

	/*! @name Checked Accessors
	
	Retrieve object with index validity check.
	@return `nullptr` if @p stableIndex is invalid.
	*/
	///@{
	T* get(SIndex stableIndex);
	const T* get(SIndex stableIndex) const;
	///@}

private:
	SIndex dispatchStableIndex();

	// Object index type for locating objects in `m_objects` internally. Currently the same as `SIndex`
	// as we cannot store more objects than `SIndex` can store.
	using OIndex = SIndex;

	std::vector<T>                       m_objects;
	std::vector<SIndex>                  m_objectToStableIndex;
	std::vector<std::pair<OIndex, bool>> m_stableIndexToObject;
	std::vector<SIndex>                  m_freeIndices;
};

// Implementations:

template<typename T, std::integral SIndex>
inline TStableIndexDenseVector<T, SIndex>::TStableIndexDenseVector() = default;

template<typename T, std::integral SIndex>
inline TStableIndexDenseVector<T, SIndex>::TStableIndexDenseVector(const std::size_t initialCapacity) :
	TStableIndexDenseVector()
{
	m_objects.reserve(initialCapacity);
	m_objectToStableIndex.reserve(initialCapacity);
	m_stableIndexToObject.reserve(initialCapacity);
	m_freeIndices.reserve(initialCapacity);
}

template<typename T, std::integral SIndex>
template<typename U>
inline auto TStableIndexDenseVector<T, SIndex>::add(U&& object)
	-> SIndex
{
	const SIndex stableIndex = dispatchStableIndex();
	
	m_objects.push_back(std::forward<U>(object));
	return stableIndex;
}

template<typename T, std::integral SIndex>
inline bool TStableIndexDenseVector<T, SIndex>::remove(const SIndex stableIndex)
{
	if(!isStableIndexValid(stableIndex))
	{
		return false;
	}

	PH_ASSERT_LT(stableIndex, m_stableIndexToObject.size());
	const OIndex objIndex     = m_stableIndexToObject[stableIndex].first;
	const OIndex lastObjIndex = static_cast<OIndex>(size() - 1);

	// Swap the target object with the last object; also copy the last object's stable
	// index to new location.
	PH_ASSERT_LT(objIndex, m_objects.size());
	PH_ASSERT_LT(lastObjIndex, m_objects.size());
	PH_ASSERT_LT(objIndex, m_objectToStableIndex.size());
	PH_ASSERT_LT(lastObjIndex, m_objectToStableIndex.size());
	std::swap(m_objects[objIndex], m_objects[lastObjIndex]);
	m_objectToStableIndex[objIndex] = m_objectToStableIndex[lastObjIndex];

	// Update target object's validity.
	m_stableIndexToObject[stableIndex].second = false;

	// Update swapped object's stable index mapping.
	PH_ASSERT_LT(m_objectToStableIndex[objIndex], m_stableIndexToObject.size());
	m_stableIndexToObject[m_objectToStableIndex[objIndex]].first = objIndex;

	// Add freed stable index for later use.
	m_freeIndices.push_back(stableIndex);

	m_objects.pop_back();
	m_objectToStableIndex.pop_back();

	return true;
}

template<typename T, std::integral SIndex>
inline void TStableIndexDenseVector<T, SIndex>::removeAll()
{
	m_objects.clear();
	m_objectToStableIndex.clear();
	m_stableIndexToObject.clear();
	m_freeIndices.clear();
}

template<typename T, std::integral SIndex>
inline bool TStableIndexDenseVector<T, SIndex>::isStableIndexValid(const SIndex stableIndex) const
{
	if(stableIndex >= m_stableIndexToObject.size())
	{
		return false;
	}

	return m_stableIndexToObject[stableIndex].second;
}

template<typename T, std::integral SIndex>
inline auto TStableIndexDenseVector<T, SIndex>::nextStableIndex() const
	-> SIndex
{
	return m_freeIndices.empty() ? m_objects.size() : m_freeIndices.back();
}

template<typename T, std::integral SIndex>
inline T& TStableIndexDenseVector<T, SIndex>::operator [] (const SIndex stableIndex)
{
	PH_ASSERT_LT(stableIndex, m_stableIndexToObject.size());
	PH_ASSERT_LT(m_stableIndexToObject[stableIndex].first, m_objects.size());

	return m_objects[m_stableIndexToObject[stableIndex].first];
}

template<typename T, std::integral SIndex>
inline const T& TStableIndexDenseVector<T, SIndex>::operator [] (const SIndex stableIndex) const
{
	PH_ASSERT_LT(stableIndex, m_stableIndexToObject.size());
	PH_ASSERT_LT(m_stableIndexToObject[stableIndex].first, m_objects.size());

	return m_objects[m_stableIndexToObject[stableIndex].first];
}

template<typename T, std::integral SIndex>
inline T* TStableIndexDenseVector<T, SIndex>::get(const SIndex stableIndex)
{
	return isStableIndexValid(stableIndex) ? &((*this)[stableIndex]) : nullptr;
}

template<typename T, std::integral SIndex>
inline const T* TStableIndexDenseVector<T, SIndex>::get(const SIndex stableIndex) const
{
	return isStableIndexValid(stableIndex) ? &((*this)[stableIndex]) : nullptr;
}

template<typename T, std::integral SIndex>
inline std::size_t TStableIndexDenseVector<T, SIndex>::size() const
{
	return m_objects.size();
}

template<typename T, std::integral SIndex>
inline typename std::vector<T>::iterator TStableIndexDenseVector<T, SIndex>::begin() noexcept
{
	return m_objects.begin();
}

template<typename T, std::integral SIndex>
inline typename std::vector<T>::const_iterator TStableIndexDenseVector<T, SIndex>::begin() const noexcept
{
	return m_objects.begin();
}

template<typename T, std::integral SIndex>
inline typename std::vector<T>::iterator TStableIndexDenseVector<T, SIndex>::end() noexcept
{
	return m_objects.end();
}

template<typename T, std::integral SIndex>
inline typename std::vector<T>::const_iterator TStableIndexDenseVector<T, SIndex>::end() const noexcept
{
	return m_objects.end();
}

template<typename T, std::integral SIndex>
inline auto TStableIndexDenseVector<T, SIndex>::dispatchStableIndex()
	-> SIndex
{
	SIndex stableIndex = INVALID_STABLE_INDEX;
	if(m_freeIndices.empty())
	{
		stableIndex = static_cast<SIndex>(m_objects.size());
		m_stableIndexToObject.push_back(std::make_pair(stableIndex, true));
	}
	else
	{
		stableIndex = m_freeIndices.back();
		m_freeIndices.pop_back();

		PH_ASSERT_LT(stableIndex, m_stableIndexToObject.size());

		m_stableIndexToObject[stableIndex].first = static_cast<OIndex>(m_objects.size());
		m_stableIndexToObject[stableIndex].second = true;
	}

	m_objectToStableIndex.push_back(stableIndex);

	return stableIndex;
}

}// end namespace ph
