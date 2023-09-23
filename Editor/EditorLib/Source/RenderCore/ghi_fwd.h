#pragma once

#include "EditorCore/Storage/fwd.h"
#include "EditorCore/Storage/TWeakHandle.h"

#include <Common/primitive_type.h>

#include <variant>

namespace ph::editor::ghi
{

/*! Dummy graphics object types for resource management.
*/
///@{
class TextureObject
{};

class VertexStorageObject
{};

class IndexStorageObject
{};

class MeshObject
{};

class ShaderObject
{};

class ShaderProgramObject
{};

class FramebufferObject
{};

class MemoryObject
{};
///@}

// Handle types
using TextureHandle = TWeakHandle<TextureObject, uint32, uint32>;
using VertexStorageHandle = TWeakHandle<VertexStorageObject, uint32, uint32>;
using IndexStorageHandle = TWeakHandle<IndexStorageObject, uint32, uint32>;
using MeshHandle = TWeakHandle<MeshObject, uint32, uint32>;
using ShaderHandle = TWeakHandle<ShaderObject, uint32, uint32>;
using ShaderProgramHandle = TWeakHandle<ShaderProgramObject, uint32, uint32>;
using FramebufferHandle = TWeakHandle<FramebufferObject, uint32, uint32>;
using MemoryHandle = TWeakHandle<MemoryObject, uint32, uint32>;

// Native handle types
using TextureNativeHandle = std::variant<
	std::monostate,
	uint64>;

// Basic info types
class SampleStateInfo;
class TextureFormatInfo;
class FramebufferFormatInfo;
class ShaderSetInfo;
class VertexAttributeLocatorInfo;
class VertexGroupFormatInfo;
class MeshVertexLayoutInfo;
class DeviceCapabilityInfo;

// Description info types
class TextureDesc;
class FramebufferDesc;

}// end namespace ph::editor::ghi
