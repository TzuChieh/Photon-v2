#pragma once

#include "Designer/DesignerObject.h"

namespace ph::editor
{

class FlatDesignerObject : public DesignerObject
{
public:
	TSpanView<DesignerObject*> getChildren() const override;
	bool canHaveChildren() const override;

private:
	DesignerObject* addChild(DesignerObject* childObj) override;
	bool removeChild(DesignerObject* childObj) override;
};

}// end namespace ph::editor
