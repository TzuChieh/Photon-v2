#pragma once

#include "Actor/Image/Image.h"
#include "Math/math_fwd.h"
#include "Actor/SDLExtension/sdl_color_enums.h"
#include "SDL/sdl_interface.h"

#include <vector>
#include <utility>
#include <type_traits>

namespace ph
{

class ConstantImage : public Image
{
public:
	std::shared_ptr<TTexture<Image::ArrayType>> genNumericTexture(
		const CookingContext& ctx) override;

	std::shared_ptr<TTexture<math::Spectrum>> genColorTexture(
		const CookingContext& ctx) override;

	template<typename T>
	void setRaw(T value);

	template<typename T>
	void setRaw(const math::TVector3<T>& values);

	template<typename T>
	void setRaw(std::vector<T> values);

	/*! @brief Set as a monochromatic color value.
	*/
	template<typename T>
	void setColor(T color, math::EColorSpace colorSpace);

	/*! @brief Set as a tristimulus color value.
	*/
	template<typename T>
	void setColor(const math::TVector3<T>& color, math::EColorSpace colorSpace);

	template<typename T>
	void setValues(std::vector<T> values, math::EColorSpace colorSpace);

private:
	std::vector<float64> m_values;
	math::EColorSpace m_colorSpace;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<ConstantImage>)
	{
		ClassType clazz("constant");
		clazz.docName("Constant Image");
		clazz.description(
			"An image that stores constant values. It can be a single scalar, a vector or a color.");
		clazz.baseOn<Image>();

		TSdlRealArray<OwnerType, float64> values("values", &OwnerType::m_values);
		values.description("A series of values to initialize the constant.");
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
inline void ConstantImage::setRaw(T value)
{
	setRaw(std::vector<T>{value});
}

template<typename T>
inline void ConstantImage::setRaw(const math::TVector3<T>& values)
{
	setRaw(std::vector<T>{values.x(), values.y(), values.z()});
}

template<typename T>
inline void ConstantImage::setRaw(std::vector<T> values)
{
	setValues(std::move(values), math::EColorSpace::Unspecified);
}

template<typename T>
inline void ConstantImage::setColor(T color, math::EColorSpace colorSpace)
{
	setValues(std::vector<T>{color}, colorSpace);
}

template<typename T>
inline void ConstantImage::setColor(const math::TVector3<T>& color, math::EColorSpace colorSpace)
{
	setValues(std::vector<T>{color.x(), color.y(), color.z()}, colorSpace);
}

template<typename T>
inline void ConstantImage::setValues(std::vector<T> values, math::EColorSpace colorSpace)
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

	m_colorSpace = colorSpace;
}

}// end namespace ph
