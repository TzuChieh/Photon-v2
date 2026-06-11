#pragma once

#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/primitive_decorations.h"
#include "Engine/Core/HitProbe.h"

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
