#pragma once

#include "Designer/designer_fwd.h"

#include <SDL/Object.h>
#include <SDL/sdl_interface.h>

namespace ph::editor
{

class AbstractDesignerObject : public Object
{
public:
	AbstractDesignerObject();
	~AbstractDesignerObject() override = 0;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<AbstractDesignerObject>)
	{
		ClassType clazz("abstract-dobj");
		clazz.docName("Abstract Designer Object");
		clazz.description("A common base for designer-related object classes.");
		clazz.baseOn<Object>();
		return clazz;
	}
};

}// end namespace ph::editor
