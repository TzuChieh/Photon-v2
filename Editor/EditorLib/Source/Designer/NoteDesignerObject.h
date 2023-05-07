#pragma once

#include "Designer/FlatDesignerObject.h"

#include <SDL/sdl_interface.h>

#include <string>

namespace ph::editor
{

class NoteDesignerObject : public FlatDesignerObject
{
public:

private:
	std::string m_text;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<NoteDesignerObject>)
	{
		ClassType clazz("note-dobj");
		clazz.docName("Note Designer Object");
		clazz.description("Showing notes in text form in the scene.");
		clazz.baseOn<FlatDesignerObject>();

		TSdlString<OwnerType> text("text", &OwnerType::m_text);
		text.description("Notes in string.");
		clazz.addField(text);

		return clazz;
	}
};

}// end namespace ph::editor
