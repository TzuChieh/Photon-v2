#pragma once

namespace ph
{

class SurfaceIntegrand;

class Material
{
public:
	virtual ~Material() = 0;

	virtual const SurfaceIntegrand* getSurfaceIntegrand() const = 0;
};

}// end namespace ph