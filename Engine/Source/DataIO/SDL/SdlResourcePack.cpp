#include "DataIO/SDL/SdlResourcePack.h"
#include "DataIO/SDL/SdlParser.h"
#include "Core/SampleGenerator/SampleGenerator.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "Common/Logger.h"

#include <iostream>

namespace ph
{

namespace
{
	Logger logger(LogSender("SDL Resource Pack"));
}

SdlResourcePack::SdlResourcePack() :
	data()
{}

}// end namespace ph
