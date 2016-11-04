#include "Model/Material/AbradedOpaque.h"

namespace ph
{

AbradedOpaque::AbradedOpaque() : 
	m_surfaceIntegrand(this), m_roughness(0.5f), m_f0(0.04f)
{

}

AbradedOpaque::~AbradedOpaque() = default;

}// end namespace ph