#pragma once

#include "SDL/Introspect/TSdlAbstractValue.h"

#include <Common/assertion.h>

#include <string_view>
#include <optional>

namespace ph
{

/*! @brief Abstractions for a SDL value type.

A default value can be specified for instances of this type. If the default value is
not explicitly provided (by `defaultTo()` or other means), default value will be the
`value initialization` of the type `T` (basically a mix of default and zero initialization).
If default value is not wanted, use `noDefault()`.

* `value initialization`:
  https://en.cppreference.com/w/cpp/language/value_initialization
*/
template<typename T, typename Owner>
class TSdlValue : public TSdlAbstractValue<T, Owner>
{
public:
	TSdlValue(
		std::string typeName, 
		std::string valueName, 
		T Owner::* valuePtr);

	TSdlValue(
		std::string typeName,
		std::string valueName,
		std::string typeSignature,
		T Owner::* valuePtr);

	std::string valueAsString(const T& value) const override = 0;
	SdlNativeData ownedNativeData(Owner& owner) const override = 0;

	void setValue(Owner& owner, T value) const override;
	T* getValue(Owner& owner) const override;
	const T* getConstValue(const Owner& owner) const override;
	void ownedValueToDefault(Owner& owner) const override;

	const T* getDefaultValue() const;

	TSdlValue& defaultTo(T defaultValue);
	TSdlValue& noDefault();
	TSdlValue& withImportance(EFieldImportance importance);
	TSdlValue& description(std::string descriptionStr);
	TSdlValue& optional();
	TSdlValue& niceToHave();
	TSdlValue& required();
	TSdlValue& enableFallback(bool isFallbackEnabled);

protected:
	void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override = 0;

	void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override = 0;

	T& valueRef(Owner& owner) const;

private:
	T Owner::* m_valuePtr;
	std::optional<T> m_defaultValue;
};

}// end namespace ph

#include "SDL/Introspect/TSdlValue.ipp"
