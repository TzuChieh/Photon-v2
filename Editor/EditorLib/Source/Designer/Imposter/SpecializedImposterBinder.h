#pragma once

#include <memory>
#include <string>

namespace ph { class ISdlResource; }
namespace ph { class SdlClass; }

namespace ph::editor
{

class DesignerScene;
class DesignerObject;
class ImposterObject;

class SpecializedImposterBinder final
{
public:
	explicit SpecializedImposterBinder(DesignerScene& scene);

	/*! @brief Given a description resource and its name, create an imposter object for it.
	*/
	ImposterObject* newImposter(
		const std::shared_ptr<ISdlResource>& descResource,
		const std::string& descName);

	/*! @brief Given a description resource type, create an imposter object for it.
	The binded description resource will be created automatically.
	*/
	ImposterObject* newImposter(const SdlClass* descClass);

private:
	static bool isDescriptionClass(const SdlClass* clazz);
	static const SdlClass* getImposterClass(const SdlClass* descClass);

	DesignerScene& m_scene;
};

}// end namespace ph::editor
