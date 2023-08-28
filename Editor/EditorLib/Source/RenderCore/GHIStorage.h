#pragma once

#include "RenderCore/ghi_enums.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <variant>

namespace ph::editor
{

class GHIStorage
{
public:
	using NativeHandle = std::variant<
		std::monostate,
		uint64>;

	explicit GHIStorage(EGHIStorageUsage usage);
	virtual ~GHIStorage();

	virtual void upload(
		const std::byte* rawData,
		std::size_t numBytes) = 0;

	virtual NativeHandle getNativeHandle();

	EGHIStorageUsage getUsage() const;

private:
	EGHIStorageUsage m_usage;
};

inline auto GHIStorage::getNativeHandle()
-> NativeHandle
{
	return std::monostate();
}

inline EGHIStorageUsage GHIStorage::getUsage() const
{
	return m_usage;
}

}// end namespace ph::editor
