#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "Frame/frame_fwd.h"
#include "Math/Function/TMathFunction2D.h"
#include "Math/TArithmeticArray.h"
#include "Core/Texture/texture_fwd.h"
#include "Math/Geometry/TAABB2D.h"

#include <vector>
#include <cstddef>

namespace ph
{

template<typename T, std::size_t N>
class TFrame final
{
public:
	template<typename U>
	using TPixel = TArithmeticArray<U, N>;

	using Pixel = TPixel<T>;

	template<typename U>
	using TAABB2D = math::TAABB2D<U>;

	template<typename U = T>
	static TPixel<U> getMonochromaticPixel(U value);

public:
	TFrame();
	TFrame(uint32 wPx, uint32 hPx);
	TFrame(const TFrame& other);
	TFrame(TFrame&& other);
	~TFrame() = default;

	void fill(T value);
	void fill(T value, const TAABB2D<uint32>& region);
	void flipHorizontally();
	void flipVertically();
	void setSize(uint32 wPx, uint32 hPx);
	void setSize(const TVector2<uint32>& sizePx);

	// TODO: user specified clamping range?
	// TODO: specify size instead of radius for finer control
	void sample(
		TFrame& sampled, 
		const TMathFunction2D<float64>& kernel, uint32 kernelRadiusPx) const;

	template<typename PerPixelOperation>
	void forEachPixel(PerPixelOperation op);

	template<typename PerPixelOperation>
	void forEachPixel(PerPixelOperation op) const;

	template<typename PerPixelOperation>
	void forEachPixel(const TAABB2D<uint32>& region, PerPixelOperation op);

	template<typename PerPixelOperation>
	void forEachPixel(const TAABB2D<uint32>& region, PerPixelOperation op) const;

	// TODO: sampling texture

	void setPixel(const TVector2<uint32>& coordPx, const Pixel& pixel);
	void setPixel(uint32 x, uint32 y, const Pixel& pixel);

	Pixel getPixel(const TVector2<uint32>& coordPx) const;
	void getPixel(uint32 x, uint32 y, Pixel* out_pixel) const;
	const T* getPixelData() const;
	TVector2<uint32> getSizePx() const;

	TFrame& operator = (const TFrame& rhs);
	TFrame& operator = (TFrame&& rhs);

	uint32 widthPx() const;
	uint32 heightPx() const;
	bool isEmpty() const;

	constexpr std::size_t numPixelComponents() const noexcept;

	// HACK
	inline std::vector<real> getRealData() const
	{
		std::vector<real> realData;
		for(std::size_t i = 0; i < m_pixelData.size(); i++)
		{
			realData.push_back(m_pixelData[i] / 255.0_r);
		}

		return realData;
	}

private:
	uint32 m_widthPx;
	uint32 m_heightPx;

	std::vector<T> m_pixelData;

	std::size_t calcPixelDataBaseIndex(uint32 x, uint32 y) const;
};

}// end namespace ph

#include "Frame/TFrame.ipp"