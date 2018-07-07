#pragma once

#include "Core/Filmic/TSamplingFilm.h"
#include "Core/Estimator/Attribute/EAttribute.h"
#include "Utility/TFixedEnumMap.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"

#include <memory>
#include <array>
#include <cstddef>

namespace ph
{

class SamplingFilmSet final
{
public:
	SamplingFilmSet();

	SamplingFilmBase* get(EAttribute tag);
	
	template<EAttribute TAG>
	decltype(auto) get() const
	{
		return m_tagToFilm.get<TAG>();
	}

	template<EAttribute TAG, typename Sample>
	void set(const std::shared_ptr<TSamplingFilm<Sample>>& film)
	{
		m_tagToFilm.get<TAG>()                   = film;
		m_films[TagToFilmMap::entryIndex<TAG>()] = film.get();
	}

private:
	using SpectralStrengthFilm = std::shared_ptr<TSamplingFilm<SpectralStrength>>;
	using Vector3Film          = std::shared_ptr<TSamplingFilm<Vector3R>>;

	using TagToFilmMap = TFixedEnumMap<
		TFixedEnumMapEntry<EAttribute::LIGHT_ENERGY, SpectralStrengthFilm>,
		TFixedEnumMapEntry<EAttribute::NORMAL,       Vector3Film>
	>;
	TagToFilmMap m_tagToFilm;

	std::array<
		SamplingFilmBase*, 
		TagToFilmMap::ENTRY_ARRAY_SIZE
	> m_films;
};

// In-header Implementations:

SamplingFilmBase* SamplingFilmSet::get(const EAttribute tag)
{
	return m_films
}

}// end namespace ph