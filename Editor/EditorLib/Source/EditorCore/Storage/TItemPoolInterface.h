#pragma once

namespace ph::editor
{

template<typename ItemAccessor, typename ItemViewer, typename Handle>
class TItemPoolInterface
{
public:
	using ItemAccessType = ItemAccessor;
	using ItemViewType = ItemViewer;
	using HandleType = Handle;

	inline virtual ~TItemPoolInterface() = default;

	virtual ItemAccessor accessItem(const Handle& handle) = 0;
	virtual ItemViewer viewItem(const Handle& handle) const = 0;
};

}// end namespace ph::editor
