#include "Engine/Actor/Material/VolumeMaterial.h"

namespace ph
{

uint16 VolumeMaterial::getOverlapPriority() const
{
	return m_overlapPriority;
}

}// end namespace ph
