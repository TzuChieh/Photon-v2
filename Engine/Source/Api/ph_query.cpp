#include "ph_query.h"
#include "Api/ApiDatabase.h"
#include "Core/Engine.h"
#include "Core/Camera/Film.h"

#include <iostream>

void phQueryRendererPercentageProgress(const PHuint64 engineId, PHfloat32* const out_percentage)
{
	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
	if(engine)
	{
		*out_percentage = engine->queryPercentageProgress();
	}
}

void phQueryRendererSampleFrequency(const PHuint64 engineId, PHfloat32* const out_frequency)
{
	using namespace ph;

	Engine* engine = ApiDatabase::getEngine(engineId);
	if(engine)
	{
		*out_frequency = engine->querySampleFrequency();
	}
}