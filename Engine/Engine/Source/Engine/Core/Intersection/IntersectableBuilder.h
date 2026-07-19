#pragma once

#include "Engine/Core/Intersection/TTransformedIntersectable.h"

#include <utility>

namespace ph
{

template<typename IntersectableGetter>
class TIntersectableBuilder;

class IntersectableBuilder final
{
public:
	/*! The referenced `intersectable` must outlive the built wrapper chain.
	*/
	template<CDerived<Intersectable> IntersectableType>
	static auto referencing(const IntersectableType* const intersectable)
	{
		return TIntersectableBuilder<TReferencedIntersectableGetter<IntersectableType>>(
			TReferencedIntersectableGetter<IntersectableType>(intersectable));
	}

	/*! The built wrapper chain owns the intersectable value.
	*/
	template<CDerived<Intersectable> IntersectableType, typename... DeducedArgs>
	static auto embedding(DeducedArgs&&... args)
	{
		return TIntersectableBuilder<TEmbeddedIntersectableGetter<IntersectableType>>(
			TEmbeddedIntersectableGetter<IntersectableType>(std::forward<DeducedArgs>(args)...));
	}
};

/*! @brief Builder state for intersectable wrapper chains.
@tparam IntersectableGetter Current intersectable getter type.
*/
template<typename IntersectableGetter>
class TIntersectableBuilder final
{
public:
	explicit TIntersectableBuilder(IntersectableGetter intersectableGetter)
		: m_intersectableGetter(std::move(intersectableGetter))
	{}

	/*! Constructs the outermost intersectable.
	*/
	auto build()
	{
		static_assert(detail::CIntersectableGetter<IntersectableGetter>,
			"`IntersectableBuilder::build()` requires an intersectable getter input.");
		static_assert(
			requires (IntersectableGetter getter)
			{
				std::move(getter).claimEmbedded();
			},
			"`IntersectableBuilder::build()` can only return an intersectable object owned by "
			"the builder chain. A chain such as `IntersectableBuilder::referencing(i).build()` "
			"is invalid because it only points to an external intersectable. Use "
			"`embedding<T>()` to build an intersectable value directly, or add an "
			"intersectable-producing decoration such as `transform()` before calling `build()`.");

		return std::move(m_intersectableGetter).claimEmbedded();
	}

	/*! Applies a general transform and keeps the chain as an intersectable.
	*/
	template<bool SHOULD_FLIP_NG = false>
	auto transform(
		const Transform* const localToWorld,
		const Transform* const worldToLocal)
	{
		using TransformedIntersectable = TTransformedIntersectable<IntersectableGetter, SHOULD_FLIP_NG>;

		return TIntersectableBuilder<TEmbeddedIntersectableGetter<TransformedIntersectable>>(
			TEmbeddedIntersectableGetter<TransformedIntersectable>(
				std::move(m_intersectableGetter),
				localToWorld,
				worldToLocal));
	}

private:
	[[PH_NO_UNIQUE_ADDRESS]]
	IntersectableGetter m_intersectableGetter;
};

}// end namespace ph
