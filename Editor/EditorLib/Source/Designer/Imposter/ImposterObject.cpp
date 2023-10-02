#include "Designer/Imposter/ImposterObject.h"

#include <utility>

namespace ph::editor
{

PH_DEFINE_LOG_GROUP(Imposter, Designer);

bool ImposterObject::bindDescription(
	const std::shared_ptr<ISdlResource>& descResource,
	const std::string& descName)
{
	if(!descResource)
	{
		PH_LOG_WARNING(Imposter,
			"Ignoring incomplete input description: resource is null.");
		return false;
	}

	m_descName = descName;
	m_descResource = descResource;
	return true;
}

void ImposterObject::unbindDescription()
{
	m_descName.clear();
	m_descResource = nullptr;
}

}// end namespace ph::editor
