#include "Designer/Imposter/SpecializedImposterBinder.h"
#include "Designer/Imposter/GeneralImposter.h"
#include "Designer/DesignerScene.h"
#include "Designer/DesignerObject.h"
#include "Designer/Imposter/ImposterObject.h"

#include <ph_core.h>
#include <Common/assertion.h>
#include <SDL/TSdl.h>

#include <unordered_set>
#include <unordered_map>

namespace ph::editor
{

SpecializedImposterBinder::SpecializedImposterBinder(DesignerScene& scene)
	: m_scene(scene)
{}

ImposterObject* SpecializedImposterBinder::newImposter(
	const std::shared_ptr<ISdlResource>& descResource,
	const std::string& descName)
{
	ImposterObject* obj = createImposterAndBindDescription(descResource, descName);
	if(!obj)
	{
		return nullptr;
	}

	m_scene.getRenderDescription().getResources().add(descResource, descName);
	return obj;
}

ImposterObject* SpecializedImposterBinder::newImposter(const SdlClass* descClass)
{
	return newImposter(
		TSdl<>::makeResource(descClass),
		DesignerObject::generateObjectName());
}

ImposterObject* SpecializedImposterBinder::newImposter(const std::string& descName)
{
	auto descResource = m_scene.getRenderDescription().getResources().get(descName);
	if(!descResource)
	{
		PH_ASSERT_MSG(false,
			"Cannot find the target description resource.");
		return nullptr;
	}

	return createImposterAndBindDescription(descResource, descName);
}

ImposterObject* SpecializedImposterBinder::createImposterAndBindDescription(
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
	if(!imposterClass)
	{
		PH_ASSERT_MSG(false,
			"No valid imposter class specified for class <" + 
			descResource->getDynamicSdlClass()->genPrettyName() + ">.");
		return nullptr;
	}

	DesignerObject* obj = m_scene.newRootObject(imposterClass);
	ImposterObject* imposterObj = static_cast<ImposterObject*>(obj);
	PH_ASSERT(dynamic_cast<ImposterObject*>(obj));
	PH_ASSERT(imposterObj);

	if(imposterObj->bindDescription(descResource, descName))
	{
		return imposterObj;
	}
	else
	{
		m_scene.deleteObject(imposterObj);
		return nullptr;
	}
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
