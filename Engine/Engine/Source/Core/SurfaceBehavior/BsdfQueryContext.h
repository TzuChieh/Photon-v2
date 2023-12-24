#pragma once

#include "Core/SurfaceBehavior/surface_optics_fwd.h"
#include "Core/LTABuildingBlock/SidednessAgreement.h"

namespace ph
{

// TODO: make this able to init query for multiple elementals or 
// based on surface phenomenon (which can produce a phenomenon/bsdfQuery iterator for surface optics to consume)
class BsdfQueryContext final
{
public:
	SurfaceElemental   elemental;
	ETransport         transport;
	SidednessAgreement sidedness;

	BsdfQueryContext();
	explicit BsdfQueryContext(SurfaceElemental elemental);

	BsdfQueryContext(
		SurfaceElemental elemental, 
		ETransport       transport, 
		ESidednessPolicy sidednessPolicy = ESidednessPolicy::Strict);
};

// In-header Implementations:

inline BsdfQueryContext::BsdfQueryContext() :
	BsdfQueryContext(ALL_ELEMENTALS)
{}

inline BsdfQueryContext::BsdfQueryContext(SurfaceElemental elemental) :
	BsdfQueryContext(elemental, ETransport::Radiance)
{}

inline BsdfQueryContext::BsdfQueryContext(
	const SurfaceElemental elemental,
	const ETransport       transport,
	const ESidednessPolicy sidednessPolicy) : 

	elemental(elemental),
	transport(transport),
	sidedness(sidednessPolicy)
{}

}// end namespace ph
