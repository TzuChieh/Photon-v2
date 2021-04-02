#pragma once

#include "DataIO/SDL/Introspect/SdlField.h"
#include "Common/assertion.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"

#include <utility>
#include <string>

namespace ph
{

enum class EFieldImportance
{
	OPTIONAL,
	NICE_TO_HAVE,
	REQUIRED
};

template<typename Owner>
class TOwnedSdlField : public SdlField
{
public:
	TOwnedSdlField(std::string typeName, std::string valueName);

	virtual void setValueToDefault(Owner& owner) = 0;
	virtual std::string valueToString(Owner& owner) const = 0;

	void fromSdl(
		Owner&             owner, 
		const std::string& sdlValue,
		SdlInputContext&   ctx);

	void toSdl(
		Owner&       owner, 
		std::string* out_sdlValue,
		std::string& out_message) const;

	TOwnedSdlField& setImportance(EFieldImportance importance);
	EFieldImportance getImportance() const;

private:
	virtual void loadFromSdl(
		Owner&             owner, 
		const std::string& sdlValue,
		SdlInputContext&   ctx) = 0;

	virtual void convertToSdl(
		Owner&       owner, 
		std::string* out_sdlValue,
		std::string& out_converterMessage) const = 0;

	EFieldImportance m_importance;
};

// In-header Implementations:

template<typename Owner>
inline TOwnedSdlField<Owner>::TOwnedSdlField(std::string typeName, std::string valueName) : 

	SdlField(std::move(typeName), std::move(valueName)),

	m_importance(EFieldImportance::OPTIONAL)
{}

template<typename Owner>
inline void TOwnedSdlField<Owner>::fromSdl(
	Owner&             owner,
	const std::string& sdlValue,
	SdlInputContext&   ctx)
{
	std::string loaderMessage;
	if(loadFromSdl(owner, sdlValue, loaderMessage))
	{
		// Even if it is a successful load, record loader's message if there is
		// any. This is done for non-optional field only.
		if(!loaderMessage.empty() && m_importance != EFieldImportance::OPTIONAL)
		{
			out_message += "field value <" + genPrettyName() + "> note: " + loaderMessage;
		}

		return true;
	}
	else
	{
		// For required field always emit a message to warn the user on failed
		// loading attempt.
		if(m_importance == EFieldImportance::REQUIRED)
		{
			out_message += "failed to load value for field <" + genPrettyName() + ">";
			if(!loaderMessage.empty())
			{
				out_message += ", " + loaderMessage;
			}
		}
		// For non-required field, we will only emit message if the field is
		// non-optional and the loader has something to say.
		else if(!loaderMessage.empty() && m_importance == EFieldImportance::NICE_TO_HAVE)
		{
			out_message += "unable to load value for field <" + genPrettyName() + ">: " + loaderMessage;
		}

		return false;
	}
}

template<typename Owner>
inline void TOwnedSdlField<Owner>::toSdl(
	Owner&             owner,
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
