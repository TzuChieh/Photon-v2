#pragma once

#include "DataIO/SDL/Introspect/TSdlReference.h"
#include "Common/assertion.h"
#include "Actor/SDLExtension/color_loaders.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/SdlResourceIdentifier.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "Math/TVector3.h"

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
	TSdlGenericColor(std::string valueName, std::shared_ptr<Image> Owner::* imagePtr);

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
		const std::string&     sdlValue,
		const SdlInputContext& ctx) const override;

private:
	std::optional<math::Vector3R> m_defaultValue;
};

// In-header Implementations:

template<typename Owner>
inline TSdlGenericColor<Owner>::TSdlGenericColor(
	std::string valueName, 
	std::shared_ptr<Image> Owner::* const imagePtr) :

	TSdlReference<Image, Owner>(
		std::move(valueName), 
		imagePtr),

	m_defaultValue()
{}

template<typename Owner>
inline void TSdlGenericColor<Owner>::loadFromSdl(
	Owner&                 owner,
	const std::string&     sdlValue,
	const SdlInputContext& ctx) const
{
	try
	{
		if(sdl::is_reference(sdlValue))
		{
			Base::loadFromSdl(owner, sdlValue, ctx);
		}
		else if(sdl::is_resource_identifier(sdlValue))
		{
			const SdlResourceIdentifier resId(sdlValue, ctx.workingDirectory);
			setValueRef(owner, sdl::load_picture_color(resId.getPathToResource()));
		}
		else
		{
			const auto values = sdl::load_vector3(sdlValue);
			setValueRef(owner, sdl::load_constant_color(values));
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
	if(m_defaultValue)
	{
		setValueRef(owner, sdl::load_constant_color(m_defaultValue.value()));
	}
	else
	{
		setValueRef(owner, nullptr);
	}
}

template<typename Owner>
inline auto TSdlGenericColor<Owner>::defaultToEmpty()
-> TSdlGenericColor&
{
	m_defaultValue = std::nullopt;
	return this;
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
	m_defaultValue = linearSrgb;
	return *this;
}

}// end namespace ph
