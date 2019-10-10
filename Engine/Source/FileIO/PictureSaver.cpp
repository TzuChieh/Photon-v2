#include "FileIO/PictureSaver.h"
#include "Frame/TFrame.h"
#include "Common/assertion.h"
#include "Common/Logger.h"
#include "Frame/frame_utils.h"
#include "FileIO/ExrFileWriter.h"

#include "Common/ThirdParty/lib_stb.h"

#include <string>
#include <vector>
#include <limits>
#include <climits>
#include <type_traits>

namespace ph
{

namespace
{
	const Logger logger(LogSender("Picture Saver"));
}

bool PictureSaver::init()
{
	stbi_flip_vertically_on_write(true);

	return true;
}

bool PictureSaver::save(const LdrRgbFrame& frame, const Path& filePath)
{
	const std::string& ext = filePath.getExtension();

	if(ext == ".png" || ext == ".PNG")
	{
		return savePng(frame, filePath);
	}
	else if(ext == ".jpg" || ext == ".JPG")
	{
		return saveJpg(frame, filePath);
	}
	else if(ext == ".bmp" || ext == ".BMP")
	{
		return saveBmp(frame, filePath);
	}
	else if(ext == ".tga" || ext == ".TGA")
	{
		return saveTga(frame, filePath);
	}
	else if(
		ext == ".exr" || ext == ".EXR" ||
		ext == ".hdr" || ext == ".HDR")
	{
		HdrRgbFrame HdrFrame;
		frame_utils::to_HDR(frame, &HdrFrame);

		return save(HdrFrame, filePath);
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"file <" + filePath.toString() + "> is an unsupported format");

		return false;
	}
}

bool PictureSaver::save(const HdrRgbFrame& frame, const Path& filePath)
{
	const std::string& ext = filePath.getExtension();
	if(ext == ".exr" || ext == ".EXR")
	{
		return saveExr(frame, filePath);
	}
	else if(ext == ".hdr" || ext == ".HDR")
	{
		return saveHdr(frame, filePath);
	}
	else if(
		ext == ".png" || ext == ".PNG" ||
		ext == ".jpg" || ext == ".JPG" ||
		ext == ".bmp" || ext == ".BMP" ||
		ext == ".tga" || ext == ".TGA")
	{
		LdrRgbFrame ldrFrame;
		frame_utils::to_LDR(frame, &ldrFrame);

		return save(ldrFrame, filePath);
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"file <" + filePath.toString() + "> is an unsupported format");

		return false;
	}
}

bool PictureSaver::savePng(const LdrRgbFrame& frame, const Path& filePath)
{
	static_assert(sizeof(LdrComponent) * CHAR_BIT == 8);

	logger.log(ELogLevel::NOTE_MIN, 
		"saving image <" + filePath.toAbsoluteString() + ">");

	return stbi_write_png(
		filePath.toString().c_str(), 
		static_cast<int>(frame.widthPx()),
		static_cast<int>(frame.heightPx()),
		3, 
		frame.getPixelData(), 
		static_cast<int>(frame.widthPx()) * 3) != 0;
}

bool PictureSaver::saveJpg(const LdrRgbFrame& frame, const Path& filePath)
{
	static_assert(sizeof(LdrComponent) * CHAR_BIT == 8);

	logger.log(ELogLevel::NOTE_MIN,
		"saving image <" + filePath.toAbsoluteString() + ">");

	return stbi_write_jpg(
		filePath.toString().c_str(),
		static_cast<int>(frame.widthPx()),
		static_cast<int>(frame.heightPx()),
		3,
		frame.getPixelData(),
		10) != 0;
}

bool PictureSaver::saveBmp(const LdrRgbFrame& frame, const Path& filePath)
{
	static_assert(sizeof(LdrComponent) * CHAR_BIT == 8);

	logger.log(ELogLevel::NOTE_MIN,
		"saving image <" + filePath.toAbsoluteString() + ">");

	return stbi_write_bmp(
		filePath.toString().c_str(),
		static_cast<int>(frame.widthPx()),
		static_cast<int>(frame.heightPx()),
		3,
		frame.getPixelData()) != 0;
}

bool PictureSaver::saveTga(const LdrRgbFrame& frame, const Path& filePath)
{
	static_assert(sizeof(LdrComponent) * CHAR_BIT == 8);

	logger.log(ELogLevel::NOTE_MIN,
		"saving image <" + filePath.toAbsoluteString() + ">");

	return stbi_write_tga(
		filePath.toString().c_str(),
		static_cast<int>(frame.widthPx()),
		static_cast<int>(frame.heightPx()),
		3,
		frame.getPixelData()) != 0;
}

bool PictureSaver::saveHdr(const HdrRgbFrame& frame, const Path& filePath)
{
	static_assert(std::is_same_v<HdrComponent, float>);

	logger.log(ELogLevel::NOTE_MIN,
		"saving image <" + filePath.toAbsoluteString() + ">");

	return stbi_write_hdr(
		filePath.toString().c_str(),
		static_cast<int>(frame.widthPx()),
		static_cast<int>(frame.heightPx()),
		3,
		frame.getPixelData()) != 0;
}

bool PictureSaver::saveExr(const HdrRgbFrame& frame, const Path& filePath)
{
	ExrFileWriter writer(filePath);
	return writer.save(frame);
}

bool PictureSaver::saveExrHighPrecision(const HdrRgbFrame& frame, const Path& filePath)
{
	ExrFileWriter writer(filePath);
	return writer.saveHighPrecision(frame);
}

bool PictureSaver::saveExr(const HdrRgbFrame& frame, std::string& byteBuffer)
{
	ExrFileWriter writer(Path(""));
	return writer.save(frame, byteBuffer);
}

}// end namespace ph
