#include "Designer/Imposter/ImposterObject.h"

#include <utility>

namespace ph::editor
{

PH_DEFINE_LOG_GROUP(ImposterObject, Designer);

bool ImposterObject::bindDescription(
	const std::shared_ptr<ISdlResource>& descResource,
	const std::string& descName)
{
	if(!descResource)
	{
		PH_LOG_WARNING(ImposterObject,
			"Ignoring incomplete input description: resource is null.");
		return false;
	}

	m_descName = descName;
	return true;
}

void ImposterObject::unbindDescription()
{
	m_descName.clear();
}

}// end namespace ph::editor
