#pragma once

#include "Core/SurfaceBehavior/surface_optics_fwd.h"
#include "Core/LTA/SidednessAgreement.h"

namespace ph
{

/*! @brief The environment a BSDF query is performed under.
*/
class BsdfQueryContext final
{
	// TODO: make this able to init query for multiple elementals or 
	// based on surface phenomenon (which can produce a phenomenon/bsdfQuery iterator for surface optics to consume)
public:
	SurfaceElemental        elemental = ALL_SURFACE_ELEMENTALS;
	ETransport              transport = ETransport::Radiance;
	lta::SidednessAgreement sidedness = lta::SidednessAgreement{lta::ESidednessPolicy::Strict};

	BsdfQueryContext() = default;
	explicit BsdfQueryContext(SurfaceElemental elemental);
	explicit BsdfQueryContext(lta::ESidednessPolicy sidednessPolicy);

	BsdfQueryContext(
		SurfaceElemental      elemental, 
		ETransport            transport, 
		lta::ESidednessPolicy sidednessPolicy = lta::ESidednessPolicy::Strict);
};

// In-header Implementations:

inline BsdfQueryContext::BsdfQueryContext(SurfaceElemental elemental)
	: BsdfQueryContext()
{
	this->elemental = elemental;
}

inline BsdfQueryContext::BsdfQueryContext(lta::ESidednessPolicy sidednessPolicy)
	: BsdfQueryContext()
{
	sidedness = lta::SidednessAgreement(sidednessPolicy);
}

inline BsdfQueryContext::BsdfQueryContext(
	const SurfaceElemental      elemental,
	const ETransport            transport,
	const lta::ESidednessPolicy sidednessPolicy)

	: elemental(elemental)
	, transport(transport)
	, sidedness(sidednessPolicy)
{}

}// end namespace ph
