#pragma once

#include "Engine/Core/HitDetail.h"
#include "Engine/Core/HitEvent.h"
#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Engine/Core/Intersection/primitive_decorations.h"

#include <Common/assertion.h>
#include <Common/compiler.h>

#include <utility>

namespace ph
{

/*! @brief Applies cook-time hit manipulations without changing primitive placement.
@tparam SHOULD_FLIP_NG Whether to flip the world-space geometric normal.
*/
template<typename PrimitiveGetter, bool SHOULD_FLIP_NG = false>
class THitManipPrimitive : public Primitive
{
	static_assert(detail::CPrimitiveGetter<PrimitiveGetter>,
		"Input type does not fulfill the requirements of a PrimitiveGetter.");

public:
	explicit THitManipPrimitive(PrimitiveGetter primitiveGetter)
		: Primitive()
		, m_inner(std::move(primitiveGetter))
	{}

	bool isIntersecting(HitEvent& event) const override
	{
		if(m_inner().isIntersecting(event))
		{
			event.pushIntermediateHit(this);
			return true;
		}

		return false;
	}

	bool reintersect(HitEvent& event, HitEvent& srcEvent) const override
	{
		PH_ASSERT(srcEvent.getTopHit() == this);
		srcEvent.popHit();

		if(srcEvent.getTopHit()->reintersect(event, srcEvent))
		{
			event.pushIntermediateHit(this);
			return true;
		}

		return false;
	}

	void calcHitDetail(HitEvent& event, HitDetail* const out_detail) const override
	{
		PH_ASSERT(event.getTopHit() == this);
		event.popHit();

		PH_ASSERT(event.getTopHit());
		event.getTopHit()->calcHitDetail(event, out_detail);
		out_detail->updatePrimitive(this);
		if constexpr(SHOULD_FLIP_NG)
		{
			out_detail->setFlippedGeometryNormal();
		}

		// No global primitive ID update: this decorator creates no instance
	}

	bool isOccluding(const Ray& ray) const override
	{
		return m_inner().isOccluding(ray);
	}

	bool mayOverlapVolume(const math::AABB3D& volume) const override
	{
		return m_inner().mayOverlapVolume(volume);
	}

	math::AABB3D calcAABB() const override
	{
		return m_inner().calcAABB();
	}

	void genPosSample(PrimitivePosSampleQuery& query, SampleFlow& sampleFlow) const override
	{
		m_inner().genPosSample(query, sampleFlow);
		if(query.outputs)
		{
			query.outputs.event().pushIntermediateHit(this);
		}
	}

	void calcPosPdf(PrimitivePosPdfQuery& query) const override
	{
		m_inner().calcPosPdf(query);
	}

	real calcExtendedArea() const override
	{
		return m_inner().calcExtendedArea();
	}

	[[nodiscard]]
	std::size_t getAttribute(
		const EPrimitiveAttribute attribute,
		const EAttributeDomain domain,
		const uint64 faceID,
		const TSpan<math::Vector3R> out_values) const override
	{
		return m_inner().getAttribute(attribute, domain, faceID, out_values);
	}

	uint32 numMetadataSlots() const override
	{
		return m_inner().numMetadataSlots();
	}

	uint32 toMetadataSlot(const uint64 faceID) const override
	{
		return m_inner().toMetadataSlot(faceID);
	}

	const PrimitiveMetadata& getMetadata(const uint32 slot) const override
	{
		return m_inner().getMetadata(slot);
	}

private:
	[[PH_NO_UNIQUE_ADDRESS]]
	PrimitiveGetter m_inner;
};

}// end namespace ph
