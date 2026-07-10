#pragma once

#include "Engine/Core/SurfaceBehavior/Property/surface_property.h"
#include "Engine/Core/SurfaceBehavior/SurfaceOptics/LaurentBelcour/LbLayerProperty.h"
#include "Engine/Math/Color/Spectrum.h"

#include <Common/primitive_type.h>

#include <utility>

namespace ph
{

template<
	typename Alpha,
	typename IorN,
	typename IorK,
	typename Depth,
	typename PhaseG,
	typename SigmaA,
	typename SigmaS>
class TLbLayerProperty final : public LbLayerProperty
{
	static_assert(CSurfaceProperty<Alpha, real>,
		"Alpha must accept SurfaceHit and return a real-convertible value.");
	static_assert(CSurfaceProperty<IorN, math::Spectrum>,
		"IorN must accept SurfaceHit and return a spectrum-convertible value.");
	static_assert(CSurfaceProperty<IorK, math::Spectrum>,
		"IorK must accept SurfaceHit and return a spectrum-convertible value.");
	static_assert(CSurfaceProperty<Depth, real>,
		"Depth must accept SurfaceHit and return a real-convertible value.");
	static_assert(CSurfaceProperty<PhaseG, real>,
		"PhaseG must accept SurfaceHit and return a real-convertible value.");
	static_assert(CSurfaceProperty<SigmaA, math::Spectrum>,
		"SigmaA must accept SurfaceHit and return a spectrum-convertible value.");
	static_assert(CSurfaceProperty<SigmaS, math::Spectrum>,
		"SigmaS must accept SurfaceHit and return a spectrum-convertible value.");

public:
	TLbLayerProperty(
		Alpha alpha,
		IorN iorN,
		IorK iorK,
		Depth depth,
		PhaseG phaseG,
		SigmaA sigmaA,
		SigmaS sigmaS)
		: m_alpha(std::move(alpha))
		, m_iorN(std::move(iorN))
		, m_iorK(std::move(iorK))
		, m_depth(std::move(depth))
		, m_phaseG(std::move(phaseG))
		, m_sigmaA(std::move(sigmaA))
		, m_sigmaS(std::move(sigmaS))
	{}

	LbLayer evaluate(
		const SurfaceHit& X,
		const LbLayer& previousLayer) const override
	{
		const real depth = m_depth(X);
		if(depth == 0.0_r)
		{
			return LbLayer(
				m_alpha(X),
				m_iorN(X),
				m_iorK(X));
		}
		else
		{
			return LbLayer(
				m_phaseG(X),
				depth,
				m_sigmaA(X),
				m_sigmaS(X),
				previousLayer);
		}
	}

private:
	Alpha m_alpha;
	IorN m_iorN;
	IorK m_iorK;
	Depth m_depth;
	PhaseG m_phaseG;
	SigmaA m_sigmaA;
	SigmaS m_sigmaS;
};

}// end namespace ph
