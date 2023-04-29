#include "Designer/FlatDesignerObject.h"

namespace ph::editor
{

TSpanView<DesignerObject*> FlatDesignerObject::getChildren() const
{
	return {};
}

bool FlatDesignerObject::canHaveChildren() const
{
	return false;
}

DesignerObject* FlatDesignerObject::addChild(DesignerObject* /* childObj */)
{
	return nullptr;
}

bool FlatDesignerObject::removeChild(DesignerObject* /* childObj */)
{
	return false;
}

}// end namespace ph::editor
