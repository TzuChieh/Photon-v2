#pragma once

#include "DataIO/SDL/Introspect/SdlField.h"
#include "Common/assertion.h"

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

	bool fromSdl(
		Owner&             owner, 
		const std::string& sdl,
		std::string&       out_message);

	void toSdl(
		Owner&       owner, 
		std::string* out_sdl,
		std::string& out_message) const;

	TOwnedSdlField& withImportance(EFieldImportance importance);

private:
	virtual bool loadFromSdl(
		Owner&             owner, 
		const std::string& sdl,
		std::string&       out_loaderMessage) = 0;

	virtual void convertToSdl(
		Owner&       owner, 
		std::string* out_sdl,
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
inline bool TOwnedSdlField<Owner>::fromSdl(
	Owner&             owner,
	const std::string& sdl,
	std::string&       out_message)
{
	std::string loaderMessage;
	if(loadFromSdl(owner, sdl, loaderMessage))
	{
		// Even if it is a successful load, record loader's message if there is
		// any. This is done for non-optional field only.
		if(!loaderMessage.empty() && m_importance != EFieldImportance::OPTIONAL)
		{
			out_message += "field <" + genFieldDescription() + "> note: " + loaderMessage;
		}

		return true;
	}
	else
	{
		// For required field always emit a message to warn the user on failed
		// loading attempt.
		if(m_importance == EFieldImportance::REQUIRED)
		{
			out_message += "failed to load field <" + genFieldDescription() + ">";
			if(!loaderMessage.empty())
			{
				out_message += ", " + loaderMessage;
			}
		}
		// For non-required field, we will only emit message if the field is
		// non-optional and the loader has something to say.
		else if(!loaderMessage.empty() && m_importance == EFieldImportance::NICE_TO_HAVE)
		{
			out_message += "unable to load field <" + genFieldDescription() + ">: " + loaderMessage;
		}

		return false;
	}
}

template<typename Owner>
inline void TOwnedSdlField<Owner>::toSdl(
	Owner&             owner,
	std::string* const out_sdl,
	std::string&       out_message) const
{
	// TODO
	PH_ASSERT_UNREACHABLE_SECTION();
}

template<typename Owner>
inline TOwnedSdlField<Owner>& TOwnedSdlField<Owner>::withImportance(const EFieldImportance importance)
{
	m_importance = importance;

	return *this;
}

}// end namespace ph
