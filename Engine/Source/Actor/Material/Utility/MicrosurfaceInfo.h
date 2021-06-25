#pragma once

#include "Common/primitive_type.h"

#include <memory>

namespace ph
{

class Microfacet;

class MicrosurfaceInfo final
{
public:
	MicrosurfaceInfo();

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
