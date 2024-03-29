#pragma once

#include "SDL/Introspect/TSdlOwnedField.h"

#include <Common/assertion.h>

#include <string>
#include <vector>

namespace ph
{

/*! @brief General concept of a SDL value type.
*/
template<typename T, typename Owner>
class TSdlAbstractValue : public TSdlOwnedField<Owner>
{
public:
	TSdlAbstractValue(
		std::string typeName, 
		std::string valueName);

	TSdlAbstractValue(
		std::string typeName, 
		std::string valueName,
		std::string typeSignature);

	/*! @brief Human-readable representation of the value.
	*/
	virtual std::string valueAsString(const T& value) const = 0;

	/*! @brief Store a value.
	*/
	virtual void setValue(Owner& owner, T value) const = 0;

	/*! @brief Get a pointer to the stored value.
	@return Pointer to the stored value. nullptr if no value is stored.
	*/
	virtual T* getValue(Owner& owner) const = 0;

	/*! @brief Get a pointer to the stored value.
	@return Pointer to the stored value. nullptr if no value is stored.
	*/
	virtual const T* getConstValue(const Owner& owner) const = 0;

	void ownedValueToDefault(Owner& owner) const override = 0;

	SdlNativeData ownedNativeData(Owner& owner) const override = 0;

	std::string valueToString(const Owner& owner) const override;

	void ownedResources(
		const Owner& owner,
		std::vector<const ISdlResource*>& out_resources) const override;

protected:
	void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override = 0;

	void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override = 0;
};

}// end namespace ph

#include "SDL/Introspect/TSdlAbstractValue.ipp"
