#pragma once

#include "Common/primitive_type.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>
#include <variant>

namespace ph
{

class InputPacket;
class ConductorFresnel;

/*! @brief Data describing the effects when light hits an conductive interface.

Contains data and procedures for generating fresnel effects of 
conductor-dielectric interface.
*/
class ConductiveInterfaceInfo
{
public:
	ConductiveInterfaceInfo();
	explicit ConductiveInterfaceInfo(const InputPacket& packet);

	std::unique_ptr<ConductorFresnel> genFresnelEffect() const;

private:
	struct FullIor
	{
		real             outer;
		SpectralStrength innerN;
		SpectralStrength innerK;
	};

	struct SchlickIor
	{
		SpectralStrength f0;
	};

	bool                              m_useExact;
	std::variant<FullIor, SchlickIor> m_ior;
};

}// end namespace ph
