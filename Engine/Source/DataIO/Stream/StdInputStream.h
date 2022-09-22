#pragma once

#include "DataIO/Stream/IInputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <utility>
#include <istream>
#include <memory>
#include <string>

namespace ph
{

class StdInputStream : public IInputStream
{
public:
	inline StdInputStream() = default;
	explicit StdInputStream(std::unique_ptr<std::istream> stream);
	StdInputStream(StdInputStream&& other);

	void read(std::size_t numBytes, std::byte* out_bytes) override;
	void readString(std::string* out_string, char delimiter) override;
	void seekGet(std::size_t pos) override;
	std::optional<std::size_t> tellGet() override;
	operator bool () const override;
	std::size_t readSome(std::size_t numBytes, std::byte* out_bytes) override;

	std::istream* getStream() const;

	StdInputStream& operator = (StdInputStream&& rhs);

protected:
	bool isStreamGoodForRead() const;

	/*! @brief Check if the stream has any error.
	@exception IOException If there is any error.
	*/
	void ensureStreamIsGoodForRead() const;

	/*! @brief A description for why the stream is not in a good state.
	*/
	std::string getReasonForError() const;

private:
	std::unique_ptr<std::istream> m_istream;
};

// In-header Implementations:

inline StdInputStream::StdInputStream(StdInputStream&& other)
{
	*this = std::move(other);
}

inline StdInputStream& StdInputStream::operator = (StdInputStream&& rhs)
{
	m_istream = std::move(rhs.m_istream);
}

inline StdInputStream::operator bool () const
{
	return isStreamGoodForRead();
}

inline std::istream* StdInputStream::getStream() const
{
	return m_istream.get();
}

inline bool StdInputStream::isStreamGoodForRead() const
{
	return m_istream != nullptr && m_istream->good();
}

}// end namespace ph
