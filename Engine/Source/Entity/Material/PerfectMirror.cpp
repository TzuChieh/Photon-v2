#include "Entity/Material/PerfectMirror.h"

namespace ph
{

PerfectMirror::~PerfectMirror() = default;

const SurfaceBehavior* PerfectMirror::getSurfaceBehavior() const
{
	return &m_surfaceBehavior;
}

}// end namespace ph