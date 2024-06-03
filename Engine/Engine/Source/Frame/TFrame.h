#pragma once

#include "Math/math_fwd.h"
#include "Frame/frame_fwd.h"
#include "Math/Function/TMathFunction2D.h"
#include "Math/TArithmeticArray.h"
#include "Core/Texture/texture_fwd.h"
#include "Math/Geometry/TAABB2D.h"
#include "Utility/TSpan.h"

#include <Common/primitive_type.h>

#include <vector>
#include <cstddef>

namespace ph
{

template<typename T, std::size_t N>
class TFrame final
{
public:
	using ElementType = T;

	template<typename U>
	using TPixelType = math::TArithmeticArray<U, N>;

	using PixelType = TPixelType<T>;

	template<typename U = T>
	static TPixelType<U> makeMonochromaticPixel(U value);

public:
	/*! @brief Constructs an empty frame.
	*/
	TFrame();

	/*! @brief Constructs a frame with specified dimensions. All pixels are initialized to 0.
	*/
	TFrame(uint32 wPx, uint32 hPx);

	TFrame(const TFrame& other);
	TFrame(TFrame&& other) noexcept;
	inline ~TFrame() = default;

	void fill(T value);
	void fill(T value, const math::TAABB2D<uint32>& region);
	void flipHorizontally();
	void flipVertically();
	void setSize(uint32 wPx, uint32 hPx);
	void setSize(const math::TVector2<uint32>& sizePx);

	// TODO: user specified clamping range?
	// TODO: specify size instead of radius for finer control
	void sample(
		TFrame& sampled, 
		const math::TMathFunction2D<float64>& kernel, uint32 kernelRadiusPx) const;

	/*! @brief Iterate over all pixels in the frame in row-major order.
	*/
	///@{
	template<typename PerPixelOperation>
	void forEachPixel(PerPixelOperation op);

	template<typename PerPixelOperation>
	void forEachPixel(PerPixelOperation op) const;
	///@}

	/*! @brief Iterate over all pixels in the frame in row-major order.
	@param region The region to iterate for.
	@param op The operation to perform for each pixel.
	*/
	///@{
	template<typename PerPixelOperation>
	void forEachPixel(const math::TAABB2D<uint32>& region, PerPixelOperation op);

	template<typename PerPixelOperation>
	void forEachPixel(const math::TAABB2D<uint32>& region, PerPixelOperation op) const;
	///@}

	// TODO: sampling texture

	void setPixel(const math::TVector2<uint32>& coordPx, const PixelType& pixel);
	void setPixel(uint32 x, uint32 y, const PixelType& pixel);

	PixelType getPixel(const math::TVector2<uint32>& coordPx) const;
	void getPixel(uint32 x, uint32 y, PixelType* out_pixel) const;

	/*! @brief Get pixel data for the full frame.
	@return Data for all pixels in row-major order. Components of a pixel are placed continuously.
	*/
	///@{
	TSpan<T> getPixelData();
	TSpanView<T> getPixelData() const;
	///@}

	/*! @brief Copy a region of pixel data into a buffer.
	@param region The region to copy from.
	@param[out] out_data Buffer to copy into. Copied pixels are in row-major and without any padding.
	*/
	void copyPixelData(const math::TAABB2D<uint32>& region, TSpan<T> out_data) const;

	math::TVector2<uint32> getSizePx() const;

	TFrame& operator = (const TFrame& rhs);
	TFrame& operator = (TFrame&& rhs) noexcept;

	uint32 widthPx() const;
	uint32 heightPx() const;
	bool isEmpty() const;

	constexpr std::size_t numPixelComponents() const noexcept;

private:
	uint32 m_widthPx;
	uint32 m_heightPx;

	std::vector<T> m_pixelData;

	std::size_t calcPixelDataBaseIndex(uint32 x, uint32 y) const;
};

}// end namespace ph

#include "Frame/TFrame.ipp"
