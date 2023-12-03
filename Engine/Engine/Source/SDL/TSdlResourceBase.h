#pragma once

#include "SDL/SdlResourceBase.h"
#include "SDL/ESdlTypeCategory.h"

namespace ph
{

/*! @brief A convenient ISdlResource with core requirements implemented.
This class is similar to SdlResourceBase, except that it provides a way for 
derived classes to supply static category information. Dynamic category is 
directly obtained from it.
*/
template<ESdlTypeCategory TYPE_CATEGORY>
class TSdlResourceBase : public SdlResourceBase
{
public:
	/*! @brief Static category information of the resource.
	This static category information is required by ISdlResource. See its documentation
	for more details.
	*/
	static constexpr ESdlTypeCategory CATEGORY = TYPE_CATEGORY;

protected:
	inline TSdlResourceBase() = default;
	inline TSdlResourceBase(const TSdlResourceBase& other) = default;
	inline TSdlResourceBase(TSdlResourceBase&& other) = default;

	inline TSdlResourceBase& operator = (const TSdlResourceBase& rhs) = default;
	inline TSdlResourceBase& operator = (TSdlResourceBase&& rhs) = default;

public:
	ESdlTypeCategory getDynamicCategory() const override;
};

// In-header Implementations:

template<ESdlTypeCategory TYPE_CATEGORY>
inline ESdlTypeCategory TSdlResourceBase<TYPE_CATEGORY>::getDynamicCategory() const
{
	return CATEGORY;
}

}// end namespace ph
