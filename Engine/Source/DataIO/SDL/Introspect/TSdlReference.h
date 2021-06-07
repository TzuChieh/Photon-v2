#pragma once

#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "Common/assertion.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/SDL/SceneDescription.h"
#include "DataIO/SDL/ETypeCategory.h"
#include "Utility/string_utils.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/sdl_helpers.h"

#include <string>
#include <memory>
#include <utility>
#include <type_traits>

namespace ph
{

class ISdlResource;

template<typename T, typename Owner>
class TSdlReference : TOwnedSdlField<Owner>
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T must be a SDL resource (derive from ISdlResource).");

public:
	inline TSdlReference(
		const ETypeCategory               category,
		std::string                       valueName,
		std::shared_ptr<T> Owner::* const valuePtr) :

		TOwnedSdlField<Owner>(sdl::category_to_string(category), std::move(valueName)),

		m_category(category),
		m_valuePtr(valuePtr)
	{
		PH_ASSERT(m_valuePtr);
	}

	inline void setValueToDefault(Owner& owner) override
	{
		// Nothing to set; default value for a SDL resource defined as nullptr
	}

	inline std::string valueToString(const Owner& owner) const override
	{
		return 
			"[" + sdl::category_to_string(category) + " ref: " +
			getValuePtr(owner) ? "valid" : "empty" +
			"]";
	}

	inline void setValuePtr(Owner& owner, std::shared_ptr<T> value) const
	{
		owner.*m_valuePtr = std::move(value);
	}

	inline const std::shared_ptr<T>& getValuePtr(const Owner& owner) const
	{
		return owner.*m_valuePtr;
	}

private:
	inline void loadFromSdl(
		Owner&                 owner,
		const std::string&     sdlValue,
		const SdlInputContext& ctx) const override
	{
		const auto resourceName = string_utils::cut_head(sdlValue, "@");
		// TODO: get res should throw and accept str view
		setValue(ctx.scene->getResource<T>(resourceName, DataTreatment()));

		if(!getValuePtr(owner))
		{
			throw SdlLoadError(
				"on parsing reference -> unable to load " + valueToString(owner));
		}
	}

	inline void convertToSdl(
		const Owner& owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const override
	{
		PH_ASSERT(out_sdlValue);

		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}

	ETypeCategory               m_category;
	std::shared_ptr<T> Owner::* m_valuePtr;
};

}// end namespace ph
