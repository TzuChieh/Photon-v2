#include "Designer/Imposter/ImposterObject.h"

#include <utility>

namespace ph::editor
{

PH_DEFINE_LOG_GROUP(ImposterObject, Designer);

bool ImposterObject::bindTarget(
	const std::shared_ptr<ISdlResource>& resource,
	const std::string& targetName)
{
	if(!resource)
	{
		PH_LOG_WARNING(ImposterObject,
			"ignoring incomplete input target: resource is null");
		return false;
	}

	m_targetName = targetName;
	return true;
}

void ImposterObject::unbindTarget()
{
	m_targetName.clear();
}

}// end namespace ph::editor
