#pragma once

#include "DataIO/Stream/IInputStream.h"
#include "DataIO/FileSystem/Path.h"
#include "Common/assertion.h"

#include <utility>
#include <memory>
#include <istream>

namespace ph
{

class BinaryFileInputStream : public IInputStream
{
public:
	inline BinaryFileInputStream() = default;
	explicit BinaryFileInputStream(const Path& filePath);
	BinaryFileInputStream(BinaryFileInputStream&& other);

	bool read(std::size_t numBytes, std::byte* out_bytes) override;
	void seekGet(std::size_t pos) override;
	std::size_t tellGet() override;
	operator bool () const override;

	template<typename T>
	bool readData(T* out_data);

	BinaryFileInputStream& operator = (BinaryFileInputStream&& rhs);

private:
	std::unique_ptr<std::istream> m_istream;
};

// In-header Implementations:

inline BinaryFileInputStream::BinaryFileInputStream(BinaryFileInputStream&& other)
{
	*this = std::move(other);
}

inline BinaryFileInputStream& BinaryFileInputStream::operator = (BinaryFileInputStream&& rhs)
{
	m_istream = std::move(rhs.m_istream);
}

inline BinaryFileInputStream::operator bool () const
{
	return m_istream != nullptr && m_istream->good();
}

template<typename T>
inline bool BinaryFileInputStream::readData(T* const out_data)
{
	static_assert(std::is_trivially_copyable_v<T>);
	PH_ASSERT(out_data);

	return read(sizeof(T), reinterpret_cast<std::byte*>(out_data));
}

}// end namespace ph
