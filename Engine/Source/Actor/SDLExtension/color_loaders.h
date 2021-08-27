#pragma once

#include "Math/TVector3.h"
#include "DataIO/FileSystem/Path.h"
#include "Actor/SDLExtension/ESdlColorSpace.h"
#include "Core/Quantity/EQuantity.h"
#include "Core/Quantity/Spectrum.h"

#include <memory>
#include <string>

namespace ph { class Image; }
namespace ph { class SdlInputPayload; }

namespace ph::sdl
{

math::Vector3R tristimulus_to_linear_SRGB(const math::Vector3R& tristimulus, ESdlColorSpace colorSpace);

Spectrum load_spectrum(const SdlInputPayload& payload, EQuantity usage);

//std::shared_ptr<Image> load_spectral_color(const Spectrum& spectrum);
std::shared_ptr<Image> load_tristimulus_color(const math::Vector3R& tristimulus, ESdlColorSpace colorSpace, EQuantity usage);
std::shared_ptr<Image> load_picture_color(const Path& filePath);

}// end namespace ph::sdl
