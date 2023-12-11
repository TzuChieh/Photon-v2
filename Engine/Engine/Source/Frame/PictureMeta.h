#pragma once

#include "Math/Color/color_basics.h"
#include "Math/TVector2.h"
#include "Utility/TSpan.h"

#include <Common/assertion.h>

#include <cstddef>
#include <vector>
#include <string>

namespace ph
{

class PictureMeta final
{
public:
	std::size_t numLayers() const;
	std::size_t numChannels(std::size_t layerIdx = 0) const;

	std::size_t addDefaultLayer();
	std::size_t addEmptyLayer();

	/*! @brief Access to the name of the picture.
	*/
	///@{
	std::string& pictureName();
	const std::string& getPictureName() const;
	///@}

	/*! @brief Access to the name of a layer.
	*/
	///@{
	std::string& name(std::size_t layerIdx = 0);
	const std::string& getName(std::size_t layerIdx = 0) const;
	///@}

	math::Vector2S& sizePx(std::size_t layerIdx = 0);
	math::EColorSpace& colorSpace(std::size_t layerIdx = 0);

	/*! @brief Set the number of channels.
	*/
	void setNumChannels(std::size_t numChannels, std::size_t layerIdx = 0);

	/*! @brief Access to the name of a channel.
	Some formats have the ability to give names to their color/data channels. If so, channel names
	will be utilized and a channel can be ignored by giving it an empty name. Set the number of
	channels by `setNumChannels()`. 
	*/
	///@{
	TSpan<std::string> channelNames(std::size_t layerIdx = 0);
	TSpanView<std::string> getChannelNames(std::size_t layerIdx = 0) const;
	///@}

	const math::Vector2S& getSizePx(std::size_t layerIdx = 0) const;
	const math::EColorSpace& getColorSpace(std::size_t layerIdx = 0) const;

private:
	struct Layer
	{
		std::string name;
		math::Vector2S sizePx = {0, 0};
		math::EColorSpace colorSpace = math::EColorSpace::sRGB;
		std::vector<std::string> channelNames = {"R", "G", "B"};
	};

	auto getLayer(std::size_t layerIdx) -> Layer&;
	auto getLayer(std::size_t layerIdx) const -> const Layer&;

	std::string m_pictureName;
	std::vector<Layer> m_layers;
};

inline std::size_t PictureMeta::numLayers() const
{
	return m_layers.size();
}

inline std::size_t PictureMeta::numChannels(std::size_t layerIdx) const
{
	return getLayer(layerIdx).channelNames.size();
}

inline std::size_t PictureMeta::addDefaultLayer()
{
	m_layers.push_back(Layer{});
	return m_layers.size() - 1;
}

inline std::size_t PictureMeta::addEmptyLayer()
{
	Layer layer;
	layer.name = "";
	layer.sizePx = {0, 0};
	layer.colorSpace = math::EColorSpace::Unspecified;
	layer.channelNames.clear();

	m_layers.push_back(layer);
	return m_layers.size() - 1;
}

inline std::string& PictureMeta::pictureName()
{
	return m_pictureName;
}

inline std::string& PictureMeta::name(std::size_t layerIdx)
{
	return getLayer(layerIdx).name;
}

inline math::Vector2S& PictureMeta::sizePx(std::size_t layerIdx)
{
	return getLayer(layerIdx).sizePx;
}
inline math::EColorSpace& PictureMeta::colorSpace(std::size_t layerIdx)
{
	return getLayer(layerIdx).colorSpace;
}

inline void PictureMeta::setNumChannels(std::size_t numChannels, std::size_t layerIdx)
{
	getLayer(layerIdx).channelNames.resize(numChannels, "");
}

inline TSpan<std::string> PictureMeta::channelNames(std::size_t layerIdx)
{
	return getLayer(layerIdx).channelNames;
}

inline const std::string& PictureMeta::getPictureName() const
{
	return m_pictureName;
}

inline const std::string& PictureMeta::getName(std::size_t layerIdx) const
{
	return getLayer(layerIdx).name;
}

inline const math::Vector2S& PictureMeta::getSizePx(std::size_t layerIdx) const
{
	return getLayer(layerIdx).sizePx;
}

inline const math::EColorSpace& PictureMeta::getColorSpace(std::size_t layerIdx) const
{
	return getLayer(layerIdx).colorSpace;
}

inline TSpanView<std::string> PictureMeta::getChannelNames(std::size_t layerIdx) const
{
	return getLayer(layerIdx).channelNames;
}

inline auto PictureMeta::getLayer(std::size_t layerIdx) -> Layer&
{
	PH_ASSERT_LT(layerIdx, m_layers.size());
	return m_layers[layerIdx];
}

inline auto PictureMeta::getLayer(std::size_t layerIdx) const -> const Layer&
{
	PH_ASSERT_LT(layerIdx, m_layers.size());
	return m_layers[layerIdx];
}

}// end namespace ph
