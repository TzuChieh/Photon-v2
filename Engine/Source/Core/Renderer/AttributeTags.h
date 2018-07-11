#pragma once

#include "Core/Renderer/EAttribute.h"
#include "Common/assertion.h"

#include <array>
#include <tuple>

namespace ph
{

class AttributeTags final
{
public:
	AttributeTags();

	void tag(EAttribute target);
	void untag(EAttribute target);
	bool isTagged(EAttribute target) const;

private:
	using Set = std::array<bool, static_cast<std::size_t>(EAttribute::SIZE)>;

	Set m_set;

	void setTag(EAttribute target, bool isOn);
	static std::size_t targetToIndex(EAttribute target);
};

// In-header Implementations:

inline AttributeTags::AttributeTags() : 
	m_set()
{
	m_set.fill(false);
}

inline void AttributeTags::tag(const EAttribute target)
{
	setTag(target, true);
}

inline void AttributeTags::untag(const EAttribute target)
{
	setTag(target, false);
}

inline bool AttributeTags::isTagged(const EAttribute target) const
{
	return m_set[targetToIndex(target)];
}

inline void AttributeTags::setTag(const EAttribute target, const bool isOn)
{
	m_set[targetToIndex(target)] = isOn;
}

inline std::size_t AttributeTags::targetToIndex(const EAttribute target)
{
	PH_ASSERT(static_cast<std::size_t>(target) < std::tuple_size_v<Set>);

	return static_cast<std::size_t>(target);
}

}// end namespace ph