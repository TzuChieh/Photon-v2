#pragma once

#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Engine/Core/Intersection/primitive_decorations.h"
#include "Engine/Core/HitEvent.h"

#include <Common/assertion.h>
#include <Common/compiler.h>

#include <concepts>
#include <utility>

namespace ph
{

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

	bool isIntersecting(HitEvent& event) const override final
	{
		if(m_primitiveGetter().isIntersecting(event))
		{
			// Hit detail will be modified by this primitive
			event.pushIntermediateHit(this);

			return true;
		}
		else
		{
			return false;
		}
	}

	bool reintersect(HitEvent& event, HitEvent& srcEvent) const override final
	{
		PH_ASSERT(srcEvent.getTopHit() == this);
		srcEvent.popHit();

		if(srcEvent.getTopHit()->reintersect(event, srcEvent))
		{
			event.pushIntermediateHit(this);
			return true;
		}
		else
		{
			return false;
		}
	}

	void calcHitDetail(HitEvent& event, HitDetail* const out_detail) const override final
	{
		// If failed, it is likely to be caused by: 1. mismatched/missing event push or pop in
		// the hit stack; 2. the hit event is invalid
		PH_ASSERT(event.getTopHit() == this);
		event.popHit();

		// Current hit is not necessary the injectee (as obtained via `getInjectee()`). For example,
		// if the injectee contains multiple instances then it could simply skip over to one of them.
		event.getTopHit()->calcHitDetail(event, out_detail);

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

	void genPosSample(PrimitivePosSampleQuery& query, SampleFlow& sampleFlow) const override final
	{
		m_primitiveGetter().genPosSample(query, sampleFlow);

		// Hit detail will be modified by this primitive
		if(query.outputs)
		{
			query.outputs.event().pushIntermediateHit(this);
		}
	}

	void calcPosPdf(PrimitivePosPdfQuery& query) const override final
	{
		m_primitiveGetter().calcPosPdf(query);
	}

	real calcExtendedArea() const override final
	{
		return m_primitiveGetter().calcExtendedArea();
	}

	[[nodiscard]]
	std::size_t getAttribute(
		const EPrimitiveAttribute attribute,
		const EAttributeDomain domain,
		const uint64 faceID,
		const TSpan<math::Vector3R> out_values) const override final
	{
		return m_primitiveGetter().getAttribute(attribute, domain, faceID, out_values);
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
