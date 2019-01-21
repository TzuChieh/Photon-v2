#pragma once

#include "Utility/TFixedEnumMap.h"
#include "Core/Renderer/EAttribute.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Math/TVector3.h"

#include <type_traits>

namespace ph
{

class Estimation final
{
public:
	template<EAttribute TAG>
	decltype(auto) get() const;

	template<EAttribute TAG, typename EstimationData>
	void set(const EstimationData& data);

private:
	using TagToEstimationMap = TFixedEnumMap<
		TFixedEnumMapEntry<EAttribute::LIGHT_ENERGY, SpectralStrength>,
		TFixedEnumMapEntry<EAttribute::NORMAL,       Vector3R>
	>;

	TagToEstimationMap m_tagToEstimation;
};

// In-header Implementations:

template<EAttribute TAG>
decltype(auto) Estimation::get() const
{
	return m_tagToEstimation.get<TAG>();
}

template<EAttribute TAG, typename EstimationData>
void Estimation::set(const EstimationData& data)
{
	static_assert(std::is_convertible_v<
		EstimationData,
		typename TagToEstimationMap::Entry<TagToEstimationMap::entryIndex<TAG>()>::Value>);

	m_tagToEstimation.get<TAG>() = data;
}

}// end namespace ph