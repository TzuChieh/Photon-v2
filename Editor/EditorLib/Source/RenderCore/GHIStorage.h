#pragma once

#include <Common/primitive_type.h>

#include <variant>

namespace ph::editor
{

class GHIStorage
{
public:
	using NativeHandle = std::variant<
		std::monostate,
		uint64>;

	virtual ~GHIStorage();

	virtual NativeHandle getNativeHandle();
};

inline auto GHIStorage::getNativeHandle()
-> NativeHandle
{
	return std::monostate();
}


}// end namespace ph::editor
