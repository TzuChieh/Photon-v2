#include "Actor/Image/RasterFileImage.h"
#include "Core/Texture/Pixel/TFrameBuffer2D.h"
#include "DataIO/io_utils.h"
#include "DataIO/io_exceptions.h"
#include "Actor/actor_exceptions.h"
#include "Frame/TFrame.h"

#include <utility>
#include <cstddef>

namespace ph
{

namespace
{

template<typename T, std::size_t N>
TFrame<T, N> make_frame_from_picture(const RegularPicture& picture)
{
	TFrame<T, N> frame(picture.frame.widthPx(), picture.frame.heightPx());
}

}// end anonymous namespace

RasterFileImage::RasterFileImage() :
	RasterFileImage(Path())
{}

RasterFileImage::RasterFileImage(Path filePath) :
	
	RasterImageBase(),

	m_filePath(std::move(filePath))
{}

//std::shared_ptr<TTexture<Image::NumericArray>> RasterFileImage::genNumericTexture(
//	ActorCookingContext& ctx) const
//{
//
//}
//
//std::shared_ptr<TTexture<math::Spectrum>> RasterFileImage::genColorTexture(
//	ActorCookingContext& ctx) const
//{
//
//}

std::shared_ptr<PixelBuffer2D> RasterFileImage::loadPixelBuffer(ActorCookingContext& ctx)
{
	RegularPicture picture;
	try
	{
		picture = io_utils::load_picture(m_filePath);
	}
	catch(const IOException& e)
	{
		// TODO: better log warning and use a default picture
		throw ActorCookException(
			"error on loading picture <" + m_filePath.toAbsoluteString() + ">");
	}


}

}// end namespace ph
