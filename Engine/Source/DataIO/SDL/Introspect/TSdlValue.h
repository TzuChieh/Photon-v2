#pragma once

#include "DataIO/SDL/Introspect/TAbstractSdlValue.h"
#include "Common/assertion.h"

#include <string_view>

namespace ph
{

/*! @brief Abstractions for a SDL value type.
A default value can be specified for instances of this type. If the default value is
not explicitly provided (by `defaultTo()` or other means), default value will be the
`value initialization` of the type `T` (basically a mix of default and zero initialization).

* `value initialization`:
  https://en.cppreference.com/w/cpp/language/value_initialization
*/
template<typename T, typename Owner>
class TSdlValue : public TAbstractSdlValue<T, Owner>
{
public:
	TSdlValue(
		std::string typeName, 
		std::string valueName, 
		T Owner::*  valuePtr);

	std::string valueAsString(const T& value) const override = 0;
	ESdlDataFormat getNativeFormat() const override = 0;
	ESdlDataType getNativeType() const override = 0;

	void setValue(Owner& owner, T value) const override;
	const T* getValue(const Owner& owner) const override;
	void setValueToDefault(Owner& owner) const override;
	SdlNativeData ownedNativeData(Owner& owner) const override;

	TSdlValue& defaultTo(T defaultValue);
	const T& defaultValue() const;
	TSdlValue& withImportance(EFieldImportance importance);
	TSdlValue& description(std::string descriptionStr);
	TSdlValue& optional();
	TSdlValue& niceToHave();
	TSdlValue& required();
	TSdlValue& enableFallback(bool isFallbackEnabled);

protected:
	void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override = 0;

	void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override = 0;

private:
	T Owner::* m_valuePtr;
	T          m_defaultValue;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TSdlValue.ipp"
