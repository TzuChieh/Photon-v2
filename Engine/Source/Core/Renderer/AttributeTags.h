#pragma once

#include "Core/Quantity/EAttribute.h"
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
	void tag(const AttributeTags& targets);
	void untag(EAttribute target);
	bool isTagged(EAttribute target) const;

private:
	using Set = std::array<bool, static_cast<std::size_t>(EAttribute::SIZE)>;

	Set m_set;

	void setTag(std::size_t targetIndex, bool isOn);
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
	setTag(targetToIndex(target), true);
}

inline void AttributeTags::tag(const AttributeTags& targets)
{
	for(std::size_t i = 0; i < targets.m_set.size(); i++)
	{
		setTag(i, m_set[i] || targets.m_set[i]);
	}
}

inline void AttributeTags::untag(const EAttribute target)
{
	setTag(targetToIndex(target), false);
}

inline bool AttributeTags::isTagged(const EAttribute target) const
{
	return m_set[targetToIndex(target)];
}

inline void AttributeTags::setTag(const std::size_t targetIndex, bool isOn)
{
	PH_ASSERT(targetIndex < std::tuple_size_v<Set>);

	m_set[targetIndex] = isOn;
}

inline std::size_t AttributeTags::targetToIndex(const EAttribute target)
{
	return static_cast<std::size_t>(target);
}

}// end namespace ph