#include "SDL/Object.h"

namespace ph
{

Object::Object() = default;

Object::Object(const Object& other) = default;

Object::Object(Object&& other) = default;

Object::~Object() = default;

Object& Object::operator = (const Object& rhs) = default;

Object& Object::operator = (Object&& rhs) = default;

}// end namespace ph
