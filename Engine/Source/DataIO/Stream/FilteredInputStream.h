#pragma once

#include "DataIO/Stream/IInputStream.h"
#include "Common/assertion.h"

#include <string>
#include <memory>
#include <utility>

namespace ph
{

class FilteredInputStream : public IInputStream
{
public:
	explicit FilteredInputStream(std::unique_ptr<IInputStream> srcStream);

	std::size_t read(std::size_t numBytes, std::byte* out_bytes) override = 0;
	void seekGet(std::size_t pos) override = 0;
	void seekPut(std::size_t pos) override = 0;
	std::size_t tellGet() const override = 0;
	std::size_t tellPut() const override = 0;

protected:
	IInputStream* getSrcStream() const;

private:
	std::unique_ptr<IInputStream> m_srcStream;
};

// In-header Implementations:

inline FilteredInputStream::FilteredInputStream(std::unique_ptr<IInputStream> srcStream) :
	m_srcStream(std::move(srcStream))
{
	PH_ASSERT(m_srcStream);
}

inline IInputStream* FilteredInputStream::getSrcStream() const
{
	return m_srcStream.get();
}

}// end namespace ph
