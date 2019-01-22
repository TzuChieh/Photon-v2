#include "Core/Filmic/TSamplingFilm.h"
#include "Math/TVector3.h"
#include "Frame/TFrame.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/Function/TConstant2D.h"
#include "Math/Function/TGaussian2D.h"
#include "Core/Filmic/SampleFilter.h"
#include "Common/assertion.h"

#include <cstddef>
#include <iostream>
#include <algorithm>

namespace ph
{

// command interface

template<typename Sample>
inline TSamplingFilm<Sample>::TSamplingFilm(const InputPacket& packet) : 
	SamplingFilmBase(packet)
{}

template<typename Sample>
inline std::unique_ptr<SamplingFilmBase> TSamplingFilm<Sample>::genChild(const TAABB2D<int64>& effectiveWindowPx)
{
	return genSamplingChild(effectiveWindowPx);
}

template<typename Sample>
inline SdlTypeInfo TSamplingFilm<Sample>::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_FILM, "sampling-film");
}

template<typename Sample>
inline void TSamplingFilm<Sample>::ciRegister(CommandRegister& cmdRegister) 
{}

}// end namespace ph