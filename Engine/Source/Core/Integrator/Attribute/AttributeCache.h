#pragma once

#include "Common/primitive_type.h"
#include "Core/Integrator/Attribute/EAttribute.h"
#include "Core/Integrator/Attribute/TAttributeEntrySet.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

#include <array>
#include <limits>
#include <type_traits>

namespace ph
{

class AttributeCache final
{
public:
	template<typename Value>
	bool hasEntry(EAttribute type) const;

	template<typename Value>
	TAttributeEntry<Value>* getEntry(EAttribute type);

	template<typename Value>
	const TAttributeEntry<Value>& getEntry(EAttribute type) const;

private:
	TAttributeEntrySet<SpectralStrength> m_spectralEntries;
	TAttributeEntrySet<Vector3R>         m_vector3Entries;
	TAttributeEntrySet<real>             m_realEntries;
	TAttributeEntrySet<int64>            m_integerEntries;
};

// Implementations:

template<typename Value>
inline bool AttributeCache::hasEntry(const EAttribute type) const
{
	return getEntry<Value>(type).hasHandler();
}

template<typename Value>
inline TAttributeEntry<Value>* AttributeCache::getEntry(const EAttribute type)
{
	if constexpr(std::is_same_v<Value, SpectralStrength>)
	{
		return m_spectralEntries.getEntry(type);
	}
	else if constexpr(std::is_same_v<Value, Vector3R>)
	{
		return m_vector3Entries.getEntry(type);
	}
	else if constexpr(std::is_same_v<Value, real>)
	{
		return m_realEntries.getEntry(type);
	}
	else if constexpr(std::is_same_v<Value, int64>)
	{
		return m_integerEntries.getEntry(type);
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

template<typename Value>
inline const TAttributeEntry<Value>& AttributeCache::getEntry(const EAttribute type) const
{
	if constexpr(std::is_same_v<Value, SpectralStrength>)
	{
		return m_spectralEntries.getEntry(type);
	}
	else if constexpr(std::is_same_v<Value, Vector3R>)
	{
		return m_vector3Entries.getEntry(type);
	}
	else if constexpr(std::is_same_v<Value, real>)
	{
		return m_realEntries.getEntry(type);
	}
	else if constexpr(std::is_same_v<Value, int64>)
	{
		return m_integerEntries.getEntry(type);
	}
	else
	{
		PH_ASSERT_UNREACHABLE_SECTION();
	}
}

}// end namespace ph