#include "Engine/World/Foundation/TransientVisualElement.h"
#include "Engine/Core/Intersection/Primitive.h"

#include <Common/assertion.h>

#include <type_traits>

namespace ph
{

void TransientVisualElement::add(const Primitive* const primitive)
{
	PH_ASSERT(primitive);
	intersectables.push_back(primitive);
	primitivesView.push_back(primitive);
}

void TransientVisualElement::addNonBlockingEmitterPrimitive(const Primitive* const primitive)
{
	PH_ASSERT(primitive);
	nonBlockingEmitterPrimitives.push_back(primitive);
	primitivesView.push_back(primitive);
}

TransientVisualElement& TransientVisualElement::add(const TransientVisualElement& other)
{
	intersectables.insert(
		intersectables.end(), 
		other.intersectables.begin(), 
		other.intersectables.end());

	surfaceEmitters.insert(
		surfaceEmitters.end(),
		other.surfaceEmitters.begin(),
		other.surfaceEmitters.end());

	nonBlockingEmitterPrimitives.insert(
		nonBlockingEmitterPrimitives.end(),
		other.nonBlockingEmitterPrimitives.begin(),
		other.nonBlockingEmitterPrimitives.end());

	primitivesView.insert(
		primitivesView.end(),
		other.primitivesView.begin(),
		other.primitivesView.end());

	return *this;
}

static_assert(std::is_copy_constructible_v<TransientVisualElement>,
	"TransientVisualElement should be copy constructible for easy data manipulation.");

}// end namespace ph
