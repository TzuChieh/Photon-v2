#pragma once

#include "Designer/Imposter/ImposterObject.h"
#include "Designer/Basic/ObjectTransform.h"

#include <SDL/sdl_interface.h>

#include <string>

namespace ph::editor
{

class GeneralImposter : public ImposterObject
{
public:
	using Base = ImposterObject;

	bool bindDescription(
		const std::shared_ptr<ISdlResource>& descResource,
		const std::string& descName) override;

	void unbindDescription() override;

	UIPropertyLayout layoutProperties() override;
	math::TDecomposedTransform<real> getLocalToParent() const override;
	void setLocalToParent(const math::TDecomposedTransform<real>& transform) override;

private:
	std::shared_ptr<ISdlResource> m_descResource;

	// SDL-binded fields:
	ObjectTransform m_imposterTransform;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<GeneralImposter>)
	{
		ClassType clazz("general-imposter");
		clazz.docName("General Imposter");
		clazz.description(
			"An object that can represent any render description resource. This is a good fallback "
			"if a more specific imposter type does not present.");
		clazz.baseOn<ImposterObject>();

		clazz.addStruct(&OwnerType::m_imposterTransform);

		return clazz;
	}
};

}// end namespace ph::editor
