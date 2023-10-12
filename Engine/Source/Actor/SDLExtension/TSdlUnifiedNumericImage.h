#pragma once

#include "SDL/Introspect/TSdlReference.h"
#include "Actor/Image/UnifiedNumericImage.h"
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

	void ownedValueToDefault(Owner& owner) const override;

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
		Owner& owner,
		const SdlInputClause& clause,
		const SdlInputContext& ctx) const override;

	void saveToSdl(
		const Owner& owner,
		SdlOutputClause& out_clause,
		const SdlOutputContext& ctx) const override;

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
	Owner& owner,
	const SdlInputClause& clause,
	const SdlInputContext& ctx) const
{
	auto numericImage = std::make_shared<UnifiedNumericImage>();

	try
	{
		// TODO: should register newly generated images to scene, so they can be saved later

		if(clause.isReference)
		{
			numericImage->setImage(Base::template loadResource<Image>(clause, ctx));
		}
		// TODO: subscripts
		else if(clause.isResourceIdentifier())
		{
			numericImage->setImage(sdl::load_picture_file(
				SdlResourceLocator(ctx).toPath(clause.value)));
		}
		// TODO: detect if clause is external file and load it
		else
		{
			numericImage->setConstant(sdl::load_number_array<float64>(clause.value));
		}
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError(
			"on parsing unified numeric image -> " + e.whatStr());
	}

	this->setValueRef(owner, std::move(numericImage));
}

template<typename Owner>
inline void TSdlUnifiedNumericImage<Owner>::saveToSdl(
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
						"failed saving numeric image SRI -> no valid information provided");
				}

				out_clause.value = SdlResourceLocator(ctx).toExternalIdentifier(path).getIdentifier();
			}
		}
		else
		{
			// Save the constant (as value) and swizzle subscripts (as tag)
			sdl::save_number_array(imageRes->getConstant(), &out_clause.value);
			out_clause.tag = imageRes->getSwizzleSubscripts();
		}
	}
	else
	{
		Base::saveToSdl(owner, out_clause, ctx);
	}
}

template<typename Owner>
inline void TSdlUnifiedNumericImage<Owner>::ownedValueToDefault(Owner& owner) const
{
	// Default image is copied so that modification done by the owner will not affect
	// other owners that also use the same default.
	if(m_defaultImage)
	{
		this->setValueRef(owner, std::make_shared<UnifiedNumericImage>(*m_defaultImage));
	}
	else
	{
		this->setValueRef(owner, nullptr);
	}
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
