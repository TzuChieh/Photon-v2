#include "Designer/Imposter/SpecializedImposterBinder.h"
#include "Designer/Imposter/GeneralImposter.h"
#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"

#include <ph_cpp_core.h>
#include <Common/assertion.h>
#include <SDL/TSdl.h>

#include <unordered_set>
#include <unordered_map>

namespace ph::editor
{

SpecializedImposterBinder::SpecializedImposterBinder(DesignerScene& scene)
	: m_scene(scene)
{}

DesignerObject* SpecializedImposterBinder::newImposter(
	const std::shared_ptr<ISdlResource>& descResource,
	const std::string& descName)
{
	if(!descResource || descName.empty())
	{
		PH_ASSERT_MSG(false,
			"Cannot bind empty description or description with empty name.");
		return nullptr;
	}

	const SdlClass* imposterClass = getImposterClass(descResource->getDynamicSdlClass());
	return imposterClass ? m_scene.newRootObject(imposterClass) : nullptr;
}

DesignerObject* SpecializedImposterBinder::newImposter(const SdlClass* descClass)
{
	return newImposter(
		TSdl<>::makeResource(descClass),
		DesignerObject::generateObjectName());
}

bool SpecializedImposterBinder::isDescriptionClass(const SdlClass* clazz)
{
	static std::unordered_set<const SdlClass*> descClasses =
		[]()
		{
			auto classes = get_registered_engine_classes();
			return std::unordered_set<const SdlClass*>(classes.begin(), classes.end());
		}();

	return clazz && descClasses.contains(clazz);
}

const SdlClass* SpecializedImposterBinder::getImposterClass(const SdlClass* descClass)
{
	static std::unordered_map<const SdlClass*, const SdlClass*> descToImposter = 
		[]()
		{
			std::unordered_map<const SdlClass*, const SdlClass*> descToImposter;
			return descToImposter;
		}();

	if(isDescriptionClass(descClass))
	{
		auto result = descToImposter.find(descClass);
		return result != descToImposter.end() ? result->second : GeneralImposter::getSdlClass();
	}
	else
	{
		return nullptr;
	}
}

}// end namespace ph::editor
