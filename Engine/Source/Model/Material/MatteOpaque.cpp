#include "Model/Material/MatteOpaque.h"

namespace ph
{

MatteOpaque::MatteOpaque() : 
	m_surfaceIntegrand(this), m_albedo(std::make_shared<ConstantTexture>(Vector3f(1, 1, 1)))
{

}

MatteOpaque::~MatteOpaque() = default;

}// end namespace ph