#pragma once

#include "Frame/PictureData.h"
#include "Common/primitive_type.h"
#include "Math/Color/color_basics.h"
#include "Math/TVector2.h"

#include <cstddef>

namespace ph
{

/*! @brief Pixel format of picture.
The characters RGB does not mean the pixel is in RGB color space. It simply represents pixel components, 
using R, G, B as placeholders.
*/
enum class EPicturePixelFormat
{
	Unspecified = 0,

	PPF_Grayscale_8,
	PPF_RGB_8,
	PPF_RGBA_8,

	PPF_Grayscale_16F,
	PPF_RGB_16F,
	PPF_RGBA_16F,

	PPF_Grayscale_32F,
	PPF_RGB_32F,
	PPF_RGBA_32F
};

class RegularPicture final
{
public:
	/*! @brief Creates an empty picture.
	*/
	RegularPicture();

	/*! @brief Creates a picture with allocated picture data.
	Pixel data can to be set later by calling `pictureData.setPixels()`.
	*/
	RegularPicture(
		math::Vector2S sizePx,
		std::size_t numComponents,
		EPicturePixelComponent componentType);

	std::size_t numComponents() const;
	bool hasAlpha() const;
	bool isLDR() const;
	bool isHDR() const;
	EPicturePixelFormat getNativeFormat() const;
	math::EColorSpace getColorSpace() const;
	bool isReversedComponents() const;
	math::Vector2S getSizePx() const;
	std::size_t getWidthPx() const;
	std::size_t getHeightPx() const;

	void setNativeFormat(EPicturePixelFormat format);
	void setComponentReversed(bool isReversed);
	void setColorSpace(math::EColorSpace colorSpace);
	
	PictureData& getPixels();
	const PictureData& getPixels() const;

	// TODO: do we need flag for different alpha order? e.g., ARGB vs RGBA. Do such format exist?

private:
	static bool isLDR(EPicturePixelFormat format);
	static bool isHDR(EPicturePixelFormat format);
	static std::size_t numComponents(EPicturePixelFormat format);
	static bool hasAlpha(EPicturePixelFormat format);

	/*! @brief Data type of the picture when stored natively (e.g., on disk). */
	EPicturePixelFormat m_nativeFormat;

	/*! @brief Color space of the loaded picture. */
	math::EColorSpace m_colorSpace;

	/*! @brief Whether the pixel components of the frame is stored reversely, e.g, in ABGR order. */
	bool m_isReversedComponents;

	/*! @brief Storage of actual pixel data. */
	PictureData m_pictureData;
};

inline EPicturePixelFormat RegularPicture::getNativeFormat() const
{
	return m_nativeFormat;
}

inline math::EColorSpace RegularPicture::getColorSpace() const
{
	return m_colorSpace;
}

inline bool RegularPicture::isReversedComponents() const
{
	return m_isReversedComponents;
}

inline math::Vector2S RegularPicture::getSizePx() const
{
	return m_pictureData.getSizePx();
}

inline std::size_t RegularPicture::getWidthPx() const
{
	return m_pictureData.getWidthPx();
}

inline std::size_t RegularPicture::getHeightPx() const
{
	return m_pictureData.getHeightPx();
}

inline PictureData& RegularPicture::getPixels()
{
	return m_pictureData;
}

inline const PictureData& RegularPicture::getPixels() const
{
	return m_pictureData;
}

}// end namespace ph
