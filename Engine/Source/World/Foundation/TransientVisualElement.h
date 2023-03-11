#pragma once

#include "Core/Intersectable/Intersectable.h"

#include <vector>
#include <memory>

namespace ph
{

class Intersectable;
class Emitter;

/*! @brief A group of cooked data that represent the visible part of the scene at a specific time. 
This data block do not persist throughout the rendering process. After cooking is done, all cooked 
data should be properly interlinked and `TransientVisualElement` will be cleaned up.
*/
class TransientVisualElement final
{
public:
	std::vector<const Intersectable*> intersectables;
	std::vector<const Emitter*> emitters;

	// DEPRECATED
	std::unique_ptr<Emitter> emitter;

public:
	TransientVisualElement& add(const TransientVisualElement& other);
};

}// end namespace ph
