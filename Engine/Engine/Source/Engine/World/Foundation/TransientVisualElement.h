#pragma once

#include "Engine/Core/Intersection/Intersectable.h"

#include <vector>

namespace ph
{

class Intersectable;
class SurfaceEmitter;
class Primitive;

/*! @brief A group of cooked data that represent the visible part of the scene at a specific time. 
This data block do not persist throughout the rendering process. After cooking is done, all cooked 
data should be properly interlinked and all `TransientVisualElement` instances will be cleaned up.
*/
class TransientVisualElement final
{
public:
	struct SurfaceEmitterUnit
	{
		const SurfaceEmitter* emitter = nullptr;

		/*! @brief Whether the light is nonblocking or directly invisible. */
		bool isNonPhysical = false;
	};

	std::vector<const Intersectable*> intersectables;

	/*! @brief Emission and sampling for all lights, including physical emissive surfaces.
	*/
	std::vector<SurfaceEmitterUnit> surfaceEmitters;
	
	/*! Shapes for the nonblocking light AS; these do not scatter or occlude rays. */
	std::vector<const Primitive*> nonBlockingEmitterPrimitives;

	/*! @brief A potentially incomplete primitive view of this visual element.
	Will be provided if obtaining such representation incurs no significant overhead (e.g., is a 
	byproduct during the build of intersectables). Otherwise, this view may not be available if not
	specifically requested.
	*/
	std::vector<const Primitive*> primitivesView;

public:
	void add(const Primitive* primitive);
	void addNonBlockingEmitterPrimitive(const Primitive* primitive);

	TransientVisualElement& add(const TransientVisualElement& other);
};

}// end namespace ph
