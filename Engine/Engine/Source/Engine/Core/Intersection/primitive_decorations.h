#pragma once

#include "Engine/Core/Intersection/Intersectable.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/Intersection/DataStructure/TIndexRangeMap.h"

#include <Common/assertion.h>

#include <concepts>
#include <memory>
#include <utility>

#include "Engine/Utility/traits.h"

namespace ph
{

namespace detail
{

template<typename GetterType>
concept CPrimitiveMetadataGetter = requires (const GetterType getter, uint32 slot)
{
	{ getter(slot) } -> std::same_as<const PrimitiveMetadata&>;
};

template<typename GetterType>
concept CPrimitiveGetter = requires (const GetterType getter)
{
	{ getter() } -> std::convertible_to<const Primitive&>;
};

template<typename GetterType>
concept CIntersectableGetter = requires (const GetterType getter)
{
	{ getter() } -> std::convertible_to<const Intersectable&>;
};

template<typename MapperType, typename PrimitiveGetter>
concept CMetadataSlotMapper = requires (
	const MapperType mapper,
	const PrimitiveGetter& primitiveGetter,
	uint64 faceID)
{
	{ mapper.numMetadataSlots(primitiveGetter) } -> std::same_as<uint32>;
	{ mapper.toMetadataSlot(primitiveGetter, faceID) } -> std::same_as<uint32>;
};

}// end namespace detail

template<CDerived<Primitive> PrimitiveType>
struct TReferencedPrimitiveGetter final
{
	const PrimitiveType* primitive;

	explicit TReferencedPrimitiveGetter(const PrimitiveType* const primitive)
		: primitive(primitive)
	{}

	const PrimitiveType& operator () () const
	{
		PH_ASSERT(primitive);
		return *primitive;
	}
};

template<CDerived<Primitive> PrimitiveType>
struct TEmbeddedPrimitiveGetter final
{
	PrimitiveType primitive;

	template<typename... DeducedArgs>
	explicit TEmbeddedPrimitiveGetter(DeducedArgs&&... args)
		: primitive(std::forward<DeducedArgs>(args)...)
	{}

	const PrimitiveType& operator () () const
	{
		return primitive;
	}

	PrimitiveType&& claimEmbedded() &&
	{
		return std::move(primitive);
	}
};

template<CDerived<Intersectable> IntersectableType>
struct TReferencedIntersectableGetter final
{
	const IntersectableType* intersectable;

	explicit TReferencedIntersectableGetter(const IntersectableType* const intersectable)
		: intersectable(intersectable)
	{}

	const IntersectableType& operator () () const
	{
		PH_ASSERT(intersectable);
		return *intersectable;
	}
};

template<CDerived<Intersectable> IntersectableType>
struct TEmbeddedIntersectableGetter final
{
	IntersectableType intersectable;

	template<typename... DeducedArgs>
	explicit TEmbeddedIntersectableGetter(DeducedArgs&&... args)
		: intersectable(std::forward<DeducedArgs>(args)...)
	{}

	const IntersectableType& operator () () const
	{
		return intersectable;
	}

	IntersectableType&& claimEmbedded() &&
	{
		return std::move(intersectable);
	}
};

struct ReferencedPrimitiveMetadataGetter final
{
	const PrimitiveMetadata* metadata;

	explicit ReferencedPrimitiveMetadataGetter(const PrimitiveMetadata* const metadata)
		: metadata(metadata)
	{}

	const PrimitiveMetadata& operator () (uint32 /* slot */) const
	{
		PH_ASSERT(metadata);
		return *metadata;
	}
};

struct EmbeddedPrimitiveMetadataGetter final
{
	PrimitiveMetadata metadata;

	template<typename... DeducedArgs>
	explicit EmbeddedPrimitiveMetadataGetter(DeducedArgs&&... args)
		: metadata(std::forward<DeducedArgs>(args)...)
	{}

	const PrimitiveMetadata& operator () (uint32 /* slot */) const
	{
		return metadata;
	}
};

struct ReferencedPrimitiveMetadataArrayGetter final
{
	std::unique_ptr<const PrimitiveMetadata*[]> metadatas;
	uint32 numMetadatas;

	ReferencedPrimitiveMetadataArrayGetter(std::unique_ptr<const PrimitiveMetadata*[]> metadatas, uint32 numMetadatas)
		: metadatas(std::move(metadatas))
		, numMetadatas(numMetadatas)
	{}

	const PrimitiveMetadata& operator () (const uint32 slot) const
	{
		PH_ASSERT_LT(slot, numMetadatas);
		PH_ASSERT(metadatas[slot]);
		return *metadatas[slot];
	}
};

struct EmbeddedPrimitiveMetadataArrayGetter final
{
	std::unique_ptr<PrimitiveMetadata[]> metadatas;
	uint32 numMetadatas;

	EmbeddedPrimitiveMetadataArrayGetter(std::unique_ptr<PrimitiveMetadata[]> metadatas, uint32 numMetadatas)
		: metadatas(std::move(metadatas))
		, numMetadatas(numMetadatas)
	{}

	const PrimitiveMetadata& operator () (const uint32 slot) const
	{
		PH_ASSERT_LT(slot, numMetadatas);
		return metadatas[slot];
	}
};

struct NativeMetadataSlotMapper final
{
	template<typename PrimitiveGetter>
	uint32 numMetadataSlots(const PrimitiveGetter& primitiveGetter) const
	{
		return primitiveGetter().numMetadataSlots();
	}

	template<typename PrimitiveGetter>
	uint32 toMetadataSlot(const PrimitiveGetter& primitiveGetter, const uint64 faceID) const
	{
		return primitiveGetter().toMetadataSlot(faceID);
	}
};

struct FaceIdToMetadataSlotMapper final
{
	const TIndexRangeMap<uint64, uint32>* faceIdToMetadataSlot;
	uint32 numSlots;

	FaceIdToMetadataSlotMapper(
		const TIndexRangeMap<uint64, uint32>* const faceIdToMetadataSlot,
		const uint32 numSlots)

		: faceIdToMetadataSlot(faceIdToMetadataSlot)
		, numSlots(numSlots)
	{}

	template<typename PrimitiveGetter>
	uint32 numMetadataSlots(const PrimitiveGetter& /* primitiveGetter */) const
	{
		return numSlots;
	}

	template<typename PrimitiveGetter>
	uint32 toMetadataSlot(const PrimitiveGetter& /* primitiveGetter */, const uint64 faceID) const
	{
		PH_ASSERT(faceIdToMetadataSlot);
		if(faceIdToMetadataSlot->isEmpty())
		{
			return 0;
		}

		return faceIdToMetadataSlot->get(faceID);
	}
};

}// end namespace ph
