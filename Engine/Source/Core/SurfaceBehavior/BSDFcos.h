#pragma once

namespace ph
{

class Intersection;
class Ray;
class SurfaceSample;

class BSDFcos
{
public:
	virtual ~BSDFcos() = 0;

	virtual void genImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const = 0;
};

}// end namespace ph