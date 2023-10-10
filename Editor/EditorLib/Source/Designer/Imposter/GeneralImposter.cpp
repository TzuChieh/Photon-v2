#include "Designer/Imposter/GeneralImposter.h"
#include "Designer/UI/UIPropertyLayout.h"

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

	return true;
}

void GeneralImposter::unbindDescription()
{
	Base::unbindDescription();
}

UIPropertyLayout GeneralImposter::layoutProperties()
{
	UIPropertyLayout layout = Base::layoutProperties();

	// Description name cannot be set directly as it requires additional logic to ensure the binding
	// to imposter is valid
	if(UIProperty* prop = layout.findPropertyInGroup("Designer", "desc-name"); prop)
	{
		prop->setReadOnly(true);
	}

	if(getDescription())
	{
		layout.addGroups(UIPropertyLayout::makeLinearListing(
			getDescription(), "Description"));
	}

	return layout;
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
