#pragma once

#include "Common/primitive_type.h"
#include "Core/Quantity/Spectrum.h"

#include <memory>
#include <variant>

namespace ph
{

class ConductorFresnel;

/*! @brief Data describing the effects when light hits an conductive interface.

Contains data and procedures for generating fresnel effects of 
conductor-dielectric interface.
*/
class ConductiveInterfaceInfo final
{
public:
	ConductiveInterfaceInfo();

	std::unique_ptr<ConductorFresnel> genFresnelEffect() const;

private:
	struct FullIor
	{
		real     outer;
		Spectrum innerN;
		Spectrum innerK;
	};

	struct SchlickIor
	{
		Spectrum f0;
	};

	bool                              m_useExact;
	std::variant<FullIor, SchlickIor> m_ior;
};

}// end namespace ph
