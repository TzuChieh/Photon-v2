#pragma once

#include <memory>
#include <string>

namespace ph
{

class Image;

}// end namespace ph

namespace ph::sdl
{

std::shared_ptr<Image> load_constant_color(const std::string& sdlValue);

}// end namespace ph::sdl
