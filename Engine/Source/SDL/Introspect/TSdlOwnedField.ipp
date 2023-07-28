#pragma once

#include "SDL/Introspect/TSdlOwnedField.h"
#include "Common/assertion.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "SDL/Introspect/SdlOutputContext.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/sdl_helpers.h"
#include "SDL/SdlInputClause.h"
#include "SDL/SdlOutputClause.h"
#include "Utility/traits.h"

#include <utility>

namespace ph
{

template<typename Owner>
inline TSdlOwnedField<Owner>::TSdlOwnedField(std::string typeName, std::string valueName) :

	SdlField(std::move(typeName), std::move(valueName)),

	m_importance(EFieldImportance::NiceToHave)
{}

template<typename Owner>
inline SdlNativeData TSdlOwnedField<Owner>::nativeData(ISdlResource& resource) const
{
	if constexpr(CDerived<Owner, ISdlResource>)
	{
		try
		{
			Owner* const owner = sdl::cast_to<Owner>(&resource);
			return ownedNativeData(*owner);
		}
		catch(const SdlException& /* e */)
		{
			return {};
		}
	}
	else
	{
		// Non SDL resource--cannot get native data, need to use `ownedNativeData()`
		return {};
	}
}

template<typename Owner>
inline void TSdlOwnedField<Owner>::fromSdl(
	Owner&                 owner,
	const SdlInputClause&  clause,
	const SdlInputContext& ctx) const
{
	try
	{
		loadFromSdl(owner, clause, ctx);
	}
	catch(const SdlException& e)
	{
		if(isFallbackEnabled())
		{
			setValueToDefault(owner);

			// Always log for recovered failed loading attempt
			// (as the user provided a SDL value string for the field, 
			// a successful parse was expected)
			//
			PH_LOG_WARNING(SdlField, "load error from {} -> {}; value defaults to {}", 
				sdl::gen_pretty_name(ctx.getSrcClass(), this),
				e.whatStr(),
				valueToString(owner));
		}
		else
		{
			// Let caller handle the error if fallback is disabled
			//
			throw SdlLoadError(
				"load error from " + sdl::gen_pretty_name(ctx.getSrcClass(), this) +
				" -> " + e.whatStr() + "; " +
				"value left uninitialized");
		}
	}
}

template<typename Owner>
inline void TSdlOwnedField<Owner>::toSdl(
	const Owner&            owner,
	SdlOutputClause&        out_clause,
	const SdlOutputContext& ctx) const
{
	try
	{
		saveToSdl(owner, out_clause, ctx);
	}
	catch(const SdlException& e)
	{
		// Provide more information and let caller handle the error
		//
		throw SdlSaveError(
			"save error from " + sdl::gen_pretty_name(ctx.getSrcClass(), this) +
			" -> " + e.whatStr());
	}
}

template<typename Owner>
inline TSdlOwnedField<Owner>& TSdlOwnedField<Owner>::setImportance(const EFieldImportance importance)
{
	m_importance = importance;

	return *this;
}

template<typename Owner>
inline EFieldImportance TSdlOwnedField<Owner>::getImportance() const
{
	return m_importance;
}

}// end namespace ph
