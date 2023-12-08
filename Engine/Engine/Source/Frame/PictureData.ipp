#include "Frame/PictureData.h"
#include "Frame/TFrame.h"
#include "Utility/utility.h"

#include <Common/assertion.h>

#include <algorithm>
#include <type_traits>
#include <utility>

namespace ph
{

inline PictureData::PictureData()
	: m_sizePx(0)
	, m_numComponents(0)
	, m_componentType(EPicturePixelComponent::Empty)
	, m_data(nullptr)
	, m_numBytesInData(0)
{}

inline PictureData::PictureData(
	const math::Vector2S sizePx,
	const std::size_t numPicComponents,
	const EPicturePixelComponent componentType)

	: m_sizePx(sizePx)
	, m_numComponents(numPicComponents)
	, m_componentType(componentType)
	, m_data()
	, m_numBytesInData(0)
{
	m_numBytesInData = sizePx.product() * numPicComponents * num_bytes_in_component(componentType);
	m_data = std::make_unique<std::byte[]>(m_numBytesInData);
}

template<typename PixelData>
inline PictureData::PictureData(
	const math::Vector2S sizePx,
	const std::size_t numPicComponents,
	const EPicturePixelComponent componentType,
	const PixelData* const pixelData,
	const std::size_t pixelDataSize)

	: PictureData(
		sizePx,
		numPicComponents,
		componentType)
{
	setPixels(pixelData, pixelDataSize);
}

inline PictureData::PictureData(PictureData&& other)
	: PictureData()
{
	*this = std::move(other);
}

inline const math::Vector2S& PictureData::getSizePx() const
{
	return m_sizePx;
}

inline std::size_t PictureData::getWidthPx() const
{
	return getSizePx().x();
}

inline std::size_t PictureData::getHeightPx() const
{
	return getSizePx().y();
}

inline std::size_t PictureData::numComponents() const
{
	return m_numComponents;
}

inline EPicturePixelComponent PictureData::getComponentType() const
{
	return m_componentType;
}

inline TSpan<std::byte> PictureData::getBytes()
{
	return {m_data.get(), m_numBytesInData};
}

inline TSpanView<std::byte> PictureData::getBytes() const
{
	return {m_data.get(), m_numBytesInData};
}

template<typename PixelData>
inline void PictureData::setPixels(
	const PixelData* const pixelData,
	const std::size_t numPixelDataElements)
{
	// Note that the following assertion my not be true: 
	//
	// `PH_ASSERT_EQ(m_numBytesInData, sizePx.product() * inNumComponents * sizeof(PixelData));`
	//
	// Consider a typical case where RGBA is packed into an `int32`. `PixelData` would be `int32`
	// which contains 4 components already. It is up to the user to provide suitably-sized `pixelData`.
	
	// Implies that `PixelData` should not be padded
	PH_ASSERT_EQ(m_numBytesInData, numPixelDataElements * sizeof(PixelData));

	// Should have been allocated; set pixels on empty picture is not allowed.
	PH_ASSERT(m_data);

	// Generally we would want `PixelData` to be trivially copyable since we are basically handling
	// raw bytes here
	static_assert(std::is_trivially_copyable_v<PixelData>);

	std::copy(
		reinterpret_cast<const std::byte*>(pixelData),
		reinterpret_cast<const std::byte*>(pixelData + numPixelDataElements),
		m_data.get());
}

template<typename FrameComponent, std::size_t N>
inline TFrame<FrameComponent, N> PictureData::toFrame() const
{
	switch(m_componentType)
	{
	case EPicturePixelComponent::Int8:
		return pictureToFrame<int8, FrameComponent, N>();

	case EPicturePixelComponent::UInt8:
		return pictureToFrame<uint8, FrameComponent, N>();

	case EPicturePixelComponent::Int16:
		return pictureToFrame<int16, FrameComponent, N>();

	case EPicturePixelComponent::UInt16:
		return pictureToFrame<uint16, FrameComponent, N>();

	case EPicturePixelComponent::Int32:
		return pictureToFrame<int32, FrameComponent, N>();

	case EPicturePixelComponent::UInt32:
		return pictureToFrame<uint32, FrameComponent, N>();

	case EPicturePixelComponent::Int64:
		return pictureToFrame<int64, FrameComponent, N>();

	case EPicturePixelComponent::UInt64:
		return pictureToFrame<uint64, FrameComponent, N>();

	case EPicturePixelComponent::Float32:
		return pictureToFrame<float32, FrameComponent, N>();

	case EPicturePixelComponent::Float64:
		return pictureToFrame<float64, FrameComponent, N>();

	default: 
		PH_ASSERT_UNREACHABLE_SECTION();
		break;
	}

	return TFrame<FrameComponent, N>();
}

inline bool PictureData::isEmpty() const
{
	return m_data == nullptr;
}

inline PictureData& PictureData::operator = (PictureData&& rhs)
{
	m_sizePx = rhs.m_sizePx;
	m_numComponents = rhs.m_numComponents;
	m_componentType = rhs.m_componentType;
	m_data = std::move(rhs.m_data);
	m_numBytesInData = rhs.m_numBytesInData;

	return *this;
}

template<typename PictureComponent, typename FrameComponent, std::size_t N>
inline TFrame<FrameComponent, N> PictureData::pictureToFrame() const
{
	// Generally we would want `PictureComponent` and `FrameComponent` to be trivially copyable since 
	// we are basically handling raw bytes here
	static_assert(std::is_trivially_copyable_v<PictureComponent>);
	static_assert(std::is_trivially_copyable_v<FrameComponent>);

	PH_ASSERT(m_data);

	using FrameType = TFrame<FrameComponent, N>;
	using FramePixelType = typename FrameType::PixelType;

	// Pixel component casting is based on the smaller number of components of the two--other
	// components are either discarded or defaulted to 0.
	const std::size_t minComponents = std::min(m_numComponents, N);

	FrameType frame(
		lossless_cast<uint32>(m_sizePx.x()),
		lossless_cast<uint32>(m_sizePx.y()));
	frame.forEachPixel(
		[this, minComponents](const std::size_t x, const std::size_t y)
		{
			FramePixelType pixel;
			for(std::size_t ci = 0; ci < minComponents; ++ci)
			{
				const std::size_t byteIndex = getByteIndex<PictureComponent>(x, y, ci);

				PictureComponent pictureComponent;
				std::copy(
					m_data.get() + byteIndex,
					m_data.get() + byteIndex + sizeof(PictureComponent),
					reinterpret_cast<std::byte*>(&pictureComponent));

				pixel[ci] = static_cast<FrameComponent>(pictureComponent);
			}

			return pixel;
		});

	return frame;
}

template<typename PictureComponent>
inline std::size_t PictureData::getByteIndex(
	const std::size_t xPx,
	const std::size_t yPx,
	const std::size_t componentIndex) const
{
	PH_ASSERT_LT(xPx, m_sizePx.x());
	PH_ASSERT_LT(yPx, m_sizePx.y());
	PH_ASSERT_LT(componentIndex, m_numComponents);

	const auto linearPixelIndex = yPx * m_sizePx.x() + xPx;
	return (linearPixelIndex * m_numComponents + componentIndex) * sizeof(PictureComponent);
}

}// end namespace ph
