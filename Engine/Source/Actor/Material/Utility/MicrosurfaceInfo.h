#pragma once

#include "Common/primitive_type.h"
#include "Actor/Material/Utility/EInterfaceMicrosurface.h"
#include "DataIO/SDL/sdl_interface.h"

#include <memory>
#include <optional>

namespace ph
{

class Microfacet;

class MicrosurfaceInfo final
{
public:
	inline MicrosurfaceInfo() = default;

	std::unique_ptr<Microfacet> genMicrofacet() const;

	bool isIsotropic() const;

private:
	EInterfaceMicrosurface m_microsurface;
	real                   m_roughnessU;
	std::optional<real>    m_roughnessV;
};

// In-header Implementations:

inline bool MicrosurfaceInfo::isIsotropic() const
{
	return !m_roughnessV.has_value();
}

}// end namespace ph
