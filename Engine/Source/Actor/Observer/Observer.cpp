#include "Actor/Observer/Observer.h"

namespace ph
{

Observer::Observer() = default;

Observer::Observer(const Observer& other) = default;

Observer& Observer::operator = (const Observer& rhs)
{
	return *this;
}

void swap(Observer& first, Observer& second)
{}

}// end namespace ph
