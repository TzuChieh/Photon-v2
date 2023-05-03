#pragma once

#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "Common/assertion.h"

#include <string>
#include <vector>

namespace ph
{

/*! @brief General concept of a SDL value type.
*/
template<typename T, typename Owner>
class TAbstractSdlValue : public TOwnedSdlField<Owner>
{
public:
	TAbstractSdlValue(
		std::string typeName, 
		std::string valueName);

	/*! @brief Human-readable representation of the value.
	*/
	virtual std::string valueAsString(const T& value) const = 0;

	/*! @brief Store a value.
	*/
	virtual void setValue(Owner& owner, T value) const = 0;

	/*! @brief Get a pointer to the stored value.
	@return Pointer to the stored value. nullptr if no value is stored.
	*/
	virtual const T* getValue(const Owner& owner) const = 0;

	virtual ESdlDataFormat getNativeFormat() const = 0;
	virtual ESdlDataType getNativeType() const = 0;

	void setValueToDefault(Owner& owner) const override = 0;

	SdlNativeData ownedNativeData(Owner& owner) const override = 0;

	std::string valueToString(const Owner& owner) const override;

	void ownedResources(
		const Owner& owner,
		std::vector<const ISdlResource*>& out_resources) const override;

protected:
	void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override = 0;

	void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override = 0;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TAbstractSdlValue.ipp"
