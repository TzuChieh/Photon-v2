#include "Frame/PictureData.h"
#include "Frame/TFrame.h"
#include "Common/assertion.h"
#include "Utility/utility.h"

#include <algorithm>

namespace ph
{

inline PictureData::PictureData()
	: m_sizePx(0)
	, m_numComponents(0)
	, m_componentType(EPicturePixelComponent::Unspecified)
	, m_data()
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
	m_numBytesInData = sizePx.product() * numPicComponents * numBytesInComponent(componentType);
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

inline PictureData::PictureData(PictureData&& other) = default;

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

inline std::byte* PictureData::getData()
{
	return m_data.get();
}

inline const std::byte* PictureData::getData() const
{
	return m_data.get();
}

inline std::size_t PictureData::numBytesInData() const
{
	return m_numBytesInData;
}

template<typename PixelData>
inline void PictureData::setPixels(
	const PixelData* const pixelData,
	const std::size_t pixelDataSize)
{
	// Note that the following assertion my not be true: 
	//
	// `PH_ASSERT_EQ(m_numBytesInData, sizePx.product() * inNumComponents * sizeof(PixelData));`
	//
	// Consider a typical case where RGBA is packed into an `int32`. `PixelData` would be `int32`
	// which contains 4 components already. It is up to the user to provide suitably-sized `pixelData`.
	
	// Implies that `PixelData` should not be padded
	PH_ASSERT_EQ(m_numBytesInData, pixelDataSize * sizeof(PixelData));

	// Should have been allocated; set pixels on empty picture is not allowed.
	PH_ASSERT(m_data);

	std::copy(
		reinterpret_cast<const std::byte*>(pixelData),
		reinterpret_cast<const std::byte*>(pixelData + pixelDataSize),
		m_data.get());
}

template<typename FrameComponent, std::size_t N>
inline TFrame<FrameComponent, N> PictureData::toFrame() const
{
	switch(m_componentType)
	{
	case EPicturePixelComponent::PPC_Int8:
		return pictureToFrame<int8, FrameComponent, N>();

	case EPicturePixelComponent::PPC_UInt8:
		return pictureToFrame<uint8, FrameComponent, N>();

	case EPicturePixelComponent::PPC_Int16:
		return pictureToFrame<int16, FrameComponent, N>();

	case EPicturePixelComponent::PPC_UInt16:
		return pictureToFrame<uint16, FrameComponent, N>();

	case EPicturePixelComponent::PPC_Int32:
		return pictureToFrame<int32, FrameComponent, N>();

	case EPicturePixelComponent::PPC_UInt32:
		return pictureToFrame<uint32, FrameComponent, N>();

	case EPicturePixelComponent::PPC_Int64:
		return pictureToFrame<int64, FrameComponent, N>();

	case EPicturePixelComponent::PPC_UInt64:
		return pictureToFrame<uint64, FrameComponent, N>();

	case EPicturePixelComponent::PPC_Float32:
		return pictureToFrame<float32, FrameComponent, N>();

	case EPicturePixelComponent::PPC_Float64:
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

inline PictureData& PictureData::operator = (PictureData&& rhs) = default;

template<typename PictureComponent, typename FrameComponent, std::size_t N>
inline TFrame<FrameComponent, N> PictureData::pictureToFrame() const
{
	PH_ASSERT(m_data);

	using FrameType = TFrame<FrameComponent, N>;
	using FramePixelType = typename FrameType::Pixel;

	// Pixel component casting is based on the smaller number of components of the two--other
	// components are either discarded or defaulted to 0.
	const std::size_t minComponents = std::min(m_numComponents, N);

	FrameType frame(
		safe_number_cast<uint32>(m_sizePx.x()), 
		safe_number_cast<uint32>(m_sizePx.y()));
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

	return (yPx * m_sizePx.x() * m_numComponents + xPx + componentIndex) * sizeof(PictureComponent);
}

inline std::size_t PictureData::numBytesInComponent(const EPicturePixelComponent componentType)
{
	switch(componentType)
	{
	case EPicturePixelComponent::Unspecified: return 0;
	case EPicturePixelComponent::PPC_Int8: return 1;
	case EPicturePixelComponent::PPC_UInt8: return 1;
	case EPicturePixelComponent::PPC_Int16: return 2;
	case EPicturePixelComponent::PPC_UInt16: return 2;
	case EPicturePixelComponent::PPC_Int32: return 4;
	case EPicturePixelComponent::PPC_UInt32: return 4;
	case EPicturePixelComponent::PPC_Int64: return 8;
	case EPicturePixelComponent::PPC_UInt64: return 8;
	case EPicturePixelComponent::PPC_Float16: return 2;
	case EPicturePixelComponent::PPC_Float32: return 4;
	case EPicturePixelComponent::PPC_Float64: return 8;
	default: PH_ASSERT_UNREACHABLE_SECTION(); return 0;
	}
}

}// end namespace ph
