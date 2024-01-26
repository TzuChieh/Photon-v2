#pragma once

#include "SDL/Introspect/TSdlOwnedField.h"

#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

namespace ph
{

template<typename Struct, typename Owner>
class TSdlStructArray : public TSdlOwnedField<Owner>
{
	static_assert(!std::is_same_v<Struct, Owner>,
		"`Struct` and `Owner` is not allowed to be the same type (for reducing the risk of "
		"cyclic references and infinite recursion on initialization).");

public:
	TSdlStructArray(std::string valueName, std::vector<Struct> Owner::* const valuePtr);

	/*! @brief By default, default value of the array is empty.
	*/
	void ownedValueToDefault(Owner& owner) const override;

	std::string valueToString(const Owner& owner) const override;

	void ownedResources(
		const Owner& owner,
		std::vector<const ISdlResource*>& out_resources) const override;

	SdlNativeData ownedNativeData(Owner& owner) const override;

	void setValueVec(Owner& owner, std::vector<Struct> value) const;
	const std::vector<Struct>& getValueVec(const Owner& owner) const;

	TSdlStructArray& withImportance(EFieldImportance importance);
	TSdlStructArray& description(std::string descriptionStr);
	TSdlStructArray& optional();
	TSdlStructArray& niceToHave();
	TSdlStructArray& required();

protected:
	void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override;

	void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override;

	static std::vector<Struct> loadStructArray(
		const SdlInputClause&  clause,
		const SdlInputContext& ctx);

	static Struct loadStruct(
		std::string_view packetName,
		const SdlInputContext& ctx);

private:
	static std::string getStructTypeName();

	std::vector<Struct> Owner::* m_valuePtr;
};

}// end namespace ph

#include "SDL/Introspect/TSdlStructArray.ipp"
