#pragma once

#include "Engine/Core/Intersection/IntersectableBuilder.h"
#include "Engine/Core/Intersection/TMetaInjectionPrimitive.h"
#include "Engine/Core/Intersection/TTransformedPrimitive.h"

#include <concepts>
#include <memory>
#include <utility>

namespace ph
{

template<typename PrimitiveGetter>
class TPrimitiveBuilder;

/*! @brief Entry point for building typed primitive wrapper chains.
This hides wrapper plumbing such as primitive getters and metadata slot mappers, so actor cooking code
can describe the intended layers directly.
*/
class PrimitiveBuilder final
{
public:
	/*! The primitive must outlive the built wrapper chain.
	*/
	template<CDerived<Primitive> PrimitiveType>
	static auto referencing(const PrimitiveType* const primitive)
	{
		return TPrimitiveBuilder<TReferencedPrimitiveGetter<PrimitiveType>>(
			TReferencedPrimitiveGetter<PrimitiveType>(primitive));
	}

	/*! The built wrapper chain owns the primitive value.
	*/
	template<CDerived<Primitive> PrimitiveType, typename... DeducedArgs>
	static auto embedding(DeducedArgs&&... args)
	{
		return TPrimitiveBuilder<TEmbeddedPrimitiveGetter<PrimitiveType>>(
			TEmbeddedPrimitiveGetter<PrimitiveType>(std::forward<DeducedArgs>(args)...));
	}
};

/*! @brief Builder state for primitive wrapper chains.
@tparam PrimitiveGetter Current primitive getter type.
*/
template<typename PrimitiveGetter>
class TPrimitiveBuilder final
{
public:
	explicit TPrimitiveBuilder(PrimitiveGetter primitiveGetter)
		: m_primitiveGetter(std::move(primitiveGetter))
	{}

	/*! Constructs the outermost primitive.
	*/
	auto build()
	{
		static_assert(detail::CPrimitiveGetter<PrimitiveGetter>,
			"`PrimitiveBuilder::build()` requires a primitive getter input.");
		static_assert(
			requires (PrimitiveGetter getter)
			{
				std::move(getter).claimEmbedded();
			},
			"`PrimitiveBuilder::build()` can only return a primitive object owned by the "
			"builder chain. A chain such as `PrimitiveBuilder::referencing(p).build()` is "
			"invalid because it only points to an external primitive. Use `embedding<T>()` "
			"to build a primitive value directly, or add a primitive-producing decoration "
			"such as `injectMetadata()` or `rigidTransform()` before calling `build()`.");

		return std::move(m_primitiveGetter).claimEmbedded();
	}

	/*! Applies a rigid transform and keeps the chain as a primitive.
	*/
	auto rigidTransform(
		const RigidTransform* const localToWorld,
		const RigidTransform* const worldToLocal)
	{
		using TransformedPrimitive = TTransformedPrimitive<PrimitiveGetter>;

		return TPrimitiveBuilder<TEmbeddedPrimitiveGetter<TransformedPrimitive>>(
			TEmbeddedPrimitiveGetter<TransformedPrimitive>(
				std::move(m_primitiveGetter),
				localToWorld,
				worldToLocal));
	}

	/*! Applies a general transform and decays the chain to an intersectable.
	*/
	auto transform(
		const Transform* const localToWorld,
		const Transform* const worldToLocal)
	{
		using TransformedIntersectable = TTransformedIntersectable<PrimitiveGetter>;

		return TIntersectableBuilder<TEmbeddedIntersectableGetter<TransformedIntersectable>>(
			TEmbeddedIntersectableGetter<TransformedIntersectable>(
				std::move(m_primitiveGetter),
				localToWorld,
				worldToLocal));
	}

	/*! Uses one externally-owned metadata slot for all hits.
	*/
	auto injectMetadata(const PrimitiveMetadata* const metadata)
	{
		using MetaPrimitive = TMetaInjectionPrimitive<
			ReferencedPrimitiveMetadataGetter,
			PrimitiveGetter>;

		return TPrimitiveBuilder<TEmbeddedPrimitiveGetter<MetaPrimitive>>(
			TEmbeddedPrimitiveGetter<MetaPrimitive>(
				ReferencedPrimitiveMetadataGetter(metadata),
				std::move(m_primitiveGetter)));
	}

	/*! Owns one metadata slot in the wrapper chain.
	*/
	auto injectMetadataCopy(PrimitiveMetadata metadata)
	{
		using MetaPrimitive = TMetaInjectionPrimitive<
			EmbeddedPrimitiveMetadataGetter,
			PrimitiveGetter>;

		return TPrimitiveBuilder<TEmbeddedPrimitiveGetter<MetaPrimitive>>(
			TEmbeddedPrimitiveGetter<MetaPrimitive>(
				EmbeddedPrimitiveMetadataGetter(std::move(metadata)),
				std::move(m_primitiveGetter)));
	}

	/*! Uses externally-owned metadata slots and a face-ID-to-slot map.
	*/
	auto injectMetadataArray(
		std::unique_ptr<const PrimitiveMetadata*[]> metadatas,
		const uint32 numMetadatas,
		const TIndexRangeMap<uint64, uint32>* const faceIdToMetadataSlot)
	{
		using MetaPrimitive = TMetaInjectionPrimitive<
			ReferencedPrimitiveMetadataArrayGetter,
			PrimitiveGetter,
			FaceIdToMetadataSlotMapper>;

		return TPrimitiveBuilder<TEmbeddedPrimitiveGetter<MetaPrimitive>>(
			TEmbeddedPrimitiveGetter<MetaPrimitive>(
				ReferencedPrimitiveMetadataArrayGetter(std::move(metadatas), numMetadatas),
				std::move(m_primitiveGetter),
				FaceIdToMetadataSlotMapper(faceIdToMetadataSlot, numMetadatas)));
	}

private:
	[[PH_NO_UNIQUE_ADDRESS]]
	PrimitiveGetter m_primitiveGetter;
};

}// end namespace ph
