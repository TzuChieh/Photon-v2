#include "Core/Filmic/TSamplingFilm.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"
#include "Math/Function/TConstant2D.h"
#include "Math/Function/TGaussian2D.h"
#include "Core/Filmic/SampleFilter.h"
#include "Common/assertion.h"

#include <cstddef>
#include <iostream>
#include <algorithm>

namespace ph
{

template<typename Sample>
inline std::unique_ptr<SamplingFilmBase> TSamplingFilm<Sample>::genChild(const TAABB2D<int64>& effectiveWindowPx)
{
	return genSamplingChild(effectiveWindowPx);
}

}// end namespace ph