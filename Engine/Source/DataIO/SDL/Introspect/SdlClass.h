#pragma once

#include "Common/assertion.h"

#include <vector>
#include <memory>
#include <cstddef>
#include <string>
#include <utility>

namespace ph
{

class SdlField;

class SdlClass
{
public:
	explicit SdlClass(std::string className);
	virtual ~SdlClass() = default;

	virtual std::size_t numFields() const = 0;
	virtual const SdlField* getField(std::size_t index) const = 0;

	const std::string& getClassName() const;
	const std::string& getDescription() const;

	SdlClass& setDescription(std::string description);

private:
	std::string m_className;
	std::string m_description;
};

// In-header Implementation:

inline SdlClass::SdlClass(std::string className) : 
	m_className(std::move(className)), m_description()
{
	PH_ASSERT(!className.empty());
}

inline const std::string& SdlClass::getClassName() const
{
	return m_className;
}

inline const std::string& SdlClass::getDescription() const
{
	return m_description;
}

inline SdlClass& SdlClass::setDescription(std::string description)
{
	m_description = std::move(description);

	return *this;
}

}// end namespace ph
