#pragma once

#include "DataIO/SDL/Introspect/TSdlReference.h"
#include "Actor/Image/UnifiedColorImage.h"
#include "Common/assertion.h"
#include "Actor/SDLExtension/image_loaders.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/SdlResourceIdentifier.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "Math/TVector3.h"
#include "Actor/SDLExtension/sdl_color_space_type.h"

#include <string>
#include <exception>
#include <vector>
#include <memory>
#include <utility>

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
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override;

	// TODO: save

private:
	UnifiedColorImage* getDefaultImageOrCreate();

	std::shared_ptr<UnifiedColorImage> m_defaultImage;
};

// In-header Implementations:

template<typename Owner>
inline TSdlUnifiedColorImage<Owner>::TSdlUnifiedColorImage(
	std::string valueName,
	std::shared_ptr<UnifiedColorImage> Owner::* const imagePtr) :

	TSdlReference<UnifiedColorImage, Owner>(
		std::move(valueName), 
		imagePtr),

	m_defaultImage(nullptr)
{}

template<typename Owner>
inline void TSdlUnifiedColorImage<Owner>::loadFromSdl(
	Owner&                 owner,
	const SdlInputPayload& payload,
	const SdlInputContext& ctx) const
{
	auto colorImage = std::make_shared<UnifiedColorImage>();

	try
	{
		if(payload.isReference())
		{
			colorImage->setImage(Base::loadResource<Image>(payload, ctx));
		}
		else if(payload.isResourceIdentifier())
		{
			const SdlResourceIdentifier resId(payload.value, ctx.getWorkingDirectory());
			colorImage->setImage(sdl::load_picture_file(resId.getPathToResource()));
		}
		else
		{
			// TODO: load spectral image
			colorImage->setConstantColor(
				sdl::load_vector3(payload.value), 
				TSdlEnum<math::EColorSpace>()[payload.tag]);
		}
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError(
			"on parsing unified color -> " + e.whatStr());
	}

	this->setValueRef(owner, std::move(colorImage));
}

template<typename Owner>
inline void TSdlUnifiedColorImage<Owner>::setValueToDefault(Owner& owner) const
{
	this->setValueRef(owner, m_defaultImage);
}

template<typename Owner>
inline auto TSdlUnifiedColorImage<Owner>::noDefault()
-> TSdlUnifiedColorImage&
{
	m_defaultImage = nullptr;
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
	getDefaultImageOrCreate()->setConstantColor(linearSRGB, math::EColorSpace::Linear_sRGB);
	return *this;
}

template<typename Owner>
inline auto TSdlUnifiedColorImage<Owner>::defaultImage(std::shared_ptr<Image> image)
-> TSdlUnifiedColorImage&
{
	getDefaultImageOrCreate()->setImage(std::move(image));
}

template<typename Owner>
inline UnifiedColorImage* TSdlUnifiedColorImage<Owner>::getDefaultImageOrCreate()
{
	if(!m_defaultImage)
	{
		m_defaultImage = std::make_shared<UnifiedColorImage>();
	}

	return m_defaultImage.get();
}

}// end namespace ph
