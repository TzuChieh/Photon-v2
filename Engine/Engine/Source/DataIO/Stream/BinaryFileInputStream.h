#pragma once

#include "DataIO/Stream/StdInputStream.h"
#include "DataIO/FileSystem/Path.h"
#include "Utility/TSpan.h"

#include <Common/assertion.h>
#include <Common/memory.h>

#include <type_traits>
#include <cstddef>
#include <optional>

namespace ph
{

class BinaryFileInputStream : public StdInputStream
{
public:
	inline BinaryFileInputStream() = default;
	explicit BinaryFileInputStream(const Path& filePath);
	inline BinaryFileInputStream(BinaryFileInputStream&& other) = default;

	template<typename T, bool DO_BYTE_REVERSAL = false>
	void readData(T* out_data);

	template<typename T, bool DO_BYTE_REVERSAL = false>
	void readData(TSpan<T> out_data);

	std::optional<std::size_t> getFileSizeInBytes() const;

	inline BinaryFileInputStream& operator = (BinaryFileInputStream&& rhs) = default;

private:
	Path m_filePath;
};

// In-header Implementations:

template<typename T, bool DO_BYTE_REVERSAL>
inline void BinaryFileInputStream::readData(T* const out_data)
{
	readData(TSpan<T>{out_data, 1});
}

template<typename T, bool DO_BYTE_REVERSAL>
inline void BinaryFileInputStream::readData(TSpan<T> out_data)
{
	static_assert(std::is_trivially_copyable_v<T>);
	PH_ASSERT(out_data.data());

	auto const dataAsBytes = std::as_writable_bytes(out_data);
	read(dataAsBytes.size(), dataAsBytes.data());

	if constexpr(DO_BYTE_REVERSAL)
	{
		for(std::size_t di = 0; di < out_data.size(); ++di)
		{
			reverse_bytes<sizeof(T)>(reinterpret_cast<std::byte*>(&out_data[di]));
		}
	}
}

}// end namespace ph
