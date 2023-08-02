#include "Designer/AbstractDesignerObject.h"
#include "Designer/UI/UIPropertyLayout.h"

namespace ph::editor
{

AbstractDesignerObject::AbstractDesignerObject() = default;

AbstractDesignerObject::AbstractDesignerObject(const AbstractDesignerObject& other) = default;

AbstractDesignerObject::AbstractDesignerObject(AbstractDesignerObject&& other) noexcept = default;

AbstractDesignerObject::~AbstractDesignerObject() = default;

UIPropertyLayout AbstractDesignerObject::layoutProperties()
{
	return UIPropertyLayout{};
}

AbstractDesignerObject& AbstractDesignerObject::operator = (const AbstractDesignerObject& rhs) = default;

AbstractDesignerObject& AbstractDesignerObject::operator = (AbstractDesignerObject&& rhs) noexcept = default;

}// end namespace ph::editor
