#include "SDL/Introspect/SdlOutputContext.h"
#include "SDL/SdlDependencyResolver.h"

#include <type_traits>

namespace ph
{

std::string_view SdlOutputContext::getResourceName(const ISdlResource* resource) const
{
	if(getDependencyResolver())
	{
		const auto resourceName = getDependencyResolver()->getResourceName(resource);

		// Just to make sure that the view is backed by others
		static_assert(
			std::is_same_v<std::remove_cv_t<decltype(resourceName)>, 
			std::string_view>);

		return resourceName;
	}
	else
	{
		return "";
	}
}

}// end namespace ph
