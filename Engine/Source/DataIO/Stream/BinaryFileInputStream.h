#pragma once

#include "DataIO/Stream/StdInputStream.h"
#include "DataIO/FileSystem/Path.h"
#include "Common/assertion.h"

#include <type_traits>

namespace ph
{

class BinaryFileInputStream : public StdInputStream
{
public:
	inline BinaryFileInputStream() = default;
	explicit BinaryFileInputStream(const Path& filePath);
	inline BinaryFileInputStream(BinaryFileInputStream&& other) = default;

	template<typename T>
	bool readData(T* out_data);

	inline BinaryFileInputStream& operator = (BinaryFileInputStream&& rhs) = default;
};

// In-header Implementations:

template<typename T>
inline bool BinaryFileInputStream::readData(T* const out_data)
{
	static_assert(std::is_trivially_copyable_v<T>);
	PH_ASSERT(out_data);

	return read(sizeof(T), reinterpret_cast<std::byte*>(out_data));
}

}// end namespace ph
