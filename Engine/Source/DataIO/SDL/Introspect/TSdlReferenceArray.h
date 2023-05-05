#pragma once

#include "DataIO/SDL/Introspect/TSdlOwnedField.h"
#include "DataIO/SDL/ESdlTypeCategory.h"

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <type_traits>

namespace ph
{

class ISdlResource;

/*! @brief A value that points to an array of SDL resources.
@tparam T Type of the referenced SDL resource.
@tparam Owner Owner type of @p T. Note that Owner can be any type (not necessarily a SDL resource).
*/
template<typename T, typename Owner>
class TSdlReferenceArray : public TSdlOwnedField<Owner>
{
public:
	TSdlReferenceArray(std::string valueName, std::vector<std::shared_ptr<T>> Owner::* valuePtr);

	/*! @brief By default, default value of the array is empty.
	*/
	void setValueToDefault(Owner& owner) const override;

	std::string valueToString(const Owner& owner) const override;

	void ownedResources(
		const Owner& owner,
		std::vector<const ISdlResource*>& out_resources) const override;

	SdlNativeData ownedNativeData(Owner& owner) const override;

	void setValueVec(Owner& owner, std::vector<std::shared_ptr<T>> value) const;
	const std::vector<std::shared_ptr<T>>& getValueVec(const Owner& owner) const;

	TSdlReferenceArray& withImportance(EFieldImportance importance);
	TSdlReferenceArray& description(std::string descriptionStr);
	TSdlReferenceArray& optional();
	TSdlReferenceArray& niceToHave();
	TSdlReferenceArray& required();

protected:
	void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override;

	void saveToSdl(
		const Owner&            owner,
		SdlOutputPayload&       out_payload,
		const SdlOutputContext& ctx) const override;

	template<typename ResourceType = T>
	static std::vector<std::shared_ptr<T>> loadReferenceArray(
		const SdlInputPayload& payload,
		const SdlInputContext& ctx);

	template<typename ResourceType = T>
	static std::shared_ptr<ResourceType> loadReference(
		std::string_view referenceName,
		const SdlInputContext& ctx);

private:
	std::vector<std::shared_ptr<T>> Owner::* m_valuePtr;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TSdlReferenceArray.ipp"
