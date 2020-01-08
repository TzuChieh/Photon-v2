#pragma once

#include "DataIO/Stream/IInputStream.h"
#include "Common/assertion.h"

#include <type_traits>

namespace ph
{

class BinaryDataInputStream : public IInputStream
{
public:
	std::size_t read(std::size_t numBytes, std::byte* out_bytes) override = 0;

	void seekGet(std::size_t pos) override = 0;
	std::size_t tellGet() const override = 0;

	template<typename T>
	bool readData(T* out_data);
};

// In-header Implementations:

template<typename T>
inline bool BinaryDataInputStream::readData(T* const out_data)
{
	static_assert(std::is_trivially_copyable_v<T>);
	PH_ASSERT(out_data);

	const auto readBytes = read(sizeof(T), reinterpret_cast<std::byte*>(out_data));
	PH_ASSERT_LE(readBytes, sizeof(T));
	return readBytes == sizeof(T);
}

}// end namespace ph
