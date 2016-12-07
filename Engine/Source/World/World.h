#pragma once

#include "Common/primitive_type.h"
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
	void update(const float32 deltaS);

	const Intersector& getIntersector() const;

private:
	std::vector<Model> m_models;

	std::unique_ptr<Intersector> m_intersector;

	static void updateIntersector(Intersector* const out_intersector, const std::vector<Model>& models);
	static void discretizeModelGeometry(const Model& model, std::vector<Triangle>* const out_triangles);
};

}// end namespace ph