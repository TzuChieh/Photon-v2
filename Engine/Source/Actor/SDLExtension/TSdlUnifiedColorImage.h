#pragma once

#include "SDL/Introspect/TSdlReference.h"
#include "Actor/Image/UnifiedColorImage.h"
#include "Common/assertion.h"
#include "Actor/SDLExtension/image_loaders.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/sdl_helpers.h"
#include "SDL/SdlResourceLocator.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "Math/TVector3.h"
#include "Actor/SDLExtension/sdl_color_space_type.h"

#include <string>
#include <exception>
#include <vector>
#include <memory>
#include <utility>
#include <optional>

namespace ph
{

/*! @brief Interprets general color representations as image resources.
*/
template<typename Owner>
class TSdlUnifiedColorImage : public TSdlReference<UnifiedColorImage, Owner>
{
private:
	using Base = TSdlReference<UnifiedColorImage, Owner>;

public:
	TSdlUnifiedColorImage(
		std::string valueName, 
		std::shared_ptr<UnifiedColorImage> Owner::* imagePtr);

	void setValueToDefault(Owner& owner) const override;

	/*! @brief No default data.
	*/
	TSdlUnifiedColorImage& noDefault();

	/*! @brief Set a monochromatic linear SRGB color for default value.
	*/
	TSdlUnifiedColorImage& defaultLinearSRGB(real linearSRGB);

	/*! @brief Set a linear SRGB color for default value.
	*/
	TSdlUnifiedColorImage& defaultLinearSRGB(const math::Vector3R& linearSRGB);

	/*! @brief Set a specific image resource for default value.
	*/
	TSdlUnifiedColorImage& defaultImage(std::shared_ptr<Image> image);

protected:
	void loadFromSdl(
		Owner&                 owner,
		const SdlInputClause&  clause,
		const SdlInputContext& ctx) const override;

	// TODO: save

private:
	UnifiedColorImage* getDefaultImage();

	std::optional<UnifiedColorImage> m_defaultImage;
};

// In-header Implementations:

template<typename Owner>
inline TSdlUnifiedColorImage<Owner>::TSdlUnifiedColorImage(
	std::string valueName,
	std::shared_ptr<UnifiedColorImage> Owner::* const imagePtr) :

	TSdlReference<UnifiedColorImage, Owner>(
		std::move(valueName), 
		imagePtr),

	m_defaultImage()
{}

template<typename Owner>
inline void TSdlUnifiedColorImage<Owner>::loadFromSdl(
	Owner&                 owner,
	const SdlInputClause&  clause,
	const SdlInputContext& ctx) const
{
	auto colorImage = std::make_shared<UnifiedColorImage>();

	try
	{
		if(clause.isReference)
		{
			colorImage->setImage(Base::template loadResource<Image>(clause, ctx));
		}
		else if(clause.isResourceIdentifier())
		{
			colorImage->setImage(sdl::load_picture_file(
				SdlResourceLocator(ctx).toPath(clause.value)));
		}
		// TODO: detect if clause is external file and load it
		else
		{
			// TODO: load spectral image

			// For constant color input, default to linear-sRGB if not specified
			const auto colorSpace = !clause.tag.empty() ?
				TSdlEnum<math::EColorSpace>()[clause.tag] : math::EColorSpace::Linear_sRGB;

			colorImage->setConstantColor(
				sdl::load_vector3(clause.value),
				colorSpace);
		}
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError(
			"on parsing unified color image -> " + e.whatStr());
	}

	this->setValueRef(owner, std::move(colorImage));
}

template<typename Owner>
inline void TSdlUnifiedColorImage<Owner>::setValueToDefault(Owner& owner) const
{
	// Default image is copied so that modification done by the owner will not affect
	// other owners that also use the same default.
	if(m_defaultImage)
	{
		this->setValueRef(owner, std::make_shared<UnifiedColorImage>(*m_defaultImage));
	}
	else
	{
		this->setValueRef(owner, nullptr);
	}
}

template<typename Owner>
inline auto TSdlUnifiedColorImage<Owner>::noDefault()
-> TSdlUnifiedColorImage&
{
	m_defaultImage = std::nullopt;
	return *this;
}

template<typename Owner>
inline auto TSdlUnifiedColorImage<Owner>::defaultLinearSRGB(const real linearSRGB)
-> TSdlUnifiedColorImage&
{
	return defaultLinearSRGB(math::Vector3R(linearSRGB));
}

template<typename Owner>
inline auto TSdlUnifiedColorImage<Owner>::defaultLinearSRGB(const math::Vector3R& linearSRGB)
-> TSdlUnifiedColorImage&
{
	getDefaultImage()->setConstantColor(linearSRGB, math::EColorSpace::Linear_sRGB);
	return *this;
}

template<typename Owner>
inline auto TSdlUnifiedColorImage<Owner>::defaultImage(std::shared_ptr<Image> image)
-> TSdlUnifiedColorImage&
{
	getDefaultImage()->setImage(std::move(image));
	return *this;
}

template<typename Owner>
inline UnifiedColorImage* TSdlUnifiedColorImage<Owner>::getDefaultImage()
{
	if(!m_defaultImage)
	{
		m_defaultImage = UnifiedColorImage();
	}

	return &(*m_defaultImage);
}

}// end namespace ph
