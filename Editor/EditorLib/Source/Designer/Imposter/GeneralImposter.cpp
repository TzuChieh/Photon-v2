#include "Designer/Imposter/GeneralImposter.h"

namespace ph::editor
{

bool GeneralImposter::bindTarget(
	const std::shared_ptr<ISdlResource>& resource,
	const std::string& targetName)
{
	if(!Base::bindTarget(resource, targetName))
	{
		return false;
	}

	m_targetResource = resource;
	return true;
}

void GeneralImposter::unbindTarget()
{
	m_targetResource = nullptr;

	Base::unbindTarget();
}

}// end namespace ph::editor
