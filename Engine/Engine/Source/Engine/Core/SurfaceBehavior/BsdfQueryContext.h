#pragma once

#include "Engine/Core/SurfaceBehavior/surface_optics_fwd.h"
#include "Engine/Core/LTA/SidednessAgreement.h"
#include "Engine/Core/LTA/enums.h"

#include <initializer_list>

namespace ph
{

/*! @brief The environment a BSDF query is performed under.
*/
class BsdfQueryContext final
{
public:
	/*! The elemental in `targetPhenomena` that will be involved. */
	SurfaceElemental elemental = ALL_SURFACE_ELEMENTALS;

	/*! The phenomena that will be involved. */
	SurfacePhenomena targetPhenomena = ALL_SURFACE_PHENOMENA;

	lta::ETransport transport = lta::ETransport::Radiance;
	lta::SidednessAgreement sidedness = lta::SidednessAgreement{lta::ESidednessPolicy::Strict};

	BsdfQueryContext() = default;
	explicit BsdfQueryContext(SurfaceElemental elemental);
	explicit BsdfQueryContext(lta::ESidednessPolicy sidednessPolicy);

	/*! @brief Creates an elemental-based context.
	*/
	BsdfQueryContext(
		SurfaceElemental elemental,
		lta::ETransport transport, 
		lta::ESidednessPolicy sidednessPolicy = lta::ESidednessPolicy::Strict);

	/*! @brief Creates a phenomenon-based context.
	*/
	BsdfQueryContext(
		SurfacePhenomena targetPhenomena,
		lta::ETransport transport,
		lta::ESidednessPolicy sidednessPolicy = lta::ESidednessPolicy::Strict);

	/*! @brief Creates a phenomenon-based context.
	*/
	BsdfQueryContext(
		std::initializer_list<ESurfacePhenomenon> targetPhenomena,
		lta::ETransport transport,
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
	this->sidedness = lta::SidednessAgreement(sidednessPolicy);
}

inline BsdfQueryContext::BsdfQueryContext(
	SurfaceElemental elemental,
	lta::ETransport transport,
	lta::ESidednessPolicy sidednessPolicy)

	: elemental(elemental)
	, targetPhenomena(ALL_SURFACE_PHENOMENA)
	, transport(transport)
	, sidedness(sidednessPolicy)
{}

inline BsdfQueryContext::BsdfQueryContext(
	SurfacePhenomena targetPhenomena,
	lta::ETransport transport,
	lta::ESidednessPolicy sidednessPolicy)

	: elemental(ALL_SURFACE_ELEMENTALS)
	, targetPhenomena(targetPhenomena)
	, transport(transport)
	, sidedness(sidednessPolicy)
{}

inline BsdfQueryContext::BsdfQueryContext(
	std::initializer_list<ESurfacePhenomenon> targetPhenomena,
	lta::ETransport transport,
	lta::ESidednessPolicy sidednessPolicy)

	: BsdfQueryContext(
		SurfacePhenomena(targetPhenomena),
		transport,
		sidednessPolicy)
{}

}// end namespace ph
