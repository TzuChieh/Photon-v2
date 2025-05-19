#pragma once

#include "Engine/Core/Intersection/Intersectable.h"

#include <vector>

namespace ph
{

class Intersectable;
class Emitter;
class Primitive;

/*! @brief A group of cooked data that represent the visible part of the scene at a specific time. 
This data block do not persist throughout the rendering process. After cooking is done, all cooked 
data should be properly interlinked and all `TransientVisualElement` instances will be cleaned up.
*/
class TransientVisualElement final
{
public:
	std::vector<const Intersectable*> intersectables;
	std::vector<const Emitter*> emitters;

	/*! @brief Represent the same shape as `intersectables`.
	Will be provided if obtaining such representation incurs no significant overhead (e.g., is a 
	byproduct during the build of intersectables). Otherwise, this view may not be available if not
	specifically requested. This view will cover all the shapes defined by `intersectables` if provided.
	*/
	std::vector<const Primitive*> primitivesView;

public:
	void add(const Primitive* primitive);

	TransientVisualElement& add(const TransientVisualElement& other);
};

}// end namespace ph
