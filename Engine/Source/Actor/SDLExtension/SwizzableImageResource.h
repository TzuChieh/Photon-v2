#pragma once

#include "Actor/Image/Image.h"

#include <memory>
#include <array>
#include <string_view>

namespace ph
{

class SwizzableImageResource final
{
public:
	SwizzableImageResource(std::shared_ptr<Image> image, std::string_view swizzleMap);

private:
	std::shared_ptr<Image>                      m_image;
	std::array<char, Image::NUMERIC_ARRAY_SIZE> m_swizzleMap;
};

}// end namespace ph
