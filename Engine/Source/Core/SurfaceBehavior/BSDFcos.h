#pragma once

namespace ph
{

class Intersection;
class Ray;
class SurfaceSample;
class Vector3f;

class BSDFcos
{
public:
	virtual ~BSDFcos() = 0;

	virtual void genImportanceSample(const Intersection& intersection, const Ray& ray, SurfaceSample* const out_sample) const = 0;
	virtual void evaluate(const Intersection& intersection, const Vector3f& L, const Vector3f& V, Vector3f* const out_value) const = 0;
};

}// end namespace ph