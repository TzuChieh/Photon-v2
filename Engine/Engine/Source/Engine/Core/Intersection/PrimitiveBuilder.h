#pragma once

#include "Engine/Core/Intersection/TMetaInjectionPrimitive.h"

#include <concepts>
#include <memory>
#include <utility>

namespace ph
{

struct NoPrimitiveBuilderInput final
{};

template<
	typename PrimitiveGetter,
	typename MetadataGetter = NoPrimitiveBuilderInput,
	typename MetadataSlotMapper = NoPrimitiveBuilderInput>
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
			TReferencedPrimitiveGetter<PrimitiveType>(primitive),
			NoPrimitiveBuilderInput{},
			NoPrimitiveBuilderInput{});
	}

	/*! The built wrapper chain owns the primitive value.
	*/
	template<CDerived<Primitive> PrimitiveType, typename... DeducedArgs>
	static auto embedding(DeducedArgs&&... args)
	{
		return TPrimitiveBuilder<TEmbeddedPrimitiveGetter<PrimitiveType>>(
			TEmbeddedPrimitiveGetter<PrimitiveType>(std::forward<DeducedArgs>(args)...),
			NoPrimitiveBuilderInput{},
			NoPrimitiveBuilderInput{});
	}
};

/*! @brief Builder state for pending primitive wrapper inputs.
@tparam PrimitiveGetter Current primitive getter type.
@tparam MetadataGetter Current metadata getter type, or `NoPrimitiveBuilderInput`.
@tparam MetadataSlotMapper Current metadata slot mapper type, or `NoPrimitiveBuilderInput`.
*/
template<
	typename PrimitiveGetter,
	typename MetadataGetter,
	typename MetadataSlotMapper>
class TPrimitiveBuilder final
{
	static constexpr bool HAS_METADATA = !std::same_as<MetadataGetter, NoPrimitiveBuilderInput>;
	static constexpr bool HAS_SLOT_MAPPER = !std::same_as<MetadataSlotMapper, NoPrimitiveBuilderInput>;

public:
	TPrimitiveBuilder(
		PrimitiveGetter primitiveGetter,
		MetadataGetter metadataGetter,
		MetadataSlotMapper slotMapper)

		: m_primitiveGetter(std::move(primitiveGetter))
		, m_metadataGetter(std::move(metadataGetter))
		, m_slotMapper(std::move(slotMapper))
	{}

	/*! Constructs the outermost primitive.
	*/
	auto build()
	{
		static_assert(detail::CPrimitiveGetter<PrimitiveGetter>,
			"`PrimitiveBuilder::build()` requires a primitive getter input.");
		static_assert(HAS_METADATA,
			"`PrimitiveBuilder::build()` requires a metadata input.");
		static_assert(detail::CPrimitiveMetadataGetter<MetadataGetter>,
			"`PrimitiveBuilder::build()` requires a metadata getter input.");

		if constexpr(!HAS_SLOT_MAPPER)
		{
			return TMetaInjectionPrimitive<MetadataGetter, PrimitiveGetter>(
				std::move(m_metadataGetter),
				std::move(m_primitiveGetter));
		}
		else
		{
			static_assert(detail::CMetadataSlotMapper<MetadataSlotMapper, PrimitiveGetter>,
				"`PrimitiveBuilder::build()` requires a metadata slot mapper input.");

			return TMetaInjectionPrimitive<MetadataGetter, PrimitiveGetter, MetadataSlotMapper>(
				std::move(m_metadataGetter),
				std::move(m_primitiveGetter),
				std::move(m_slotMapper));
		}
	}

	/*! Uses one externally-owned metadata slot for all hits.
	*/
	auto injectMetadata(const PrimitiveMetadata* const metadata)
	{
		static_assert(!HAS_METADATA,
			"`PrimitiveBuilder` already has metadata. Build the current primitive before injecting new metadata.");

		return TPrimitiveBuilder<
			PrimitiveGetter,
			ReferencedPrimitiveMetadataGetter,
			MetadataSlotMapper>(
				std::move(m_primitiveGetter),
				ReferencedPrimitiveMetadataGetter(metadata),
				std::move(m_slotMapper));
	}

	/*! Owns one metadata slot in the wrapper chain.
	*/
	auto injectMetadataCopy(PrimitiveMetadata metadata)
	{
		static_assert(!HAS_METADATA,
			"`PrimitiveBuilder` already has metadata. Build the current primitive before injecting new metadata.");

		return TPrimitiveBuilder<
			PrimitiveGetter,
			EmbeddedPrimitiveMetadataGetter,
			MetadataSlotMapper>(
				std::move(m_primitiveGetter),
				EmbeddedPrimitiveMetadataGetter(std::move(metadata)),
				std::move(m_slotMapper));
	}

	/*! Uses externally-owned metadata slots and a face-ID-to-slot map.
	*/
	auto injectMetadataArray(
		std::unique_ptr<const PrimitiveMetadata*[]> metadatas,
		const uint32 numMetadatas,
		const TIndexRangeMap<uint64, uint32>& faceIdToMetadataSlot)
	{
		static_assert(!HAS_METADATA,
			"`PrimitiveBuilder` already has metadata. Build the current primitive before injecting new metadata.");

		return TPrimitiveBuilder<
			PrimitiveGetter,
			ReferencedPrimitiveMetadataArrayGetter,
			FaceIdToMetadataSlotMapper>(
				std::move(m_primitiveGetter),
				ReferencedPrimitiveMetadataArrayGetter(std::move(metadatas), numMetadatas),
				FaceIdToMetadataSlotMapper(&faceIdToMetadataSlot, numMetadatas));
	}

private:
	[[PH_NO_UNIQUE_ADDRESS]]
	PrimitiveGetter m_primitiveGetter;

	[[PH_NO_UNIQUE_ADDRESS]]
	MetadataGetter m_metadataGetter;

	[[PH_NO_UNIQUE_ADDRESS]]
	MetadataSlotMapper m_slotMapper;
};

}// end namespace ph
