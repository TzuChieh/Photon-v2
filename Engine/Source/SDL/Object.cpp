#include "SDL/Object.h"

namespace ph
{

Object::Object() = default;

Object::Object(const Object& other) = default;

Object::Object(Object&& other) noexcept = default;

Object::~Object() = default;

Object& Object::operator = (const Object& rhs) = default;

Object& Object::operator = (Object&& rhs) noexcept = default;

}// end namespace ph
