#pragma once

#include <string>

namespace ph
{

class IDataStream
{
public:
	virtual ~IDataStream() = default;

	/*! @brief Access to the stream's name.
	This method is not meant to be used in a high performance context, unless
	otherwise stated by the overridder. Stream name is not always available.
	*/
	virtual std::string acquireName();

	/*! @brief Whether the stream is in a non-erroneous state.
	*/
	virtual operator bool () const = 0;
};

// In-header Implementations:

inline std::string IDataStream::acquireName()
{
	return "";
}

}// end namespace ph
