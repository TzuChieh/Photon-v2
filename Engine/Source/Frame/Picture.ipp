#include "Frame/Picture.h"
#include "Frame/TFrame.h"
#include "Common/assertion.h"

#include <algorithm>

namespace ph
{

template<typename PixelData>
inline Picture::Picture(
	const math::Vector2S sizePx,
	const std::size_t numPicComponents,
	const EPicturePixelComponent componentType,
	const PixelData* const pixelData,
	const std::size_t pixelDataSize)

	: m_sizePx(sizePx)
	, m_numComponents(numPicComponents)
	, m_componentType(componentType)
	, m_data()
{
	const std::size_t numBytes = sizePx.product() * numPicComponents * numBytesInComponent(componentType);
	
	// Note that the following assertion my not be true: 
	//
	// `PH_ASSERT_EQ(numBytes, sizePx.product() * inNumComponents * sizeof(PixelData));`
	//
	// Consider a typical case where RGBA is packed into an `int32`. `PixelData` would be `int32`
	// which contains 4 components already. It is up to the user to provide suitably-sized `pixelData`.
	
	// Implies that `PixelData` should not be padded
	PH_ASSERT_EQ(numBytes, pixelDataSize * sizeof(PixelData));

	m_data = std::make_unique<std::byte[]>(numBytes);

	std::copy(
		reinterpret_cast<const std::byte*>(pixelData),
		reinterpret_cast<const std::byte*>(pixelData + pixelDataSize),
		m_data.get());
}

inline const math::Vector2S& Picture::getSizePx() const
{
	return m_sizePx;
}

inline std::size_t Picture::getWidthPx() const
{
	return getSizePx().x();
}

inline std::size_t Picture::getHeightPx() const
{
	return getSizePx().y();
}

inline std::size_t Picture::numComponents() const
{
	return m_numComponents;
}

inline std::byte* Picture::getData()
{
	PH_ASSERT(m_data);
	return m_data.get();
}

inline const std::byte* Picture::getData() const
{
	PH_ASSERT(m_data);
	return m_data.get();
}

template<typename FrameComponent, std::size_t N>
inline TFrame<FrameComponent, N> Picture::toFrame() const
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

template<typename PictureComponent, typename FrameComponent, std::size_t N>
inline TFrame<FrameComponent, N> Picture::pictureToFrame() const
{
	PH_ASSERT(m_data);

	using FrameType = TFrame<FrameComponent, N>;
	using FramePixelType = typename FrameType::Pixel;

	// Pixel component casting is based on the smaller number of components of the two--other
	// components are either discarded or defaulted to 0.
	const std::size_t minComponents = std::min(m_numComponents, N);

	FrameType frame(m_sizePx.x(), m_sizePx.y());
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
inline std::size_t Picture::getByteIndex(
	const std::size_t xPx,
	const std::size_t yPx,
	const std::size_t componentIndex) const
{
	PH_ASSERT_LT(xPx, m_sizePx.x());
	PH_ASSERT_LT(yPx, m_sizePx.y());
	PH_ASSERT_LT(componentIndex, m_numComponents);

	return (yPx * m_sizePx.x() * m_numComponents + xPx + componentIndex) * sizeof(PictureComponent);
}

inline std::size_t Picture::numBytesInComponent(const EPicturePixelComponent componentType)
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
