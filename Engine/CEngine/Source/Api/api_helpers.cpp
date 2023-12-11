#include "Api/api_helpers.h"

#include <Frame/PictureMeta.h>

#include <cstddef>

namespace ph
{

PictureMeta make_picture_meta(const PhFrameSaveInfo& saveInfo)
{
	PictureMeta meta;
	const auto layerIdx = meta.addEmptyLayer();

	meta.setNumChannels(saveInfo.numChannels, layerIdx);
	for(std::size_t channelIdx = 0; channelIdx < saveInfo.numChannels; ++channelIdx)
	{
		meta.channelNames(layerIdx)[channelIdx] = saveInfo.channelNames[channelIdx];
	}

	return meta;
}

}// end namespace ph
