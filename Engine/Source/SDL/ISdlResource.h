#pragma once

#include "SDL/ESdlTypeCategory.h"
#include "SDL/SdlResourceId.h"

namespace ph { class SdlClass; }

namespace ph
{

/*! @brief Interface for all SDL resource.

A SDL resource is data that can be managed by SDL and actor system.

Note that in addition to the pure virtual methods, implementation should add
a static @p ESdlTypeCategory field named @p CATEGORY and assign a proper category
to it. This will facilitate static resource category inspection and is required
for some introspection classes. However, these normally will not need to be
done since the base classes for each category already did it.
*/
class ISdlResource
{
protected:
	inline ISdlResource() = default;

public:
	inline virtual ~ISdlResource() = default;

	virtual ESdlTypeCategory getDynamicCategory() const = 0;
	virtual SdlResourceId getId() const = 0;

	/*! @brief Get runtime SDL class of the resource.
	@return Pointer to the runtime SDL class. nullptr if PH_DEFINE_SDL_CLASS() is
	not implemented by the resource class.
	*/
	virtual const SdlClass* getDynamicSdlClass() const;
};

// In-header Implementation:

inline const SdlClass* ISdlResource::getDynamicSdlClass() const
{
	return nullptr;
}

}// end namespace ph
