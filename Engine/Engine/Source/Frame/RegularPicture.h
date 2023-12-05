#pragma once

#include "Frame/picture_basics.h"
#include "Frame/PictureData.h"
#include "Math/Color/color_basics.h"
#include "Math/TVector2.h"

#include <Common/primitive_type.h>

#include <cstddef>

namespace ph
{

/*! @brief Format of common pictures.
*/
class RegularPictureFormat final
{
public:
	RegularPictureFormat();

	math::EColorSpace getColorSpace() const;
	bool isReversedComponents() const;
	bool hasAlpha() const;
	bool isGrayscale() const;

	void setColorSpace(math::EColorSpace colorSpace);
	void setIsReversedComponents(bool isReversedComponents);
	void setHasAlpha(bool hasAlpha);
	void setIsGrayscale(bool isGrayscale);

private:
	/*! @brief Color space of the loaded picture. */
	math::EColorSpace m_colorSpace;

	/*! @brief Whether the pixel components of the frame is stored reversely, e.g, in ABGR order. */
	bool m_isReversedComponents;

	/*! @brief Whether there is an alpha channel. */
	bool m_hasAlpha;

	/*! @brief Whether the picture is in grayscale. */
	bool m_isGrayscale;
};

/*! @brief Raw representation of common picture types.
*/
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

	RegularPicture(RegularPicture&& other);

	const RegularPictureFormat& getFormat() const;
	void setFormat(const RegularPictureFormat& format);
	std::size_t numComponents() const;
	EPicturePixelComponent getComponentType() const;
	bool isLDR() const;
	bool isHDR() const;
	math::Vector2S getSizePx() const;
	std::size_t getWidthPx() const;
	std::size_t getHeightPx() const;
	
	PictureData& getPixels();
	const PictureData& getPixels() const;

	RegularPicture& operator = (RegularPicture&& rhs);

private:
	static bool isLDR(EPicturePixelComponent componentType);
	static bool isHDR(EPicturePixelComponent componentType);

	/*! @brief Format of the picture. Typically closely related to how it is stored natively (e.g., on disk). */
	RegularPictureFormat m_format;

	/*! @brief Storage of actual pixel data. */
	PictureData m_pictureData;
};

inline math::EColorSpace RegularPictureFormat::getColorSpace() const
{
	return m_colorSpace;
}

inline bool RegularPictureFormat::isReversedComponents() const
{
	return m_isReversedComponents;
}

inline bool RegularPictureFormat::hasAlpha() const
{
	return m_hasAlpha;
}

inline bool RegularPictureFormat::isGrayscale() const
{
	return m_isGrayscale;
}

inline const RegularPictureFormat& RegularPicture::getFormat() const
{
	return m_format;
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

inline std::size_t RegularPicture::numComponents() const
{
	return m_pictureData.numComponents();
}

inline EPicturePixelComponent RegularPicture::getComponentType() const
{
	return m_pictureData.getComponentType();
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
