#pragma once

#include "DataIO/Stream/IOutputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <utility>
#include <memory>
#include <ostream>
#include <string>

namespace ph
{

class StdOutputStream : public IOutputStream
{
public:
	StdOutputStream() = default;
	explicit StdOutputStream(std::unique_ptr<std::ostream> stream);
	StdOutputStream(StdOutputStream&& other) noexcept;

	void write(std::size_t numBytes, const std::byte* bytes) override;
	void writeString(std::string_view str) override;
	void seekPut(std::size_t pos) override;
	std::optional<std::size_t> tellPut() override;
	operator bool () const override;

	std::ostream* getStream() const;

	StdOutputStream& operator = (StdOutputStream&& rhs) noexcept;

protected:
	bool isStreamGoodForWrite() const;

	/*! @brief Check if the stream has any error.
	@exception IOException If there is any error.
	*/
	void ensureStreamIsGoodForWrite() const;

	/*! @brief A description for why the stream is not in a good state.
	*/
	std::string getReasonForError() const;

private:
	std::unique_ptr<std::ostream> m_ostream;
};

// In-header Implementations:

inline StdOutputStream::StdOutputStream(StdOutputStream&& other) noexcept
{
	*this = std::move(other);
}

inline StdOutputStream& StdOutputStream::operator = (StdOutputStream&& rhs) noexcept
{
	m_ostream = std::move(rhs.m_ostream);
	return *this;
}

inline StdOutputStream::operator bool () const
{
	return isStreamGoodForWrite();
}

inline std::ostream* StdOutputStream::getStream() const
{
	return m_ostream.get();
}

inline bool StdOutputStream::isStreamGoodForWrite() const
{
	return m_ostream != nullptr && m_ostream->good();
}

}// end namespace ph
