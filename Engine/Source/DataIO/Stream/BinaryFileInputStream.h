#pragma once

#include "DataIO/Stream/BinaryDataInputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <memory>
#include <istream>

namespace ph
{

class BinaryFileInputStream : public BinaryDataInputStream
{
public:
	explicit BinaryFileInputStream(const Path& filePath);

	bool read(std::size_t numBytes, std::byte* out_bytes) override;

	void seekGet(std::size_t pos) override;
	std::size_t tellGet() const override;

private:
	std::unique_ptr<std::istream> m_istream;
};

}// end namespace ph
