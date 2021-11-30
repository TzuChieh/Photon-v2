#pragma once

#include "DataIO/Stream/StdOutputStream.h"
#include "DataIO/FileSystem/Path.h"
#include "Common/assertion.h"

#include <type_traits>

namespace ph
{

class BinaryFileOutputStream : public StdOutputStream
{
public:
	inline BinaryFileOutputStream() = default;
	explicit BinaryFileOutputStream(const Path& filePath);
	inline BinaryFileOutputStream(BinaryFileOutputStream&& other) = default;

	template<typename T>
	void writeData(const T* data);

	inline BinaryFileOutputStream& operator = (BinaryFileOutputStream&& rhs) = default;
};

// In-header Implementations:

template<typename T>
inline void BinaryFileOutputStream::writeData(const T* const data)
{
	static_assert(std::is_trivially_copyable_v<T>);
	PH_ASSERT(data);

	return write(sizeof(T), reinterpret_cast<const std::byte*>(data));
}

}// end namespace ph
