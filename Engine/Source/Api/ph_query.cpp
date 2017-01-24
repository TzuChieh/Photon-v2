#include "ph_query.h"
#include "Api/ApiDatabase.h"
#include "Core/Renderer.h"
#include "Filmic/Film.h"

#include <iostream>

void phQueryRendererPercentageProgress(const PHuint64 rendererId, PHfloat32* const out_percentage)
{
	using namespace ph;

	Renderer* renderer = ApiDatabase::getRenderer(rendererId);
	if(renderer)
	{
		*out_percentage = renderer->queryPercentageProgress();
	}
}

void phQueryRendererSampleFrequency(const PHuint64 rendererId, PHfloat32* const out_frequency)
{
	using namespace ph;

	Renderer* renderer = ApiDatabase::getRenderer(rendererId);
	if(renderer)
	{
		*out_frequency = renderer->querySampleFrequency();
	}
}