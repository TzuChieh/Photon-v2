#pragma once

#include "DataIO/Stream/IOutputStream.h"
#include "DataIO/FileSystem/Path.h"

#include <utility>
#include <memory>
#include <ostream>

namespace ph
{

class StdOutputStream : public IOutputStream
{
public:
	StdOutputStream() = default;
	explicit StdOutputStream(std::unique_ptr<std::ostream> stream);
	StdOutputStream(StdOutputStream&& other);

	void write(std::size_t numBytes, const std::byte* bytes) override;
	void seekPut(std::size_t pos) override;
	std::optional<std::size_t> tellPut() override;
	operator bool () const override;

	std::ostream* getStream() const;

	StdOutputStream& operator = (StdOutputStream&& rhs);

private:
	std::unique_ptr<std::ostream> m_ostream;

	/*! @brief Enable the use of exceptions when std::ostream has error.
	This call will immediately throw std::ostream::failure if the member 
	std::ostream is already in an error state.
	*/
	void useExceptionForOStreamError();
};

// In-header Implementations:

inline StdOutputStream::StdOutputStream(StdOutputStream&& other)
{
	*this = std::move(other);
}

inline StdOutputStream& StdOutputStream::operator = (StdOutputStream&& rhs)
{
	m_ostream = std::move(rhs.m_ostream);
	return *this;
}

inline StdOutputStream::operator bool () const
{
	return m_ostream != nullptr && m_ostream->good();
}

inline std::ostream* StdOutputStream::getStream() const
{
	return m_ostream.get();
}

}// end namespace ph
