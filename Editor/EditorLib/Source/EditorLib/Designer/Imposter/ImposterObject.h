#pragma once

#include "EditorLib/Designer/FlatDesignerObject.h"

#include <Engine/SDL/sdl_interface.h>
#include <Common/logging.h>

#include <string>
#include <memory>

namespace ph::editor
{

PH_DECLARE_LOG_GROUP(ImposterObject);

class ImposterObject : public FlatDesignerObject
{
public:
	virtual bool bindDescription(
		const std::shared_ptr<ISdlResource>& descResource,
		const std::string& descName);

	virtual void unbindDescription();

	const std::string& getDescriptionName() const;
	ISdlResource* getDescription() const;

private:
	std::shared_ptr<ISdlResource> m_descResource;

	// SDL-binded fields
	std::string m_descName;

public:
	PH_DEFINE_SDL_CLASS(ImposterObject, clazz, outerScope=editor)
	{
		clazz.typeName("imposter");
		clazz.docName("Imposter Object");
		clazz.description(
			"Base of all imporster objects. Imposters are designer objects representing some render "
			"description resource.");
		clazz.baseOn<FlatDesignerObject>();

		TSdlString<OwnerType> descName("desc-name", &OwnerType::m_descName);
		descName.description("Name of the render description resource this object is representing.");
		clazz.addField(descName);
	}
};

}// end namespace ph::editor

#include "EditorLib/Designer/Imposter/ImposterObject.ipp"
