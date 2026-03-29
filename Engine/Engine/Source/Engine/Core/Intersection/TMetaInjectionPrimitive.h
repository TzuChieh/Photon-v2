#pragma once

#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
#include "Engine/Core/HitProbe.h"
#include "Engine/Utility/traits.h"

#include <Common/assertion.h>

#include <concepts>
#include <utility>
#include <array>

namespace ph
{

namespace detail
{

template<typename GetterType>
concept CPrimitiveMetaGetter = requires (const GetterType getter, uint32 slot)
{
	{ getter(slot) } -> std::same_as<const PrimitiveMetadata&>;
};

template<typename GetterType>
concept CPrimitiveGetter = requires (const GetterType getter)
{
	{ getter() } -> std::convertible_to<const Primitive&>;
};

}// end namespace detail

struct ReferencedPrimitiveMetaGetter final
{
	const PrimitiveMetadata* metadata;

	explicit ReferencedPrimitiveMetaGetter(const PrimitiveMetadata* const metadata)
		: metadata(metadata)
	{}

	const PrimitiveMetadata& operator () (uint32 /* slot */) const
	{
		PH_ASSERT(metadata);
		return *metadata;
	}
};

struct EmbeddedPrimitiveMetaGetter final
{
	PrimitiveMetadata metadata;

	template<typename... DeducedArgs>
	explicit EmbeddedPrimitiveMetaGetter(DeducedArgs&&... args)
		: metadata(std::forward<DeducedArgs>(args)...)
	{}

	const PrimitiveMetadata& operator () (uint32 /* slot */) const
	{
		return metadata;
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

template<typename PrimitiveMetaGetter, typename PrimitiveGetter>
class TMetaInjectionPrimitive : public Primitive
{
	static_assert(detail::CPrimitiveMetaGetter<PrimitiveMetaGetter>,
		"Input type does not fulfill the requirements of a PrimitiveMetaGetter.");
	static_assert(detail::CPrimitiveGetter<PrimitiveGetter>,
		"Input type does not fulfill the requirements of a PrimitiveGetter.");

	// TODO: could use EBO on some cases

public:
	TMetaInjectionPrimitive(PrimitiveMetaGetter metaGetter, PrimitiveGetter primitiveGetter)
		: Primitive()
		, m_metaGetter(std::move(metaGetter))
		, m_primitiveGetter(std::move(primitiveGetter))
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
		return m_primitiveGetter().numMetadataSlots();
	}

	uint32 toMetadataSlot(const uint64 faceID) const override final
	{
		return m_primitiveGetter().toMetadataSlot(faceID);
	}

	const PrimitiveMetadata& getMetadata(const uint32 slot) const override final
	{
		// Metadata from `m_primitiveGetter()->getMetadata()` (if any) is intentionally overridden
		// by the injected one
		return m_metaGetter(slot);
	}

	/*! @brief Gets the primitive that has got metadata injected.
	@return Pointer to the primitive carried by `PrimitiveGetter`.
	*/
	const auto& getInjectee() const
	{
		return m_primitiveGetter();
	}

private:
	PrimitiveMetaGetter m_metaGetter;
	PrimitiveGetter m_primitiveGetter;
};

}// end namespace ph
