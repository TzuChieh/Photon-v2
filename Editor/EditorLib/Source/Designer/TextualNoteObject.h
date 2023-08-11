#pragma once

#include "Designer/FlatDesignerObject.h"
#include "Designer/Basic/ObjectTransform.h"

#include <SDL/sdl_interface.h>

#include <string>

namespace ph::editor
{

class TextualNoteObject : public FlatDesignerObject
{
public:

private:
	std::string m_text;
	ObjectTransform m_transform;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<TextualNoteObject>)
	{
		ClassType clazz("textual-note-dobj");
		clazz.docName("Textual Note Designer Object");
		clazz.description("Showing notes in text form in the scene.");
		clazz.baseOn<FlatDesignerObject>();

		TSdlString<OwnerType> text("text", &OwnerType::m_text);
		text.description("Notes in string.");
		clazz.addField(text);

		clazz.addStruct(&OwnerType::m_transform);

		return clazz;
	}
};

}// end namespace ph::editor
