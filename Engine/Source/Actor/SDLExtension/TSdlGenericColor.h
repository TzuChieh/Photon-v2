#pragma once

#include "DataIO/SDL/Introspect/TSdlReference.h"
#include "Common/assertion.h"
#include "Actor/SDLExtension/color_loaders.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/SdlResourceIdentifier.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "Math/TVector3.h"
#include "Core/Quantity/EQuantity.h"

#include <string>
#include <exception>
#include <vector>
#include <memory>
#include <utility>
#include <optional>

namespace ph
{

class Image;

// TODO: rename to TSdlGenericImage and with TSdlGenericColorImage, TSdlGenericNonColorImage

/*! @brief Interprets general color representations as image resources.
*/
template<typename Owner>
class TSdlGenericColor : public TSdlReference<Image, Owner>
{
private:
	using Base = TSdlReference<Image, Owner>;

public:
	TSdlGenericColor(
		std::string valueName, 
		EQuantity usage, 
		std::shared_ptr<Image> Owner::* imagePtr);

	void setValueToDefault(Owner& owner) const override;

	/*! @brief Default to no image resource.
	*/
	TSdlGenericColor& defaultToEmpty();

	/*! @brief Default to a monochromatic linear SRGB value.
	*/
	TSdlGenericColor& defaultToLinearSrgb(real linearSrgb);

	/*! @brief Default to a linear SRGB value.
	*/
	TSdlGenericColor& defaultToLinearSrgb(const math::Vector3R& linearSrgb);

protected:
	void loadFromSdl(
		Owner&                 owner,
		const SdlInputPayload& payload,
		const SdlInputContext& ctx) const override;

	// TODO: save

private:
	EQuantity m_usage;
	// TODO: support for more tristimulus color spaces or generic default image
	std::optional<math::Vector3R> m_defaultLinearSrgb;
};

// In-header Implementations:

template<typename Owner>
inline TSdlGenericColor<Owner>::TSdlGenericColor(
	std::string valueName,
	const EQuantity usage,
	std::shared_ptr<Image> Owner::* const imagePtr) :

	TSdlReference<Image, Owner>(
		std::move(valueName), 
		imagePtr),

	m_usage(usage),
	m_defaultLinearSrgb()
{}

template<typename Owner>
inline void TSdlGenericColor<Owner>::loadFromSdl(
	Owner&                 owner,
	const SdlInputPayload& payload,
	const SdlInputContext& ctx) const
{
	try
	{
		if(payload.isReference())
		{
			Base::loadFromSdl(owner, payload, ctx);
		}
		else if(payload.isResourceIdentifier())
		{
			const SdlResourceIdentifier resId(payload.value, ctx.getWorkingDirectory());
			this->setValueRef(owner, sdl::load_picture_color(resId.getPathToResource()));
		}
		else
		{
			// TODO: load spectral image
			const auto tristimulus = sdl::load_vector3(std::string(payload.value));
			this->setValueRef(owner, sdl::load_tristimulus_color(tristimulus, ESdlColorSpace::LINEAR_SRGB, m_usage));
		}
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError(
			"on parsing generic color -> " + e.whatStr());
	}
}

template<typename Owner>
inline void TSdlGenericColor<Owner>::setValueToDefault(Owner& owner) const
{
	if(m_defaultLinearSrgb)
	{
		this->setValueRef(owner, sdl::load_tristimulus_color(
			m_defaultLinearSrgb.value(), ESdlColorSpace::LINEAR_SRGB, m_usage));
	}
	else
	{
		this->setValueRef(owner, nullptr);
	}
}

template<typename Owner>
inline auto TSdlGenericColor<Owner>::defaultToEmpty()
-> TSdlGenericColor&
{
	m_defaultLinearSrgb = std::nullopt;
	return *this;
}

template<typename Owner>
inline auto TSdlGenericColor<Owner>::defaultToLinearSrgb(const real linearSrgb)
-> TSdlGenericColor&
{
	return defaultToLinearSrgb(math::Vector3R(linearSrgb));
}

template<typename Owner>
inline auto TSdlGenericColor<Owner>::defaultToLinearSrgb(const math::Vector3R& linearSrgb)
-> TSdlGenericColor&
{
	m_defaultLinearSrgb = linearSrgb;
	return *this;
}

}// end namespace ph
