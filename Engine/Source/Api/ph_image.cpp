#pragma once

#include "ph_core.h"
#include "Api/ApiDatabase.h"
#include "Image/HDRFrame.h"

#include <iostream>
#include <memory>

void phCreateFrame(PHuint64* out_frameId, const PHuint32 frameWidthPx, const PHuint32 frameHeightPx, const PHint32 frameType)
{
	switch(frameType)
	{
	case PH_HDR_FRAME_TYPE:
		*out_frameId = ph::ApiDatabase::addFrame(std::make_unique<ph::HDRFrame>(frameWidthPx, frameHeightPx));
		std::cout << "Frame<" << *out_frameId << "> created" << std::endl;
		break;

	default:
		std::cerr << "unidentified renderer type at phCreateFrame()" << std::endl;
	}
}

void phDeleteFrame(const PHuint64 frameId)
{
	if(ph::ApiDatabase::removeFrame(frameId))
	{
		std::cout << "Frame<" << frameId << "> deleted" << std::endl;
	}
	else
	{
		std::cout << "error while deleting Frame<" << frameId << ">" << std::endl;
	}
}