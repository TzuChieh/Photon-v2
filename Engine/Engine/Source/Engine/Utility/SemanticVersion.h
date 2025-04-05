#pragma once

#include <Common/primitive_type.h>

#include <string>
#include <compare>
#include <string_view>

namespace ph
{

/*! @brief Convenient software version handling routines.
See https://semver.org/ for a detailed explaination of the concept.
*/
class SemanticVersion final
{
public:
	SemanticVersion();
	SemanticVersion(uint16 major, uint16 minor, uint16 patch);
	explicit SemanticVersion(std::string_view version);

	bool isInitial() const;

	std::string toString() const;

	inline bool operator == (const SemanticVersion& other) const = default;
	std::strong_ordering operator <=> (const SemanticVersion& other) const;

private:
	uint16 m_major;
	uint16 m_minor;
	uint16 m_patch;
};

// In-header Implementations:

inline SemanticVersion::SemanticVersion() :
	SemanticVersion(0, 0, 0)
{}

inline SemanticVersion::SemanticVersion(const uint16 major, const uint16 minor, const uint16 patch) : 
	m_major(major), m_minor(minor), m_patch(patch)
{}

inline bool SemanticVersion::isInitial() const
{
	return m_major == 0 && m_minor == 0 && m_patch == 0;
}

inline std::strong_ordering SemanticVersion::operator <=> (const SemanticVersion& other) const
{
	if(auto cmp = m_major <=> other.m_major; cmp != 0)
	{
		return cmp;
	}

	if(auto cmp = m_minor <=> other.m_minor; cmp != 0)
	{
		return cmp;
	}

	return m_patch <=> other.m_patch;
}

}// end namespace ph
