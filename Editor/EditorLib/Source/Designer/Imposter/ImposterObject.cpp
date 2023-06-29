#include "Designer/Imposter/ImposterObject.h"

#include <utility>

namespace ph::editor
{

PH_DEFINE_LOG_GROUP(ImposterObject, Designer);

void ImposterObject::setTarget(std::shared_ptr<ISdlResource> resource)
{
	if(!resource)
	{
		PH_LOG_WARNING(ImposterObject,
			"ignoring incomplete input target: resource is null");
		return;
	}

	m_targetResource = std::move(resource);
}

}// end namespace ph::editor
