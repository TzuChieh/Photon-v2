#pragma once

#include "Common/primitive_type.h"

#include <memory>

namespace ph
{

class InputPacket;
class Microfacet;

class MicrosurfaceInfo
{
public:
	MicrosurfaceInfo();
	explicit MicrosurfaceInfo(const InputPacket& packet);

	std::unique_ptr<Microfacet> genMicrofacet() const;

	bool isIsotropic() const;

private:
	enum class EType
	{
		TROWBRIDGE_REITZ,// a.k.a. GGX
		BECKMANN
	};

	EType m_type;
	real  m_alphaU;
	real  m_alphaV;
};

// In-header Implementations:

inline bool MicrosurfaceInfo::isIsotropic() const
{
	return m_alphaU == m_alphaV;
}

}// end namespace ph

/*
	<SDL_struct>

	<type_name> microsurface-info </type_name>

	<input name="distribution-model" type="string">
		<description>
			Possible value are "trowbridge-reitz" (or equivalently "ggx", 
			for both isotropic and anisotropic surface appearances), 
			and "beckmann" (isotropic only).
		</description>
	</input>
	<input name="roughness" type="real">
		<description>
			Isotropic surface roughness in [0, 1], the material will appear
			to be smoother with smaller roughness value.
		</description>
	</input>
	<input name="roughness-u" type="real">
		<description>
			Similar to the roughness parameter, but is used for anisotropic
			surface appearances. This value controls the U component of
			surface roughness.
		</description>
	</input>
	<input name="roughness-v" type="real">
		<description>
			Similar to the roughness parameter, but is used for anisotropic
			surface appearances. This value controls the V component of
			surface roughness.
		</description>
	</input>
	<input name="mapping" type="string">
		<description>
			Method for transforming roughness value into an internal parameter
			"alpha". Possible values are "squared", "pbrt-v3", and "equaled".
		</description>
	</input>

	</SDL_struct>
*/
