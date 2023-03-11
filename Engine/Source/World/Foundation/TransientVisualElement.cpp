#include "World/Foundation/TransientVisualElement.h"

namespace ph
{

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

}// end namespace ph
