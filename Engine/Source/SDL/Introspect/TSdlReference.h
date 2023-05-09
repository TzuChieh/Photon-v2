#pragma once

#include "SDL/Introspect/TSdlOwnedField.h"
#include "SDL/ESdlTypeCategory.h"

#include <string>
#include <memory>
#include <type_traits>

namespace ph
{

class ISdlResource;

/*! @brief A value that points to a SDL resource.
@tparam T Type of the referenced SDL resource.
@tparam Owner Owner type of @p T. Note that Owner can be any type (not necessarily a SDL resource).
*/
template<typename T, typename Owner>
class TSdlReference : public TSdlOwnedField<Owner>
{
public:
	TSdlReference(std::string valueName, std::shared_ptr<T> Owner::* valuePtr);

	/*! @brief By default, default value of a SDL reference is empty.
	*/
	void setValueToDefault(Owner& owner) const override;

	std::string valueToString(const Owner& owner) const override;

	void ownedResources(
		const Owner& owner,
		std::vector<const ISdlResource*>& out_resources) const override;

	SdlNativeData ownedNativeData(Owner& owner) const override;

	void setValueRef(Owner& owner, std::shared_ptr<T> value) const;
	const std::shared_ptr<T>& getValueRef(const Owner& owner) const;

	TSdlReference& withImportance(EFieldImportance importance);
	TSdlReference& description(std::string descriptionStr);
	TSdlReference& optional();
	TSdlReference& niceToHave();
	TSdlReference& required();

protected:
	void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override;

	void saveToSdl(
		const Owner&            owner,
		SdlOutputClause&        out_clause,
		const SdlOutputContext& ctx) const override;

	template<typename ResourceType = T>
	static std::shared_ptr<ResourceType> loadResource(
		const SdlInputClause&  clause,
		const SdlInputContext& ctx);

private:
	std::shared_ptr<T> Owner::* m_valuePtr;
};

}// end namespace ph

#include "SDL/Introspect/TSdlReference.ipp"
