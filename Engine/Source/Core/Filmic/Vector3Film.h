#pragma once

#include "Common/primitive_type.h"
#include "Core/Filmic/filmic_fwd.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Math/TVector3.h"
#include "Core/Filmic/HdrRgbFilm.h"

#include <vector>

namespace ph
{

/*
	A film that records 3-D vectors.
*/
class Vector3Film : public TSamplingFilm<Vector3R>
{
public:
	Vector3Film(
		int64 actualWidthPx, int64 actualHeightPx,
		const SampleFilter& filter);

	Vector3Film(
		int64 actualWidthPx, int64 actualHeightPx,
		const TAABB2D<int64>& effectiveWindowPx,
		const SampleFilter& filter);

	void addSample(float64 xPx, float64 yPx, const Vector3R& vec3) override;
	void clear() override;
	void setEffectiveWindowPx(const TAABB2D<int64>& effectiveWindow) override;

private:
	void developRegion(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const override;

	HdrRgbFilm m_film;
};

// In-header Implementations:

inline Vector3Film::Vector3Film(
	int64 actualWidthPx, int64 actualHeightPx,
	const SampleFilter& filter) : 

	TSamplingFilm(actualWidthPx, actualHeightPx, filter),

	m_film(actualWidthPx, actualHeightPx, filter)
{}

inline Vector3Film::Vector3Film(
	int64 actualWidthPx, int64 actualHeightPx,
	const TAABB2D<int64>& effectiveWindowPx,
	const SampleFilter& filter) :

	TSamplingFilm(actualWidthPx, actualHeightPx, effectiveWindowPx, filter),

	m_film(actualWidthPx, actualHeightPx, effectiveWindowPx, filter)
{}

inline void Vector3Film::addSample(const float64 xPx, const float64 yPx, const Vector3R& vec3)
{
	m_film.addSample(xPx, yPx, vec3);
}

inline void Vector3Film::clear()
{
	m_film.clear();
}

//inline std::unique_ptr<TSamplingFilm<Vector3R>> Vector3Film::genSamplingChild(const TAABB2D<int64>& effectiveWindowPx)
//{
//	auto childFilm = std::make_unique<Vector3Film>(
//		getActualResPx().x, getActualResPx().y,
//		effectiveWindowPx,
//		getFilter());
//
//	Vector3Film* child  = childFilm.get();
//	Vector3Film* parent = this;
//	childFilm->setMerger([=]()
//	{
//		PH_ASSERT(child && parent);
//
//		parent->m_film.mergeWith(child->m_film);
//	});
//
//	return childFilm;
//}

inline void Vector3Film::setEffectiveWindowPx(const TAABB2D<int64>& effectiveWindow)
{
	TSamplingFilm::setEffectiveWindowPx(effectiveWindow);

	m_film.setEffectiveWindowPx(effectiveWindow);
}

inline void Vector3Film::developRegion(HdrRgbFrame& out_frame, const TAABB2D<int64>& regionPx) const
{
	m_film.develop(out_frame, regionPx);
}

}// end namespace ph
