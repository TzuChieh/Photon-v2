#pragma once

#include "EditorCore/Storage/fwd.h"
#include "EditorCore/Storage/TWeakHandle.h"

#include <Common/assertion.h>

#include <cstddef>

namespace ph::editor
{

/*! @brief Handle with strong reference semantics.
Default constructor creates empty handle.
*/
template<typename ItemInterface, typename Index = std::size_t, typename Generation = Index>
class TStrongHandle final
{
public:
	using WeakHandleType = TWeakHandle<ItemInterface, Index, Generation>;
	using PoolType = TItemPoolInterface<ItemInterface, WeakHandleType>;

	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TStrongHandle);

	inline TStrongHandle(const WeakHandleType& weakHandle, PoolType* const pool)
		: m_pool(pool)
		, m_weakHandle(weakHandle)
	{}

	/*!
	@return The underlying weak handle.
	*/
	inline const WeakHandleType& getWeak() const
	{
		return m_weakHandle;
	}

	/*!
	@return Whether the handle points to nothing.
	*/
	inline bool isEmpty() const
	{
		return !m_pool || m_weakHandle.isEmpty();
	}

	ItemInterface* accessItem();
	const ItemInterface* viewItem() const;

	inline ItemInterface* operator -> ()
	{
		return accessItem();
	}

	inline const ItemInterface* operator -> () const
	{
		return viewItem();
	}

	/*!
	@return Whether the handle points to something. Equivalent to checking if the handle is not empty.
	*/
	inline operator bool () const
	{
		return !isEmpty();
	}

	inline bool operator == (const TStrongHandle& rhs) const = default;

private:
	PoolType* m_pool = nullptr;
	WeakHandleType m_weakHandle;
};

}// end namespace ph::editor

#include "EditorCore/Storage/TItemPoolInterface.h"

namespace ph::editor
{

template<typename ItemInterface, typename Index, typename Generation>
inline ItemInterface* TStrongHandle<ItemInterface, Index, Generation>::accessItem()
{
	PH_ASSERT(!isEmpty());
	return m_pool->accessItem(m_weakHandle);
}

template<typename ItemInterface, typename Index, typename Generation>
inline const ItemInterface* TStrongHandle<ItemInterface, Index, Generation>::viewItem() const
{
	PH_ASSERT(!isEmpty());
	return m_pool->viewItem(m_weakHandle);
}

}// end namespace ph::editor
