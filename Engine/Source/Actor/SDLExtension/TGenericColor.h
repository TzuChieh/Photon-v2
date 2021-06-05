#pragma once

#include "DataIO/SDL/Introspect/TSdlReference.h"
#include "Common/assertion.h"
#include "Actor/SDLExtension/color_loaders.h"
#include "DataIO/SDL/sdl_exceptions.h"

#include <string>
#include <exception>
#include <vector>
#include <memory>
#include <utility>

namespace ph
{

class Image;

}// end namespace ph

namespace ph::sdl
{

// TODO: default color?

template<typename Owner>
class TGenericColor : public TSdlReference<ETypeCategory::REF_IMAGE, Image, Owner>
{
private:
	using Base = TSdlReference<ETypeCategory::REF_IMAGE, Image, Owner>;

public:
	TGenericColor(std::string valueName, std::shared_ptr<Image> Owner::* imagePtr);

private:
	void loadFromSdl(
		Owner&                 owner,
		const std::string&     sdlValue,
		const SdlInputContext& ctx) const override;
};

// In-header Implementations:

template<typename Owner>
inline TGenericColor<Owner>::TGenericColor(std::string valueName, std::shared_ptr<Image> Owner::* const imagePtr) :
	TSdlReference<ETypeCategory::REF_IMAGE, Image, Owner>(std::move(valueName), imagePtr)
{}

template<typename Owner>
inline void TGenericColor<Owner>::loadFromSdl(
	Owner&                 owner,
	const std::string&     sdlValue,
	const SdlInputContext& ctx) const
{
	try
	{
		if(is_reference(sdlValue))
		{
			Base::loadFromSdl(owner, sdlValue, ctx);
		}
		// If the value is not a reference, load it as a constant
		else
		{
			setValuePtr(owner, load_constant_color(sdlValue));
		}
	}
	catch (const SdlLoadError& e)
	{
		throw SdlLoadError(
			"on parsing generic color -> " + e.whatStr());
	}
}

}// end namespace ph::sdl
