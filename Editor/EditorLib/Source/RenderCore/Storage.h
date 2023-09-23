#pragma once

#include "RenderCore/ghi_enums.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <variant>

namespace ph::editor::ghi
{

class Storage
{
public:
	using NativeHandle = std::variant<
		std::monostate,
		uint64>;

	explicit Storage(EStorageUsage usage);
	virtual ~Storage();

	virtual void upload(
		const std::byte* rawData,
		std::size_t numBytes) = 0;

	virtual NativeHandle getNativeHandle();

	EStorageUsage getUsage() const;

private:
	EStorageUsage m_usage;
};

inline auto Storage::getNativeHandle()
-> NativeHandle
{
	return std::monostate();
}

inline EStorageUsage Storage::getUsage() const
{
	return m_usage;
}

}// end namespace ph::editor::ghi
