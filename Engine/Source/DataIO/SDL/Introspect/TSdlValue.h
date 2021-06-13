#pragma once

#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "Common/assertion.h"

#include <utility>
#include <string_view>

namespace ph
{

template<typename T, typename Owner>
class TSdlValue : public TOwnedSdlField<Owner>
{
public:
	TSdlValue(
		std::string typeName, 
		std::string valueName, 
		T Owner::*  valuePtr);

	std::string valueToString(const Owner& owner) const = 0;

	void setValueToDefault(Owner& owner) const override;

	void setValue(Owner& owner, T value) const;
	const T& getValue(const Owner& owner) const;
	const T& getDefaultValue() const;

	TSdlValue& defaultTo(T defaultValue);
	TSdlValue& withImportance(EFieldImportance importance);
	TSdlValue& description(std::string descriptionStr);
	TSdlValue& optional();
	TSdlValue& niceToHave();
	TSdlValue& required();

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
	T Owner::* m_valuePtr;
	T          m_defaultValue;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TSdlValue.ipp"
