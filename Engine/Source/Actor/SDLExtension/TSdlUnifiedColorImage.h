#pragma once

#include "SDL/Introspect/TSdlReference.h"
#include "Actor/Image/UnifiedColorImage.h"
#include "Common/assertion.h"
#include "Actor/SDLExtension/sdl_image_io.h"
#include "SDL/sdl_exceptions.h"
#include "SDL/sdl_helpers.h"
#include "SDL/SdlResourceLocator.h"
#include "SDL/Introspect/SdlInputContext.h"
#include "Math/TVector3.h"
#include "Actor/SDLExtension/sdl_color_enums.h"

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

	void ownedValueToDefault(Owner& owner) const override;

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
		Owner& owner,
		const SdlInputClause& clause,
		const SdlInputContext& ctx) const override;

	void saveToSdl(
		const Owner& owner,
		SdlOutputClause& out_clause,
		const SdlOutputContext& ctx) const override;

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
	Owner& owner,
	const SdlInputClause& clause,
	const SdlInputContext& ctx) const
{
	std::shared_ptr<UnifiedColorImage> imageRes;

	try
	{
		if(clause.isReference)
		{
			std::shared_ptr<Image> referencedRes = Base::template loadResource<Image>(clause, ctx);

			// If we referenced a unified image, then we can treat it as ordinary SDL reference
			imageRes = std::dynamic_pointer_cast<UnifiedColorImage>(referencedRes);

			// Referenced image is of another type or null, create a new unified image to contain it
			if(!imageRes)
			{
				
				imageRes = TSdl<UnifiedColorImage>::makeResource();
				imageRes->setImage(referencedRes);
			}
		}
		else if(clause.isResourceIdentifier())
		{
			SdlResourceLocator resolver(ctx);

			// We support inline SRI only
			ResourceIdentifier sri(clause.value);
			if(sri.resolve(resolver))
			{
				this->setValue(owner, sri);
			}
			else
			{
				// Expected to be a valid SRI. It is an error if it is not a SRI (or cannot be resolved).
				throw_formatted<SdlLoadError>(
					"failed loading SRI -> cannot resolve {}", sri.getIdentifier());
			}

			imageRes = TSdl<UnifiedColorImage>::makeResource();
			imageRes->setFile(sdl::load_picture_file(
				SdlResourceLocator(ctx).toPath(clause.value)));
		}
		else
		{
			// For constant color input, default to linear-sRGB if not specified
			const auto colorSpace = !clause.tag.empty() ?
				TSdlEnum<math::EColorSpace>()[clause.tag] : math::EColorSpace::Linear_sRGB;

			imageRes = TSdl<UnifiedColorImage>::makeResource();
			imageRes->setConstantColor(sdl::load_vector3(clause.value), colorSpace);
		}
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError(
			"on parsing unified color image -> " + e.whatStr());
	}

	this->setValueRef(owner, imageRes);
}

template<typename Owner>
inline void TSdlUnifiedColorImage<Owner>::saveToSdl(
	const Owner& owner,
	SdlOutputClause& out_clause,
	const SdlOutputContext& ctx) const
{
	const auto& imageRes = this->getValueRef(owner);
	if(imageRes->isInlinable())
	{
		// We cannot inline an image reference
		PH_ASSERT(!imageRes->getImage());

		if(!imageRes->getFile().isEmpty())
		{
			// We support inline SRI only
			const ResourceIdentifier& sri = imageRes->getFile();

			// Store the original identifier (the unresolved one)
			if(sri.hasIdentifier())
			{
				out_clause.value = sri.getIdentifier();
			}
			// Try to work out a SRI from path
			else
			{
				const Path& path = sri.getPath();
				if(path.isEmpty())
				{
					throw SdlSaveError(
						"failed saving color image SRI -> no valid information provided");
				}

				out_clause.value = SdlResourceLocator(ctx).toExternalIdentifier(path).getIdentifier();
			}
		}
		else
		{
			// Save the constant (as value) and color space (as tag)
			sdl::save_vector3(imageRes->getConstant(), &out_clause.value);
			out_clause.tag = TSdlEnum<math::EColorSpace>{}[imageRes->getConstantColorSpace()];
		}
	}
	else
	{
		Base::saveToSdl(owner, out_clause, ctx);
	}
}

template<typename Owner>
inline void TSdlUnifiedColorImage<Owner>::ownedValueToDefault(Owner& owner) const
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
