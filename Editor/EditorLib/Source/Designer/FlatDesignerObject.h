#pragma once

#include "Designer/DesignerObject.h"

#include <SDL/sdl_interface.h>

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

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<FlatDesignerObject>)
	{
		ClassType clazz("flat-dobj");
		clazz.docName("Flat Designer Object");
		clazz.description("A designer object that does not allow any children.");
		clazz.baseOn<DesignerObject>();
		return clazz;
	}
};

}// end namespace ph::editor
