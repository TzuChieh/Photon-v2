#pragma once

#include "EditorCore/Storage/fwd.h"

#include <Common/primitive_type.h>

namespace ph::editor
{

/*! Dummy graphics object types for resource management.
*/
///@{
class GHITextureObject
{};

class GHIVertexStorageObject
{};

class GHIIndexStorageObject
{};

class GHIMeshObject
{};

class GHIShaderObject
{};

class GHIShaderProgramObject
{};

class GHIFramebufferObject
{};
///@}

// Handle types
using GHITextureHandle = TWeakHandle<GHITextureObject, uint32, uint32>;
using GHIVertexStorageHandle = TWeakHandle<GHIVertexStorageObject, uint32, uint32>;
using GHIIndexStorageHandle = TWeakHandle<GHIIndexStorageObject, uint32, uint32>;
using GHIMeshHandle = TWeakHandle<GHIMeshObject, uint32, uint32>;
using GHIShaderHandle = TWeakHandle<GHIShaderObject, uint32, uint32>;
using GHIShaderProgramHandle = TWeakHandle<GHIShaderProgramObject, uint32, uint32>;
using GHIFramebufferHandle = TWeakHandle<GHIFramebufferObject, uint32, uint32>;

// Info types
class GHIInfoSampleState;
class GHIInfoTextureFormat;
class GHIInfoFramebufferFormat;
class GHIInfoFramebufferAttachment;
class GHIInfoShaderSet;
class GHIInfoVertexAttributeLocator;
class GHIInfoVertexGroupFormat;
class GHIInfoMeshVertexLayout;

}// end namespace ph::editor
