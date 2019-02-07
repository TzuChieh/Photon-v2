#pragma once

//#include "Common/assertion.h"
//#include "Core/Filmic/filmic_fwd.h"
//#include "Common/primitive_type.h"
//#include "Math/TVector2.h"
//#include "Core/Bound/TAABB2D.h"
//
//#include <functional>
//
//namespace ph
//{
//
//template<typename Sample>
//class TMergeableFilmProxy
//{
//public:
//	TMergeableFilmProxy();
//	TMergeableFilmProxy(TSamplingFilm<Sample>* film, std::function<void()> merger);
//
//	void merge();
//	void addSample(float64 xPx, float64 yPx, const Sample& sample);
//	void clear();
//
//	TVector2<int64> getActualResPx() const;
//	TVector2<int64> getEffectiveResPx() const;
//	TVector2<float64> getSampleResPx() const;
//	TAABB2D<int64> getEffectiveWindowPx() const;
//	TAABB2D<float64> getSampleWindowPx() const;
//
//private:
//	TSamplingFilm<Sample>* m_film;
//	std::function<void()>  m_merger;
//};
//
//}// end namespace ph
//
//#include "Core/Filmic/TMergeableFilmProxy.ipp"