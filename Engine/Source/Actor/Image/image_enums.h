#pragma once

namespace ph
{

enum class EImageWrapMode
{
	UNSPECIFIED = 0,

	ClampToEdge,
	Repeat
};

enum class EImageSampleMode
{
	UNSPECIFIED = 0,

	Bilinear,
	Nearest,
	Trilinear
};

}// end namespace ph
