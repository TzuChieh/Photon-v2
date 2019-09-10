#pragma once

#include "Common/primitive_type.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>
#include <variant>

namespace ph
{

class InputPacket;
class DielectricFresnel;

/*! @brief Data describing the effects when light hits an dielectric interface.

Contains data and procedures for generating fresnel effects of 
dielectric-dielectric interface.
*/
class DielectricInterfaceInfo
{
public:
	DielectricInterfaceInfo();
	explicit DielectricInterfaceInfo(const InputPacket& packet);

	std::unique_ptr<DielectricFresnel> genFresnelEffect() const;

private:
	bool m_useExact;
	real m_iorOuter;
	real m_iorInner;
};

}// end namespace ph
