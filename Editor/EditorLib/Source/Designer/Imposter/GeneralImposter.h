#pragma once

#include "Designer/Imposter/ImposterObject.h"

#include <SDL/sdl_interface.h>

#include <string>

namespace ph::editor
{

class GeneralImposter : public ImposterObject
{
public:
	using Base = ImposterObject;

	bool bindTarget(
		const std::shared_ptr<ISdlResource>& resource,
		const std::string& targetName) override;

	void unbindTarget() override;

private:
	std::shared_ptr<ISdlResource> m_targetResource;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<GeneralImposter>)
	{
		ClassType clazz("general-imposter");
		clazz.docName("General Imposter");
		clazz.description(
			"An object that can represent any render description resource. This is a good fallback "
			"if a more specific imposter type does not present.");
		clazz.baseOn<ImposterObject>();

		return clazz;
	}
};

}// end namespace ph::editor
