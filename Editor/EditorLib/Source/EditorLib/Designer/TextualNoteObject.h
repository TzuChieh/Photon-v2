#pragma once

#include "EditorLib/Designer/FlatDesignerObject.h"
#include "EditorLib/Designer/Basic/ObjectTransform.h"

#include <Engine/SDL/sdl_interface.h>

#include <string>

namespace ph::editor
{

class TextualNoteObject : public FlatDesignerObject
{
public:
	math::TDecomposedTransform<real> getLocalToParent() const override;
	void setLocalToParent(const math::TDecomposedTransform<real>& transform) override;

private:
	std::string m_text;
	ObjectTransform m_textTransform;

public:
	PH_DEFINE_SDL_CLASS(TextualNoteObject, clazz, outerScope=editor)
	{
		clazz.typeName("textual-note-dobj");
		clazz.docName("Textual Note Designer Object");
		clazz.description("Showing notes in text form in the scene.");
		clazz.baseOn<FlatDesignerObject>();

		TSdlString<OwnerType> text("text", &OwnerType::m_text);
		text.description("Notes in string.");
		clazz.addField(text);

		clazz.addStruct(&OwnerType::m_textTransform);
	}
};

}// end namespace ph::editor
