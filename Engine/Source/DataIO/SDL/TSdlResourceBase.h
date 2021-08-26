#pragma once

#include "DataIO/SDL/SdlResourceBase.h"
#include "DataIO/SDL/ETypeCategory.h"

namespace ph
{

/*! @brief A convenient ISdlResource with core requirements implemented.
This class is similar to SdlResourceBase, except that it provides a way for 
derived classes to supply static category information. Dynamic category is 
directly obtained from it.
*/
template<ETypeCategory TYPE_CATEGORY>
class TSdlResourceBase : public SdlResourceBase
{
public:
	/*! @brief Static category information of the resource.
	This static category information is required by ISdlResource. See its documentation
	for more details.
	*/
	static constexpr ETypeCategory CATEGORY = TYPE_CATEGORY;

protected:
	inline TSdlResourceBase() = default;
	inline TSdlResourceBase(const TSdlResourceBase& other) = default;
	inline TSdlResourceBase(TSdlResourceBase&& other) = default;

	inline TSdlResourceBase& operator = (const TSdlResourceBase& rhs) = default;
	inline TSdlResourceBase& operator = (TSdlResourceBase&& rhs) = default;

public:
	ETypeCategory getCategory() const override;
};

// In-header Implementations:

template<ETypeCategory TYPE_CATEGORY>
inline ETypeCategory TSdlResourceBase<TYPE_CATEGORY>::getCategory() const
{
	return CATEGORY;
}

}// end namespace ph
