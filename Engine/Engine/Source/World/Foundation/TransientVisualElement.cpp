#include "World/Foundation/TransientVisualElement.h"
#include "Core/Intersectable/Primitive.h"

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

TransientVisualElement& TransientVisualElement::add(const TransientVisualElement& other)
{
	intersectables.insert(
		intersectables.end(), 
		other.intersectables.begin(), 
		other.intersectables.end());

	emitters.insert(
		emitters.end(),
		other.emitters.begin(),
		other.emitters.end());

	return *this;
}

static_assert(std::is_copy_constructible_v<TransientVisualElement>,
	"TransientVisualElement should be copy constructible for easy data manipulation.");

}// end namespace ph
