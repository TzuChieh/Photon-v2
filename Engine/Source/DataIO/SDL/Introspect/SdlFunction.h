#pragma once

#include "Common/Logger.h"
#include "Common/assertion.h"

#include <string>
#include <utility>

namespace ph
{

class SdlField;
class SdlInputContext;
class ValueClauses;
class ISdlResource;

class SdlFunction
{
public:
	explicit SdlFunction(std::string name);
	virtual ~SdlFunction() = default;

	virtual void call(
		ISdlResource*          resource,
		ValueClauses&          clauses,
		const SdlInputContext& ctx) const = 0;

	// TODO: saveCall() & asyncCall() ?

	virtual std::size_t numParams() const = 0;
	virtual const SdlField* getParam(std::size_t index) const = 0;

	std::string genPrettyName() const;
	const std::string& getName() const;
	const std::string& getDescription() const;

protected:
	SdlFunction& setDescription(std::string description);

	static const Logger logger;

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
