#pragma once

#include "Designer/Imposter/ImposterObject.h"

#include <SDL/sdl_interface.h>

#include <string>

namespace ph::editor
{

class GeneralImposter : public ImposterObject
{
public:

private:
	//std::string m_text;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<GeneralImposter>)
	{
		ClassType clazz("general-imposter");
		clazz.docName("General Imposter");
		clazz.description(
			"An object that can represent any render description resource. This is a good fallback "
			"if a more specific imposter type does not present.");
		clazz.baseOn<ImposterObject>();

		/*TSdlString<OwnerType> text("text", &OwnerType::m_text);
		text.description("Notes in string.");
		clazz.addField(text);*/

		return clazz;
	}
};

}// end namespace ph::editor
