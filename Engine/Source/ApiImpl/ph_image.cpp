#pragma once

#include "ph_image.h"
#include "ApiImpl/ApiDatabase.h"
#include "Image/HDRFrame.h"

#include <iostream>

void phCreateHdrFrame(PHuint64* out_frameId, const PHuint32 widthPx, const PHuint32 heightPx)
{
	*out_frameId = ph::ApiDatabase::addHdrFrame(ph::HDRFrame(widthPx, heightPx));

	std::cout << "HDRFrame<" << *out_frameId << "> created" << std::endl;
}

void phDeleteHdrFrame(const PHuint64 frameId)
{
	if(ph::ApiDatabase::removeHdrFrame(frameId))
	{
		std::cout << "HDRFrame<" << frameId << "> deleted" << std::endl;
	}
	else
	{
		std::cout << "error while deleting HDRFrame<" << frameId << ">" << std::endl;
	}
}