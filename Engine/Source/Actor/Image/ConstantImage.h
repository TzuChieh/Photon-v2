#pragma once

#include "Actor/Image/Image.h"
#include "Math/math_fwd.h"
#include "Actor/SDLExtension/sdl_color_space_type.h"
#include "DataIO/SDL/sdl_interface.h"

#include <vector>
#include <utility>
#include <type_traits>

namespace ph
{

class ConstantImage : public Image
{
public:
	ConstantImage();

	template<typename T>
	explicit ConstantImage(T value);

	template<typename T>
	explicit ConstantImage(const math::TVector3<T>& values);

	template<typename T>
	explicit ConstantImage(std::vector<T> values);

	template<typename T>
	ConstantImage(T color, math::EColorSpace colorSpace);

	template<typename T>
	ConstantImage(const math::TVector3<T>& values, math::EColorSpace colorSpace);

	template<typename T>
	ConstantImage(std::vector<T> values, math::EColorSpace colorSpace);

	std::shared_ptr<TTexture<Image::Array>> genNumericTexture(
		CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		CookingContext& ctx) override;

private:
	std::vector<float64> m_values;
	math::EColorSpace    m_colorSpace;

public:
	PH_DEFINE_SDL_CLASS(TOwnerSdlClass<ConstantImage>)
	{
		ClassType clazz("constant");
		clazz.docName("Constant Image");
		clazz.description("An image that stores constant value. It can be a single scalar, a vector or a color.");
		clazz.baseOn<Image>();

		TSdlRealArray<OwnerType, float64> values("value", &OwnerType::m_values);
		values.description("A series of values to populate the const image.");
		values.defaultTo({0});
		values.required();
		clazz.addField(values);

		TSdlEnumField<OwnerType, math::EColorSpace> colorSpace(&OwnerType::m_colorSpace);
		colorSpace.description("Associated color space of the constant.");
		colorSpace.defaultTo(math::EColorSpace::Unspecified);
		colorSpace.optional();
		clazz.addField(colorSpace);

		return clazz;
	}
};

// In-ueader Implementations:

template<typename T>
inline ConstantImage::ConstantImage(const T value) :
	ConstantImage(value, math::EColorSpace::Unspecified)
{}

template<typename T>
inline ConstantImage::ConstantImage(const math::TVector3<T>& values) :
	ConstantImage(values, math::EColorSpace::Unspecified)
{}

template<typename T>
inline ConstantImage::ConstantImage(std::vector<T> values) :
	ConstantImage(std::move(values), math::EColorSpace::Unspecified)
{}

template<typename T>
inline ConstantImage::ConstantImage(const T color, const math::EColorSpace colorSpace) :
	ConstantImage(std::vector<T>{color}, colorSpace)
{}

template<typename T>
inline ConstantImage::ConstantImage(const math::TVector3<T>& values, const math::EColorSpace colorSpace) :
	ConstantImage(std::vector<T>{values.x(), values.y(), values.z()}, colorSpace)
{}

template<typename T>
inline ConstantImage::ConstantImage(std::vector<T> values, const math::EColorSpace colorSpace) :
	m_values(), m_colorSpace(colorSpace)
{
	if constexpr(std::is_same_v<T, float64>)
	{
		m_values = std::move(values);
	}
	else
	{
		m_values.resize(values.size());
		for(std::size_t i = 0; i < values.size(); ++i)
		{
			m_values[i] = static_cast<float64>(values[i]);
		}
	}
}

}// end namespace ph
