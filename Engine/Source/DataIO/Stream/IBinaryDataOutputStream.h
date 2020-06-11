#pragma once

#include "DataIO/Stream/IOutputStream.h"
#include "Common/assertion.h"

#include <type_traits>

namespace ph
{

class IBinaryDataOutputStream : public IOutputStream
{
public:
	bool write(std::size_t numBytes, const std::byte* bytes) override = 0;

	void seekPut(std::size_t pos) override = 0;
	std::size_t tellPut() override = 0;

	operator bool() const override = 0;

	template<typename T>
	bool writeData(const T* data);
};

// In-header Implementations:

template<typename T>
inline bool IBinaryDataOutputStream::writeData(const T* const data)
{
	static_assert(std::is_trivially_copyable_v<T>);
	PH_ASSERT(out_data);

	return write(sizeof(T), reinterpret_cast<const std::byte*>(data));;
}

}// end namespace ph
