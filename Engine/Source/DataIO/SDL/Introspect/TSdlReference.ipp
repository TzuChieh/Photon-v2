#pragma once

#include "DataIO/SDL/Introspect/TSdlReference.h"
#include "Common/assertion.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/SDL/SceneDescription.h"
#include "Utility/string_utils.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/sdl_helpers.h"

#include <utility>

namespace ph
{

template<typename T, typename Owner>
inline TSdlReference<T, Owner>::TSdlReference(
	std::string                       valueName,
	std::shared_ptr<T> Owner::* const valuePtr) :

	TOwnedSdlField<Owner>(
		sdl::category_to_string(sdl::category_of<T>()),
		std::move(valueName)),

	m_valuePtr(valuePtr)
{
	PH_ASSERT(m_valuePtr);
}

template<typename T, typename Owner>
inline void TSdlReference<T, Owner>::setValueToDefault(Owner& owner) const
{
	// Default value for a SDL resource defined as nullptr
	setValueRef(owner, nullptr);
}

template<typename T, typename Owner>
inline std::string TSdlReference<T, Owner>::valueToString(const Owner& owner) const
{
	return 
		"[" + sdl::category_to_string(sdl::category_of<T>()) + " ref: " +
		std::string(getValueRef(owner) ? "valid" : "empty") + "]";
}

template<typename T, typename Owner>
inline void TSdlReference<T, Owner>::setValueRef(Owner& owner, std::shared_ptr<T> value) const
{
	owner.*m_valuePtr = std::move(value);
}

template<typename T, typename Owner>
inline const std::shared_ptr<T>& TSdlReference<T, Owner>::getValueRef(const Owner& owner) const
{
	return owner.*m_valuePtr;
}

template<typename T, typename Owner>
inline void TSdlReference<T, Owner>::loadFromSdl(
	Owner&                 owner,
	const SdlPayload&      payload,
	const SdlInputContext& ctx) const
{
	const auto resourceName = string_utils::cut_head(payload.value, "@");
	// TODO: get res should accept str view
	// TODO: allow type mismatch?
	try
	{
		PH_ASSERT(ctx.getRawScene());

		setValueRef(
			owner,
			ctx.getRawScene()->getResource<T>(std::string(resourceName)));
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError(
			"unable to load resource on parsing reference " + 
			valueToString(owner) + " -> " + e.whatStr());
	}
}

template<typename T, typename Owner>
inline void TSdlReference<T, Owner>::convertToSdl(
	const Owner& owner,
	std::string* out_sdlValue,
	std::string& out_converterMessage) const
{
	PH_ASSERT(out_sdlValue);

	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

template<typename T, typename Owner>
inline auto TSdlReference<T, Owner>::withImportance(const EFieldImportance importance)
-> TSdlReference&
{
	setImportance(importance);
	return *this;
}

template<typename T, typename Owner>
inline auto TSdlReference<T, Owner>::description(std::string descriptionStr)
-> TSdlReference&
{
	setDescription(std::move(descriptionStr));
	return *this;
}

template<typename T, typename Owner>
inline auto TSdlReference<T, Owner>::optional()
-> TSdlReference&
{
	return withImportance(EFieldImportance::OPTIONAL);
}

template<typename T, typename Owner>
inline auto TSdlReference<T, Owner>::niceToHave()
-> TSdlReference&
{
	return withImportance(EFieldImportance::NICE_TO_HAVE);
}

template<typename T, typename Owner>
inline auto TSdlReference<T, Owner>::required()
-> TSdlReference&
{
	return withImportance(EFieldImportance::REQUIRED);
}

}// end namespace ph
