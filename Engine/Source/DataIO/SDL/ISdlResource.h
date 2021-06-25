#pragma once

#include "DataIO/SDL/ETypeCategory.h"

namespace ph
{

/*! @brief Interface for all SDL resource.

A SDL resource is data that can be managed by SDL and actor system.

Note that in addition to the pure virtual methods, implementation should add
a static @p ETypeCategory field named @p CATEGORY and assign a proper category
to it. This will facilitate static resource category inspection and is required
for some introspection classes. However, these normally will not need to be
done since the base classes for each category already did it.
*/
class ISdlResource
{
public:
	virtual ~ISdlResource() = default;

	virtual ETypeCategory getCategory() const = 0;
};

}// end namespace ph

// TODO: remove these
#include "DataIO/SDL/CommandRegister.h"
