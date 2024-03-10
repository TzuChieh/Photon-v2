#pragma once

#include "Core/Intersection/Primitive.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/HitProbe.h"
#include "Utility/traits.h"

#include <Common/assertion.h>

#include <concepts>
#include <utility>

namespace ph
{

namespace detail
{

template<typename GetterType>
concept CPrimitiveMetaGetter = requires (const GetterType getter)
{
	{ getter() } -> std::same_as<const PrimitiveMetadata*>;
};

template<typename GetterType>
concept CPrimitiveGetter = requires (const GetterType getter)
{
	{ getter() } -> std::convertible_to<const Primitive*>;
};

}// end namespace detail

struct ReferencedPrimitiveMetaGetter final
{
	const PrimitiveMetadata* metadata;

	explicit ReferencedPrimitiveMetaGetter(const PrimitiveMetadata* const metadata)
		: metadata(metadata)
	{}

	const PrimitiveMetadata* operator () () const
	{
		return metadata;
	}
};

struct EmbeddedPrimitiveMetaGetter final
{
	PrimitiveMetadata metadata;

	template<typename... DeducedArgs>
	explicit EmbeddedPrimitiveMetaGetter(DeducedArgs&&... args)
		: metadata(std::forward<DeducedArgs>(args)...)
	{}

	const PrimitiveMetadata* operator () () const
	{
		return &metadata;
	}
};

template<CDerived<Primitive> PrimitiveType>
struct TReferencedPrimitiveGetter final
{
	const PrimitiveType* primitive;

	explicit TReferencedPrimitiveGetter(const PrimitiveType* const primitive)
		: primitive(primitive)
	{}

	const PrimitiveType* operator () () const
	{
		return primitive;
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

	const PrimitiveType* operator () () const
	{
		return &primitive;
	}
};

// TODO: could use EBO on some cases

template<detail::CPrimitiveMetaGetter PrimitiveMetaGetter, detail::CPrimitiveGetter PrimitiveGetter>
class TMetaInjectionPrimitive : public Primitive
{
public:
	TMetaInjectionPrimitive(PrimitiveMetaGetter metaGetter, PrimitiveGetter primitiveGetter)
		: Primitive()
		, m_metaGetter(std::move(metaGetter))
		, m_primitiveGetter(std::move(primitiveGetter))
	{}

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override
	{
		if(m_primitiveGetter()->isIntersecting(ray, probe))
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
		HitProbe& srcProbe) const override
	{
		PH_ASSERT(srcProbe.getCurrentHit() == this);
		srcProbe.popHit();

		if(srcProbe.getCurrentHit()->reintersect(ray, probe, srcRay, srcProbe))
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
		HitDetail* const out_detail) const override
	{
		// If failed, it is likely to be caused by: 1. mismatched/missing probe push or pop in
		// the hit stack; 2. the hit event is invalid
		PH_ASSERT(probe.getCurrentHit() == this);
		probe.popHit();

		// Current hit is not necessary the injectee (as obtained via `getInjectee()`). For example,
		// if the injectee contains multiple instances then it could simply skip over to one of them.
		probe.getCurrentHit()->calcHitDetail(ray, probe, out_detail);

		// This is a representative of the original primitive
		out_detail->setHitIntrinsics(
			this,
			out_detail->getUVW(),
			out_detail->getRayT(),
			out_detail->getFaceID(),
			out_detail->getFaceTopology());
	}

	math::AABB3D calcAABB() const override
	{
		return m_primitiveGetter()->calcAABB();
	}

	bool isOccluding(const Ray& ray) const override
	{
		return m_primitiveGetter()->isOccluding(ray);
	}

	bool mayOverlapVolume(const math::AABB3D& volume) const override
	{
		return m_primitiveGetter()->mayOverlapVolume(volume);
	}

	void genPosSample(
		PrimitivePosSampleQuery& query, 
		SampleFlow& sampleFlow,
		HitProbe& probe) const override
	{
		m_primitiveGetter()->genPosSample(query, sampleFlow, probe);

		// Hit detail will be modified by this primitive
		probe.pushIntermediateHit(this);
	}

	void calcPosSamplePdfA(
		PrimitivePosSamplePdfQuery& query,
		HitProbe& probe) const override
	{
		m_primitiveGetter()->calcPosSamplePdfA(query, probe);

		// Hit detail will be modified by this primitive
		probe.pushIntermediateHit(this);
	}

	real calcExtendedArea() const override
	{
		return m_primitiveGetter()->calcExtendedArea();
	}

	bool uvwToPosition(
		const math::Vector3R& uvw,
		math::Vector3R* const out_position) const override
	{
		return m_primitiveGetter()->uvwToPosition(uvw, out_position);
	}

	const PrimitiveMetadata* getMetadata() const override
	{
		// Metadata from `m_primitiveGetter()->getMetadata()` (if any) is intentionally overridden
		// by the injected one
		return m_metaGetter();
	}

	/*! @brief Gets the primitive that has got metadata injected.
	@return Pointer to the primitive carried by `PrimitiveGetter`.
	*/
	const auto* getInjectee() const
	{
		return m_primitiveGetter();
	}

private:
	PrimitiveMetaGetter m_metaGetter;
	PrimitiveGetter m_primitiveGetter;
};

}// end namespace ph
