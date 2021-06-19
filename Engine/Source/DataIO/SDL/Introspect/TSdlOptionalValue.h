#pragma once

#include "DataIO/SDL/Introspect/TAbstractSdlValue.h"
#include "Common/assertion.h"

#include <string_view>
#include <optional>

namespace ph
{

/*! @brief Abstractions for a SDL value type that can be empty.

An optional SDL value defaults to empty (no value) and has optional importance
(@p EFieldImportance::OPTIONAL) always. If the above properties are undesired,
consider using @p TSdlValue.
*/
template<typename T, typename Owner>
class TSdlOptionalValue : public TAbstractSdlValue<T, Owner>
{
public:
	TSdlOptionalValue(
		std::string               typeName, 
		std::string               valueName, 
		std::optional<T> Owner::* valuePtr);

	std::string valueAsString(const T& value) const override = 0;

	void setValue(Owner& owner, T value) const override;
	const T* getValue(const Owner& owner) const override;
	void setValueToDefault(Owner& owner) const override;

	TSdlOptionalValue& description(std::string descriptionStr);

protected:
	void loadFromSdl(
		Owner&                 owner,
		const std::string&     sdlValue,
		const SdlInputContext& ctx) const override = 0;

	void convertToSdl(
		const Owner& owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const override = 0;

private:
	std::optional<T> Owner::* m_valuePtr;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TSdlOptionalValue.ipp"
