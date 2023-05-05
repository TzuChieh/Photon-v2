#pragma once

#include "DataIO/SDL/Introspect/TSdlAbstractValue.h"
#include "Common/assertion.h"

#include <string_view>
#include <optional>

namespace ph
{

/*! @brief Abstractions for a SDL value type that can be empty.

An optional SDL value defaults to empty (no value) and has optional importance
(@p EFieldImportance::Optional) always. While no default value can be provided
for the optional field, it allows the detection of uninitialized value during
runtime. If the above properties are undesired, consider using @p TSdlValue.
*/
template<typename T, typename Owner>
class TSdlOptionalValue : public TSdlAbstractValue<T, Owner>
{
public:
	TSdlOptionalValue(
		std::string               typeName, 
		std::string               valueName, 
		std::optional<T> Owner::* valuePtr);

	std::string valueAsString(const T& value) const override = 0;
	SdlNativeData ownedNativeData(Owner& owner) const override = 0;

	void setValue(Owner& owner, T value) const override;
	T* getValue(Owner& owner) const override;
	const T* getConstValue(const Owner& owner) const override;
	void setValueToDefault(Owner& owner) const override;

	TSdlOptionalValue& description(std::string descriptionStr);

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
	std::optional<T> Owner::* m_valuePtr;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TSdlOptionalValue.ipp"
