#pragma once

#include "Core/Filmic/TSamplingFilm.h"
#include "Core/Estimator/Attribute/EAttribute.h"
#include "Utility/TFixedEnumMap.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "Utility/INoncopyable.h"

#include <memory>
#include <array>
#include <cstddef>
#include <type_traits>

namespace ph
{

class SamplingFilmSet final : public INoncopyable
{
public:
	SamplingFilmSet();
	SamplingFilmSet(SamplingFilmSet&& other);

	SamplingFilmBase* get(EAttribute tag);
	
	template<EAttribute TAG>
	decltype(auto) get() const;

	template<EAttribute TAG, typename FilmResource>
	void set(FilmResource film);

	template<std::size_t D_INDEX = 0>
	SamplingFilmSet genChild(const TAABB2D<int64>& effectiveWindowPx);

	SamplingFilmSet& operator = (SamplingFilmSet&& rhs);

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

inline SamplingFilmBase* SamplingFilmSet::get(const EAttribute tag)
{
	switch(tag)
	{
	case EAttribute::LIGHT_ENERGY: 
		return m_films[TagToFilmMap::entryIndex<EAttribute::LIGHT_ENERGY>()];
		break;

	case EAttribute::NORMAL:
		return m_films[TagToFilmMap::entryIndex<EAttribute::NORMAL>()];
		break;

	default:
		return nullptr;
		break;
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return nullptr;
}

template<EAttribute TAG>
inline decltype(auto) SamplingFilmSet::get() const
{
	return m_tagToFilm.get<TAG>();
}

template<EAttribute TAG, typename FilmResource>
inline void SamplingFilmSet::set(FilmResource film)
{
	static_assert(std::is_convertible_v<
		FilmResource, 
		TagToFilmMap::Entry<TagToFilmMap::entryIndex<TAG>()>::Value>);

	m_tagToFilm.get<TAG>()                   = std::move(film);
	m_films[TagToFilmMap::entryIndex<TAG>()] = m_tagToFilm.get<TAG>().get();
}

template<std::size_t D_INDEX>
inline SamplingFilmSet SamplingFilmSet::genChild(const TAABB2D<int64>& effectiveWindowPx)
{
	if constexpr(D_INDEX == TagToFilmMap::ENTRY_ARRAY_SIZE)
	{
		return SamplingFilmSet();
	}
	else
	{
		SamplingFilmSet childSet = genChild<D_INDEX + 1>(effectiveWindowPx);

		auto& parentFilm = m_tagToFilm.getEntry<D_INDEX>().getValue();
		if(parentFilm)
		{
			auto childFilm = parentFilm->genSamplingChild(effectiveWindowPx);
			childSet.set<TagToFilmMap::entryKey<D_INDEX>()>(std::move(childFilm));
		}

		return std::move(childSet);
	}
}

}// end namespace ph