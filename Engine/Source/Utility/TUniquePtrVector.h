#pragma once

#include "Common/assertion.h"

#include <vector>
#include <memory>
#include <utility>
#include <cstddef>

namespace ph
{

template<typename BaseType>
class TUniquePtrVector final
{
public:
	// TODO: emplace

	template<typename T>
	T* add(std::unique_ptr<T> uniquePtr);

	std::unique_ptr<BaseType> remove(std::size_t index);
	void removeAll();
	void clearOne(std::size_t index);
	void clearRange(std::size_t beginIndex, std::size_t endIndex);
	BaseType* get(std::size_t index) const;

	std::unique_ptr<BaseType>& getUniquePtr(std::size_t index);
	const std::unique_ptr<BaseType>& getUniquePtr(std::size_t index) const;

	std::size_t size() const;

	BaseType* operator [] (std::size_t index) const;

	/*! @name Iterators

	Iterators for stored objects.
	*/
	///@{
	typename std::vector<std::unique_ptr<BaseType>>::iterator       begin() noexcept;
	typename std::vector<std::unique_ptr<BaseType>>::const_iterator begin() const noexcept;
	typename std::vector<std::unique_ptr<BaseType>>::iterator       end()   noexcept;
	typename std::vector<std::unique_ptr<BaseType>>::const_iterator end()   const noexcept;
	///@}

private:
	std::vector<std::unique_ptr<BaseType>> m_uniquePtrs;
};

// In-header Implementations:

template<typename BaseType>
template<typename T>
inline T* TUniquePtrVector<BaseType>::add(std::unique_ptr<T> uniquePtr)
{
	// The `is_same` check is necessary since `is_base_of` would be false if `T` is a fundamental type
	static_assert(std::is_base_of_v<BaseType, T> || std::is_same_v<BaseType, T>);

	T* const ptr = uniquePtr.get();
	m_uniquePtrs.push_back(std::move(uniquePtr));
	return ptr;
}

template<typename BaseType>
inline std::unique_ptr<BaseType> TUniquePtrVector<BaseType>::remove(const std::size_t index)
{
	PH_ASSERT_IN_RANGE(index, 0, m_uniquePtrs.size());

	auto uniquePtr = std::move(m_uniquePtrs[index]);
	m_uniquePtrs.erase(m_uniquePtrs.begin() + index);
	return uniquePtr;
}

template<typename BaseType>
inline void TUniquePtrVector<BaseType>::removeAll()
{
	m_uniquePtrs.clear();
}

template<typename BaseType>
inline void TUniquePtrVector<BaseType>::clearOne(const std::size_t index)
{
	PH_ASSERT_IN_RANGE(index, 0, m_uniquePtrs.size());

	m_uniquePtrs[index] = nullptr;
}

template<typename BaseType>
inline void TUniquePtrVector<BaseType>::clearRange(const std::size_t beginIndex, const std::size_t endIndex)
{
	PH_ASSERT_LE(beginIndex, endIndex);

	for(std::size_t ptrIndex = beginIndex; ptrIndex < endIndex; ++ptrIndex)
	{
		clearOne(ptrIndex);
	}
}

template<typename BaseType>
inline BaseType* TUniquePtrVector<BaseType>::get(const std::size_t index) const
{
	return getUniquePtr(index).get();
}

template<typename BaseType>
inline std::unique_ptr<BaseType>& TUniquePtrVector<BaseType>::getUniquePtr(const std::size_t index)
{
	PH_ASSERT_IN_RANGE(index, 0, m_uniquePtrs.size());
	return m_uniquePtrs[index];
}

template<typename BaseType>
inline const std::unique_ptr<BaseType>& TUniquePtrVector<BaseType>::getUniquePtr(const std::size_t index) const
{
	PH_ASSERT_IN_RANGE(index, 0, m_uniquePtrs.size());
	return m_uniquePtrs[index];
}

template<typename BaseType>
inline std::size_t TUniquePtrVector<BaseType>::size() const
{
	return m_uniquePtrs.size();
}

template<typename BaseType>
inline BaseType* TUniquePtrVector<BaseType>::operator [] (const std::size_t index) const
{
	return get(index);
}

template<typename BaseType>
inline typename std::vector<std::unique_ptr<BaseType>>::iterator TUniquePtrVector<BaseType>::begin() noexcept
{
	return m_uniquePtrs.begin();
}

template<typename BaseType>
inline typename std::vector<std::unique_ptr<BaseType>>::const_iterator TUniquePtrVector<BaseType>::begin() const noexcept
{
	return m_uniquePtrs.begin();
}

template<typename BaseType>
inline typename std::vector<std::unique_ptr<BaseType>>::iterator TUniquePtrVector<BaseType>::end() noexcept
{
	return m_uniquePtrs.end();
}

template<typename BaseType>
inline typename std::vector<std::unique_ptr<BaseType>>::const_iterator TUniquePtrVector<BaseType>::end() const noexcept
{
	return m_uniquePtrs.end();
}

}// end namespace ph
