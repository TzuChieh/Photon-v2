#pragma once

#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/Intersection/DataStructure/TIndexRangeMap.h"
#include "Engine/Core/HitProbe.h"
#include "Engine/Utility/traits.h"

#include <Common/assertion.h>
#include <Common/compiler.h>

#include <concepts>
#include <memory>
#include <utility>

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
};

template<
	typename PrimitiveMetadataGetter,
	typename PrimitiveGetter,
	typename MetadataSlotMapper = NativeMetadataSlotMapper>
class TMetaInjectionPrimitive : public Primitive
{
	static_assert(detail::CPrimitiveMetadataGetter<PrimitiveMetadataGetter>,
		"Input type does not fulfill the requirements of a PrimitiveMetadataGetter.");
	static_assert(detail::CPrimitiveGetter<PrimitiveGetter>,
		"Input type does not fulfill the requirements of a PrimitiveGetter.");
	static_assert(detail::CMetadataSlotMapper<MetadataSlotMapper, PrimitiveGetter>,
		"Input type does not fulfill the requirements of a MetadataSlotMapper.");

public:
	TMetaInjectionPrimitive(PrimitiveMetadataGetter metadataGetter, PrimitiveGetter primitiveGetter)
		requires std::same_as<MetadataSlotMapper, NativeMetadataSlotMapper>

		: TMetaInjectionPrimitive(
			std::move(metadataGetter),
			std::move(primitiveGetter),
			NativeMetadataSlotMapper{})
	{}

	TMetaInjectionPrimitive(
		PrimitiveMetadataGetter metadataGetter,
		PrimitiveGetter primitiveGetter,
		MetadataSlotMapper slotMapper)

		: Primitive()
		, m_metadataGetter(std::move(metadataGetter))
		, m_primitiveGetter(std::move(primitiveGetter))
		, m_slotMapper(std::move(slotMapper))
	{}

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override final
	{
		if(m_primitiveGetter().isIntersecting(ray, probe))
		{
			// Hit detail will be modified by this primitive
			probe.pushIntermediateHit(this);

			return true;
		}
		else
		{
			return false;
		}
	}

	bool reintersect(
		const Ray& ray,
		HitProbe& probe,
		const Ray& srcRay,
		HitProbe& srcProbe) const override final
	{
		PH_ASSERT(srcProbe.getTopHit() == this);
		srcProbe.popHit();

		if(srcProbe.getTopHit()->reintersect(ray, probe, srcRay, srcProbe))
		{
			probe.pushIntermediateHit(this);
			return true;
		}
		else
		{
			return false;
		}
	}

	void calcHitDetail(
		const Ray&       ray,
		HitProbe&        probe,
		HitDetail* const out_detail) const override final
	{
		// If failed, it is likely to be caused by: 1. mismatched/missing probe push or pop in
		// the hit stack; 2. the hit event is invalid
		PH_ASSERT(probe.getTopHit() == this);
		probe.popHit();

		// Current hit is not necessary the injectee (as obtained via `getInjectee()`). For example,
		// if the injectee contains multiple instances then it could simply skip over to one of them.
		probe.getTopHit()->calcHitDetail(ray, probe, out_detail);

		// This is a representative of the original primitive
		out_detail->updatePrimitive(this);

		// Global primitive ID is not updated since no instancing is done here
	}

	math::AABB3D calcAABB() const override final
	{
		return m_primitiveGetter().calcAABB();
	}

	bool isOccluding(const Ray& ray) const override
	{
		return m_primitiveGetter().isOccluding(ray);
	}

	bool mayOverlapVolume(const math::AABB3D& volume) const override final
	{
		return m_primitiveGetter().mayOverlapVolume(volume);
	}

	void genPosSample(
		PrimitivePosSampleQuery& query, 
		SampleFlow& sampleFlow,
		HitProbe& probe) const override final
	{
		m_primitiveGetter().genPosSample(query, sampleFlow, probe);

		// Hit detail will be modified by this primitive
		probe.pushIntermediateHit(this);
	}

	void calcPosPdf(PrimitivePosPdfQuery& query) const override final
	{
		m_primitiveGetter().calcPosPdf(query);
	}

	real calcExtendedArea() const override final
	{
		return m_primitiveGetter().calcExtendedArea();
	}

	uint32 numMetadataSlots() const override final
	{
		return m_slotMapper.numMetadataSlots(m_primitiveGetter);
	}

	uint32 toMetadataSlot(const uint64 faceID) const override final
	{
		return m_slotMapper.toMetadataSlot(m_primitiveGetter, faceID);
	}

	const PrimitiveMetadata& getMetadata(const uint32 slot) const override final
	{
		// Metadata from `m_primitiveGetter()->getMetadata()` (if any) is intentionally overridden
		// by the injected one
		return m_metadataGetter(slot);
	}

	/*! @brief Gets the primitive that has got metadata injected.
	@return Pointer to the primitive carried by `PrimitiveGetter`.
	*/
	const auto& getInjectee() const
	{
		return m_primitiveGetter();
	}

private:
	[[PH_NO_UNIQUE_ADDRESS]]
	PrimitiveMetadataGetter m_metadataGetter;

	[[PH_NO_UNIQUE_ADDRESS]]
	PrimitiveGetter m_primitiveGetter;

	[[PH_NO_UNIQUE_ADDRESS]]
	MetadataSlotMapper m_slotMapper;
};

}// end namespace ph
