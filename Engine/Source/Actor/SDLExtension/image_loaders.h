#pragma once

#include "Math/TVector3.h"
#include "DataIO/FileSystem/Path.h"
#include "Math/Color/Spectrum.h"
#include "Actor/SDLExtension/sdl_color_space_type.h"
#include "Actor/SDLExtension/sdl_color_usage_type.h"

#include <memory>
#include <string>

namespace ph { class Image; }
namespace ph { class SdlInputPayload; }

namespace ph::sdl
{

math::Spectrum tristimulus_to_spectrum(const math::Vector3R& tristimulus, math::EColorSpace colorSpace, math::EColorUsage usage);
math::Spectrum load_spectrum(const SdlInputPayload& payload, math::EColorUsage usage);

//std::shared_ptr<Image> load_spectral_color(const Spectrum& spectrum);
std::shared_ptr<Image> load_tristimulus_color(const math::Vector3R& tristimulus, math::EColorSpace colorSpace);
std::shared_ptr<Image> load_picture_file(const Path& filePath);

}// end namespace ph::sdl
