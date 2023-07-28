#pragma once

#include "Designer/FlatDesignerObject.h"

#include <SDL/sdl_interface.h>
#include <Common/logging.h>

#include <string>
#include <memory>

namespace ph::editor
{

PH_DECLARE_LOG_GROUP(ImposterObject);

class ImposterObject : public FlatDesignerObject
{
public:
	virtual bool bindTarget(
		const std::shared_ptr<ISdlResource>& resource,
		const std::string& targetName);

	virtual void unbindTarget();

	const std::string& getTargetName() const;

private:
	// SDL-binded fields:
	std::string m_targetName;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<ImposterObject>)
	{
		ClassType clazz("imposter");
		clazz.docName("Imposter Object");
		clazz.description(
			"Base of all imporster objects. Imposters are designer objects representing some render "
			"description resource.");
		clazz.baseOn<FlatDesignerObject>();

		TSdlString<OwnerType> targetName("target-name", &OwnerType::m_targetName);
		targetName.description("Name of the render description resource this object is representing.");
		clazz.addField(targetName);

		return clazz;
	}
};

}// end namespace ph::editor

#include "Designer/Imposter/ImposterObject.ipp"
