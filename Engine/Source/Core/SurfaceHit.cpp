#include "Core/SurfaceHit.h"

namespace ph
{
	
SurfaceHit SurfaceHit::switchChannel(const uint32 newChannel) const
{
	HitProbe newProbe = m_recordedProbe;
	newProbe.setChannel(newChannel);
	return SurfaceHit(m_incidentRay, newProbe);
}

}// end namespace ph