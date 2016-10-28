#pragma once

namespace ph
{

class SurfaceIntegrand;

class Material
{
public:
	virtual ~Material() = 0;

	virtual const SurfaceIntegrand* getSurfaceIntegrand() = 0;
};

}// end namespace ph