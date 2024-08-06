#pragma once

#include "Math/Geometry/TLineSegment.h"

#include <Common/primitive_type.h>

#include <concepts>
#include <optional>
#include <cstddef>

namespace ph::math
{

template<typename TesterFunc, typename Item>
concept CItemSegmentIntersectionTesterVanilla = requires (TesterFunc func, Item item)
{
	{ func(item, TLineSegment<real>{}) } -> std::same_as<std::optional<real>>;
};

template<typename TesterFunc, typename Item>
concept CItemSegmentIntersectionTesterWithIndex = requires (
	TesterFunc func, 
	Item item, 
	TLineSegment<real> segment,
	std::size_t itemIndex)
{
	{ func(item, segment, itemIndex) } -> std::same_as<std::optional<real>>;
};

template<typename TesterFunc, typename Item>
concept CItemSegmentIntersectionTester = 
	CItemSegmentIntersectionTesterVanilla<TesterFunc, Item> ||
	CItemSegmentIntersectionTesterWithIndex<TesterFunc, Item>;

}// end namespace ph::math
