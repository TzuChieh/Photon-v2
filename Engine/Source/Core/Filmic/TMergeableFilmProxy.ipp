#pragma once

//#include "Core/Filmic/TMergeableFilmProxy.h"
//#include "Core/Filmic/TSamplingFilm.h"
//
//namespace ph
//{
//
//template<typename Sample>
//inline TMergeableFilmProxy<Sample>::TMergeableFilmProxy() :
//	m_film  (nullptr),
//	m_merger(nullptr)
//{}
//
//template<typename Sample>
//inline TMergeableFilmProxy<Sample>::TMergeableFilmProxy(
//	TSamplingFilm<Sample>* const film,
//	std::function<void()>        merger) :
//
//	m_film  (film),
//	m_merger(std::move(merger))
//{
//	PH_ASSERT(film);
//	PH_ASSERT(merger);
//}
//
//template<typename Sample>
//inline void TMergeableFilmProxy<Sample>::merge()
//{
//	m_merger();
//}
//
//template<typename Sample>
//inline void TMergeableFilmProxy<Sample>::addSample(
//	const float64 xPx, 
//	const float64 yPx, 
//	const Sample& sample)
//{
//	m_film->addSample(xPx, yPx, sample);
//}
//
//template<typename Sample>
//inline void TMergeableFilmProxy<Sample>::clear()
//{
//	m_film->clear();
//}
//
//template<typename Sample>
//inline TVector2<int64> TMergeableFilmProxy<Sample>::getActualResPx() const
//{
//	return m_film->getActualResPx();
//}
//
//template<typename Sample>
//inline TVector2<int64> TMergeableFilmProxy<Sample>::getEffectiveResPx() const
//{
//	return m_film->getEffectiveResPx();
//}
//
//template<typename Sample>
//inline TVector2<float64> TMergeableFilmProxy<Sample>::getSampleResPx() const
//{
//	return m_film->getSampleResPx();
//}
//
//template<typename Sample>
//inline TAABB2D<int64> TMergeableFilmProxy<Sample>::getEffectiveWindowPx() const
//{
//	return m_film->getEffectiveWindowPx();
//}
//
//template<typename Sample>
//inline TAABB2D<float64> TMergeableFilmProxy<Sample>::getSampleWindowPx() const
//{
//	return m_film->getSampleWindowPx();
//}
//
//}// end namespace ph