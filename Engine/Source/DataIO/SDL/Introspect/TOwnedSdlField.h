#pragma once

#include "DataIO/SDL/Introspect/SdlField.h"
#include "Common/assertion.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "Common/Logger.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/SdlIOUtils.h"

#include <utility>
#include <string>

namespace ph
{

enum class EFieldImportance
{
	// Value for the field may be omitted as the default value is usually
	// what the user want
	OPTIONAL,

	// Value for the field should be provided as the engine may not be able 
	// to provide a suitable value by itself
	NICE_TO_HAVE,

	// Value for the field is necessary and the engine provided value is
	// typically not what the user meant to have
	REQUIRED
};

template<typename Owner>
class TOwnedSdlField : public SdlField
{
public:
	TOwnedSdlField(std::string typeName, std::string valueName);

	virtual void setValueToDefault(Owner& owner) const = 0;
	virtual std::string valueToString(const Owner& owner) const = 0;

	void fromSdl(
		Owner&                 owner, 
		const std::string&     sdlValue,
		const SdlInputContext& ctx) const;

	void toSdl(
		const Owner& owner,
		std::string* out_sdlValue,
		std::string& out_message) const;

	TOwnedSdlField& setImportance(EFieldImportance importance);
	EFieldImportance getImportance() const;

private:
	virtual void loadFromSdl(
		Owner&             owner, 
		const std::string& sdlValue,
		SdlInputContext&   ctx) const = 0;

	virtual void convertToSdl(
		const Owner& owner,
		std::string* out_sdlValue,
		std::string& out_converterMessage) const = 0;

	EFieldImportance m_importance;
};

// In-header Implementations:

template<typename Owner>
inline TOwnedSdlField<Owner>::TOwnedSdlField(std::string typeName, std::string valueName) : 

	SdlField(std::move(typeName), std::move(valueName)),

	m_importance(EFieldImportance::NICE_TO_HAVE)
{}

template<typename Owner>
inline void TOwnedSdlField<Owner>::fromSdl(
	Owner&                 owner,
	const std::string&     sdlValue,
	const SdlInputContext& ctx) const
{
	try
	{
		loadFromSdl(owner, sdlValue, ctx);
	}
	catch(const SdlLoadError& e)
	{
		if(isFallbackEnabled())
		{
			setValueToDefault(owner);

			// Always log for recovered failed loading attempt
			// (as the user provided a SDL value string for the field, 
			// a successful parse was expected)
			//
			logger.log(ELogLevel::WARNING_MED,
				"load error from " + SdlIOUtils::genPrettyName(ctx.srcClass, this) + 
				" -> " + e.what() + "; " +
				"value defaults to " + valueToString(owner));
		}
		else
		{
			// Let caller handle the error if fallback is disabled
			//
			throw SdlLoadError(
				"load error from " + SdlIOUtils::genPrettyName(ctx.srcClass, this) +
				" -> " + e.what() + "; " +
				"value left uninitialized");
		}
	}
}

template<typename Owner>
inline void TOwnedSdlField<Owner>::toSdl(
	const Owner&       owner,
	std::string* const out_sdlValue,
	std::string&       out_message) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

template<typename Owner>
inline TOwnedSdlField<Owner>& TOwnedSdlField<Owner>::setImportance(const EFieldImportance importance)
{
	m_importance = importance;

	return *this;
}

template<typename Owner>
inline EFieldImportance TOwnedSdlField<Owner>::getImportance() const
{
	return m_importance;
}

}// end namespace ph
