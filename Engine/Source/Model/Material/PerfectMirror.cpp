#include "Model/Material/PerfectMirror.h"

namespace ph
{

PerfectMirror::~PerfectMirror() = default;

const SurfaceIntegrand* PerfectMirror::getSurfaceIntegrand() const
{
	return &m_surfaceIntegrand;
}

}// end namespace ph