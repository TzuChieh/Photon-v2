#pragma once

#include "DataIO/SDL/Introspect/TSdlReference.h"
#include "Actor/Image/UnifiedNumericImage.h"
#include "Common/assertion.h"
#include "Actor/SDLExtension/image_loaders.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/SdlResourceIdentifier.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "Math/TVector3.h"
#include "Actor/SDLExtension/sdl_color_usage_type.h"

#include <string>
#include <exception>
#include <vector>
#include <memory>
#include <utility>
#include <cstddef>
#include <array>
#include <optional>

namespace ph
{

class Image;

/*! @brief Interprets general color representations as image resources.
*/
template<typename Owner>
class TSdlUnifiedNumericImage : public TSdlReference<UnifiedNumericImage, Owner>
{
private:
	using Base = TSdlReference<UnifiedNumericImage, Owner>;

public:
	TSdlUnifiedNumericImage(
		std::string valueName, 
		std::shared_ptr<UnifiedNumericImage> Owner::* imagePtr);

	void setValueToDefault(Owner& owner) const override;

	/*! @brief No default data.
	*/
	TSdlUnifiedNumericImage& noDefault();

	/*! @brief Default to a constant value.
	*/
	template<std::size_t N>
	TSdlUnifiedNumericImage& defaultToConstant(const std::array<float64, N>& constant);

	/*! @brief Set a specific image resource for default value.
	*/
	TSdlUnifiedNumericImage& defaultImage(std::shared_ptr<Image> image);

protected:
	void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override;

	// TODO: save

private:
	UnifiedNumericImage* getDefaultImage();

	std::optional<UnifiedNumericImage> m_defaultImage;
};

// In-header Implementations:

template<typename Owner>
inline TSdlUnifiedNumericImage<Owner>::TSdlUnifiedNumericImage(
	std::string valueName,
	std::shared_ptr<UnifiedNumericImage> Owner::* const imagePtr) :

	TSdlReference<UnifiedNumericImage, Owner>(
		std::move(valueName), 
		imagePtr),

	m_defaultImage()
{}

template<typename Owner>
inline void TSdlUnifiedNumericImage<Owner>::loadFromSdl(
	Owner&                 owner,
	const SdlInputPayload& payload,
	const SdlInputContext& ctx) const
{
	auto numericImage = std::make_shared<UnifiedNumericImage>();

	try
	{
		// TODO: should register newly generated images to scene, so they can be saved later

		if(payload.isReference())
		{
			numericImage->setImage(Base::template loadResource<Image>(payload, ctx));
		}
		// TODO: subscripts
		else if(payload.isResourceIdentifier())
		{
			const SdlResourceIdentifier resId(payload.value, ctx.getWorkingDirectory());
			numericImage->setImage(sdl::load_picture_file(resId.getPathToResource()));
		}
		else
		{
			const auto numberArray = sdl::load_number_array<float64>(payload.value);
			numericImage->setConstant(numberArray.data(), numberArray.size());
		}
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError(
			"on parsing unified numeric image -> " + e.whatStr());
	}

	this->setValueRef(owner, std::move(numericImage));
}

template<typename Owner>
inline void TSdlUnifiedNumericImage<Owner>::setValueToDefault(Owner& owner) const
{
	// Default image is copied so that modification done by the owner will not affect
	// other owners that also use the same default.
	this->setValueRef(owner, std::make_shared<UnifiedNumericImage>(*m_defaultImage));
}

template<typename Owner>
inline auto TSdlUnifiedNumericImage<Owner>::noDefault()
-> TSdlUnifiedNumericImage&
{
	m_defaultImage = std::nullopt;
	return *this;
}

template<typename Owner>
inline auto TSdlUnifiedNumericImage<Owner>::defaultImage(std::shared_ptr<Image> image)
-> TSdlUnifiedNumericImage&
{
	getDefaultImage()->setImage(std::move(image));
	return *this;
}

template<typename Owner>
template<std::size_t N>
inline auto TSdlUnifiedNumericImage<Owner>::defaultToConstant(const std::array<float64, N>& constant)
-> TSdlUnifiedNumericImage&
{
	getDefaultImage()->setConstant<N>(constant);
	return *this;
}

template<typename Owner>
inline UnifiedNumericImage* TSdlUnifiedNumericImage<Owner>::getDefaultImage()
{
	if(!m_defaultImage)
	{
		m_defaultImage = UnifiedNumericImage();
	}

	return &(*m_defaultImage);
}

}// end namespace ph
