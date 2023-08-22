#pragma once

#include "EditorCore/Storage/fwd.h"

namespace ph::editor
{

template<typename ItemInterface, CWeakHandle Handle>
class TItemPoolInterface
{
public:
	using ItemInterfaceType = ItemInterface;
	using HandleType = Handle;

	inline virtual ~TItemPoolInterface() = default;

	virtual ItemInterface* accessItem(const Handle& handle) = 0;
	virtual const ItemInterface* viewItem(const Handle& handle) const = 0;
};

}// end namespace ph::editor
