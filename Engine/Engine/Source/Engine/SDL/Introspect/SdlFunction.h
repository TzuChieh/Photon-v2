#pragma once

#include "Engine/SDL/Introspect/ISdlInstantiable.h"
#include "Engine/SDL/sdl_fwd.h"

#include <Common/assertion.h>
#include <Common/logging.h>

#include <string>
#include <utility>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(SdlFunction, SDL);

class SdlFunction : public ISdlInstantiable
{
public:
	SdlFunction();

	virtual void call(
		ISdlResource*          resource,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const = 0;

	// TODO: saveCall() & asyncCall() ?

	virtual std::size_t numParams() const = 0;
	virtual const SdlField* getParam(std::size_t index) const = 0;

	std::size_t numFields() const override;
	const SdlField* getField(std::size_t index) const override;
	std::string_view getTypeName() const override;
	std::string_view getDescription() const override;

	/*!
	@return Function name.
	*/
	std::string_view getName() const;

	std::string genPrettyName() const;

protected:
	SdlFunction& setName(std::string name);
	SdlFunction& setDescription(std::string description);

private:
	std::string m_name;
	std::string m_description;
};

// In-header Implementations:

inline SdlFunction::SdlFunction() : 
	m_name       (),
	m_description()
{}

inline std::size_t SdlFunction::numFields() const
{
	return numParams();
}

inline const SdlField* SdlFunction::getField(std::size_t index) const
{
	return getParam(index);
}

inline std::string_view SdlFunction::getTypeName() const
{
	return getName();
}

inline std::string_view SdlFunction::getName() const
{
	PH_ASSERT(!m_name.empty());
	return m_name;
}

inline std::string_view SdlFunction::getDescription() const
{
	return m_description;
}

inline SdlFunction& SdlFunction::setName(std::string name)
{
	m_name = std::move(name);

	return *this;
}

inline SdlFunction& SdlFunction::setDescription(std::string description)
{
	m_description = std::move(description);

	return *this;
}

}// end namespace ph
