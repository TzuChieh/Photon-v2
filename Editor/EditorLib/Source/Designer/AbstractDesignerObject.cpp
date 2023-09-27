#include "Designer/AbstractDesignerObject.h"
#include "Designer/UI/UIPropertyLayout.h"

namespace ph::editor
{

AbstractDesignerObject::AbstractDesignerObject()
	: m_state()
	, m_sceneStorageIndex(static_cast<uint64>(-1))
{}

AbstractDesignerObject::AbstractDesignerObject(const AbstractDesignerObject& other) = default;

AbstractDesignerObject::AbstractDesignerObject(AbstractDesignerObject&& other) noexcept = default;

AbstractDesignerObject::~AbstractDesignerObject() = default;

UIPropertyLayout AbstractDesignerObject::layoutProperties()
{
	return UIPropertyLayout::makeLinearListing(this, "SDL Properties");
}

AbstractDesignerObject& AbstractDesignerObject::operator = (const AbstractDesignerObject& rhs) = default;

AbstractDesignerObject& AbstractDesignerObject::operator = (AbstractDesignerObject&& rhs) noexcept = default;

}// end namespace ph::editor
