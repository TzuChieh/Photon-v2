#pragma once

#include "DataIO/SDL/Introspect/TOwnedSdlField.h"
#include "Common/assertion.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/SDL/NamedResourceStorage.h"
#include "DataIO/SDL/ETypeCategory.h"
#include "DataIO/SDL/SdlTypeInfo.h"
#include "Utility/string_utils.h"
#include "DataIO/SDL/sdl_exceptions.h"

#include <string>
#include <memory>
#include <utility>
#include <type_traits>

namespace ph
{

class ISdlResource;

template<ETypeCategory CATEGORY, typename T, typename Owner>
class TSdlReference : TOwnedSdlField<Owner>
{
	static_assert(std::is_base_of_v<ISdlResource, T>,
		"T must be a SDL resource (derive from ISdlResource)");

public:
	inline TSdlReference(
		std::string                       valueName,
		std::shared_ptr<T> Owner::* const valuePtr) :

		TOwnedSdlField<Owner>(SdlTypeInfo::categoryToName(CATEGORY), std::move(valueName)),

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
			"[" + SdlTypeInfo::categoryToName(CATEGORY) + " ref: " + 
			owner.*m_valuePtr ? "valid" : "empty" +
			"]";
	}

private:
	inline void loadFromSdl(
		Owner&             owner,
		const std::string& sdlValue,
		SdlInputContext&   ctx) override
	{
		const auto resourceName = string_utils::cut_head(sdlValue, "@");
		// TODO: get res should throw and accept str view
		owner.*m_valuePtr = ctx.resources->getResource<T>(resourceName, DataTreatment());

		if(!(owner.*m_valuePtr))
		{
			throw SdlLoadError("on parsing reference -> unable to load " + valueToString(owner));
		}
	}

	inline void convertToSdl(
		const Owner& owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const override
	{
		PH_ASSERT(out_sdl);

		// TODO
		PH_ASSERT_UNREACHABLE_SECTION();
	}

	std::shared_ptr<T> Owner::* m_valuePtr;
};

}// end namespace ph
