#pragma once

#include "DataIO/Stream/StdOutputStream.h"
#include "DataIO/FileSystem/Path.h"
#include "Utility/TSpan.h"

#include <Common/assertion.h>
#include <Common/memory.h>

#include <type_traits>
#include <array>
#include <algorithm>
#include <cstddef>

namespace ph
{

class BinaryFileOutputStream : public StdOutputStream
{
public:
	inline BinaryFileOutputStream() = default;
	explicit BinaryFileOutputStream(const Path& filePath);
	inline BinaryFileOutputStream(BinaryFileOutputStream&& other) = default;

	template<typename T, bool DO_BYTE_REVERSAL = false>
	void writeData(const T* data);

	template<typename T, bool DO_BYTE_REVERSAL = false>
	void writeData(TSpanView<T> data);

	inline BinaryFileOutputStream& operator = (BinaryFileOutputStream&& rhs) = default;
};

// In-header Implementations:

template<typename T, bool DO_BYTE_REVERSAL>
inline void BinaryFileOutputStream::writeData(const T* const data)
{
	writeData<T, DO_BYTE_REVERSAL>(TSpanView<T>{data, 1});
}

template<typename T, bool DO_BYTE_REVERSAL>
inline void BinaryFileOutputStream::writeData(TSpanView<T> data)
{
	static_assert(std::is_trivially_copyable_v<T>);
	PH_ASSERT(data.data());

	auto const dataAsBytes = std::as_bytes(data);
	if constexpr(DO_BYTE_REVERSAL)
	{
		for(std::size_t di = 0; di < data.size(); ++di)
		{
			std::array<std::byte, sizeof(T)> reversedBytes;
			std::copy_n(
				reinterpret_cast<const std::byte*>(&data[di]), sizeof(T), reversedBytes.data());
			reverse_bytes<sizeof(T)>(reversedBytes.data());

			// OPT: this is calling write() a lot as we do not have large buffer for storing the
			// reversed bytes; should find a way to implement a more efficient way for arrays

			write(sizeof(T), reversedBytes.data());
		}
	}
	else
	{
		write(dataAsBytes.size(), dataAsBytes.data());
	}
}

}// end namespace ph
