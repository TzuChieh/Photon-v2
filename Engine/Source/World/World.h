#pragma once

#include "Model/Geometry/Triangle.h"
#include "Model/Model.h"
#include "Intersector.h"

#include <vector>
#include <memory>

namespace ph
{

class Intersection;
class Ray;
class Triangle;

class World final
{
public:
	World();

	void addModel(const Model& model);
	bool isIntersecting(const Ray& ray, Intersection* out_intersection) const;

	void cook();

private:
	std::vector<Model> m_models;
	std::vector<Triangle> m_triangles;

	std::unique_ptr<Intersector> m_intersector;
};

}// end namespace ph