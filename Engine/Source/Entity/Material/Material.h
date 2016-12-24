#pragma once

namespace ph
{

class SurfaceBehavior;

class Material
{
public:
	virtual ~Material() = 0;

	virtual const SurfaceBehavior* getSurfaceBehavior() const = 0;
};

}// end namespace ph