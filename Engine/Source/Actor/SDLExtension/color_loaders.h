#pragma once

#include "Math/TVector3.h"
#include "DataIO/FileSystem/Path.h"

#include <memory>
#include <string>

namespace ph { class Image; }

namespace ph::sdl { class SdlPayload; }

namespace ph::sdl
{

enum class ESdlColorSpace
{
	UNSPECIFIED,
	SRGB,
	LINEAR_SRGB,
	SPECTRAL
};

ESdlColorSpace to_color_space(const std::string_view sdlTag);

Spectrum load_spectrum(const SdlPayload& payload);
//std::shared_ptr<Image> load_spectral_color(const Spectrum& values);
std::shared_ptr<Image> load_constant_color(const math::Vector3R& values);
std::shared_ptr<Image> load_picture_color(const Path& filePath);

// In-header Implementations:

inline ESdlColorSpace to_color_space(const std::string_view sdlTag)
{
	if(sdlTag.empty())
	{
		return ESdlColorSpace::UNSPECIFIED;
	}

	if(sdlTag == "SRGB")
	{
		return ESdlColorSpace::SRGB;
	}
	else if(sdlTag == "LSRGB")
	{
		return ESdlColorSpace::LINEAR_SRGB;
	}
	else if(sdlTag == "SPD")
	{
		return ESdlColorSpace::SPECTRAL;
	}
	else
	{
		// throw
	}
}

}// end namespace ph::sdl
