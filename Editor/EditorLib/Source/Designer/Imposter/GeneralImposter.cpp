#include "Designer/Imposter/GeneralImposter.h"

#include <Math/Transform/TDecomposedTransform.h>

namespace ph::editor
{

bool GeneralImposter::bindDescription(
	const std::shared_ptr<ISdlResource>& descResource,
	const std::string& descName)
{
	if(!Base::bindDescription(descResource, descName))
	{
		return false;
	}

	m_descResource = descResource;
	return true;
}

void GeneralImposter::unbindDescription()
{
	m_descResource = nullptr;

	Base::unbindDescription();
}

math::TDecomposedTransform<real> GeneralImposter::getLocalToParent() const
{
	return m_imposterTransform.getDecomposed();
}

void GeneralImposter::setLocalToParent(const math::TDecomposedTransform<real>& transform)
{
	m_imposterTransform.set(transform);
}

}// end namespace ph::editor
