#pragma once

#include <vector>
#include <memory>

namespace ph
{

class Intersection;
class Ray;
class Model;
class Primitive;

class World final
{
public:
	void addModel(const Model& model);
	bool isIntersecting(const Ray& ray, Intersection* out_intersection) const;

	void cook();

private:
	std::vector<Model> m_models;
	std::vector<std::unique_ptr<Primitive>> m_primitives;
};

}// end namespace ph