#pragma once

#include "DataIO/Stream/IInputStream.h"
#include "Common/assertion.h"

#include <type_traits>

namespace ph
{

class IBinaryDataInputStream : public IInputStream
{
public:
	bool read(std::size_t numBytes, std::byte* out_bytes) override = 0;
	void seekGet(std::size_t pos) override = 0;
	std::size_t tellGet() override = 0;
	operator bool () const override = 0;

	template<typename T>
	bool readData(T* out_data);
};

// In-header Implementations:

template<typename T>
inline bool IBinaryDataInputStream::readData(T* const out_data)
{
	static_assert(std::is_trivially_copyable_v<T>);
	PH_ASSERT(out_data);

	return read(sizeof(T), reinterpret_cast<std::byte*>(out_data));;
}

}// end namespace ph
