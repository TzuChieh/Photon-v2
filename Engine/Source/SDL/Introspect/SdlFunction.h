#pragma once

#include "Common/assertion.h"
#include "Common/logging.h"

#include <string>
#include <utility>

namespace ph
{

PH_DEFINE_EXTERNAL_LOG_GROUP(SdlFunction, SDL);

class SdlField;
class SdlInputContext;
class SdlInputClauses;
class ISdlResource;

class SdlFunction
{
public:
	explicit SdlFunction(std::string name);
	virtual ~SdlFunction() = default;

	virtual void call(
		ISdlResource*          resource,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const = 0;

	// TODO: saveCall() & asyncCall() ?

	virtual std::size_t numParams() const = 0;
	virtual const SdlField* getParam(std::size_t index) const = 0;

	std::string genPrettyName() const;
	const std::string& getName() const;
	const std::string& getDescription() const;

protected:
	SdlFunction& setDescription(std::string description);

private:
	std::string m_name;
	std::string m_description;
};

// In-header Implementations:

inline SdlFunction::SdlFunction(std::string name) : 
	m_name       (std::move(name)),
	m_description()
{
	PH_ASSERT(!m_name.empty());
}

inline const std::string& SdlFunction::getName() const
{
	return m_name;
}

inline const std::string& SdlFunction::getDescription() const
{
	return m_description;
}

inline SdlFunction& SdlFunction::setDescription(std::string description)
{
	m_description = std::move(description);

	return *this;
}

}// end namespace ph
