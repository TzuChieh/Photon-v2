#include "Designer/AbstractDesignerObject.h"

namespace ph::editor
{

AbstractDesignerObject::AbstractDesignerObject() = default;

AbstractDesignerObject::AbstractDesignerObject(const AbstractDesignerObject& other) = default;

AbstractDesignerObject::AbstractDesignerObject(AbstractDesignerObject&& other) = default;

AbstractDesignerObject::~AbstractDesignerObject() = default;

AbstractDesignerObject& AbstractDesignerObject::operator = (const AbstractDesignerObject& rhs) = default;

AbstractDesignerObject& AbstractDesignerObject::operator = (AbstractDesignerObject&& rhs) = default;

}// end namespace ph::editor
