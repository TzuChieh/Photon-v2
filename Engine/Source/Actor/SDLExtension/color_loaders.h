#pragma once

#include "Math/TVector3.h"
#include "DataIO/FileSystem/Path.h"

#include <memory>
#include <string>

namespace ph
{

class Image;

}// end namespace ph

namespace ph::sdl
{

math::Vector3R load_constant_values(const std::string& sdlValue);
std::shared_ptr<Image> load_constant_color(const math::Vector3R& values);
std::shared_ptr<Image> load_picture_color(const Path& filePath);

}// end namespace ph::sdl
