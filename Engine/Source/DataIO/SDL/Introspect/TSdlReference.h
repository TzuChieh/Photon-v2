#pragma once

#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "DataIO/SDL/ETypeCategory.h"

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
class TSdlReference : public TOwnedSdlField<Owner>
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T must be a SDL resource (derive from ISdlResource).");

public:
	TSdlReference(std::string valueName, std::shared_ptr<T> Owner::* valuePtr);

	/*! @brief Default value of a SDL reference is empty.
	*/
	void setValueToDefault(Owner& owner) const override;

	std::string valueToString(const Owner& owner) const override;

	const ISdlResource* associatedResource(const Owner& owner) const override;

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
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override;

	void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override;

private:
	std::shared_ptr<T> Owner::* m_valuePtr;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TSdlReference.ipp"
