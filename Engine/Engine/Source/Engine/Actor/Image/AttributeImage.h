#pragma once

#include "Engine/Actor/Image/Image.h"
#include "Engine/Actor/Image/sdl_image_enums.h"
#include "Engine/SDL/sdl_interface.h"

#include <memory>

namespace ph
{

/*! @brief Exposes general attributes as image data.
*/
class AttributeImage : public Image
{
public:
	std::shared_ptr<TTexture<Image::NumericType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	AttributeImage& setKind(EAttributeKind kind);
	EAttributeKind getKind() const;

private:
	EAttributeKind m_kind;

public:
	PH_DEFINE_SDL_CLASS(AttributeImage, clazz)
	{
		clazz.typeName("attribute");
		clazz.docName("Attribute Image");
		clazz.description(
			"Exposes general data as image data.");
		clazz.baseOn<Image>();

		TSdlEnumField<OwnerType, EAttributeKind> kind("kind", &OwnerType::m_kind);
		kind.description("The surface attribute data to expose.");
		kind.defaultTo(EAttributeKind::UvwFromGeometryBound);
		kind.required();
		clazz.addField(kind);
	}
};

inline AttributeImage& AttributeImage::setKind(const EAttributeKind kind)
{
	m_kind = kind;
	return *this;
}

inline EAttributeKind AttributeImage::getKind() const
{
	return m_kind;
}

}// end namespace ph
