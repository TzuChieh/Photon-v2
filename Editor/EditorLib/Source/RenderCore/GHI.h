#pragma once

#include "RenderCore/ghi_fwd.h"
#include "RenderCore/ghi_enums.h"
#include "RenderCore/Shader.h"
#include "RenderCore/ShaderProgram.h"
#include "RenderCore/VertexStorage.h"
#include "RenderCore/IndexStorage.h"
#include "RenderCore/Mesh.h"

#include <Utility/INoCopyAndMove.h>
#include <Common/logging.h>
#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Math/math_fwd.h>
#include <Utility/TSpan.h>

#include <cstddef>
#include <type_traits>
#include <source_location>
#include <string>
#include <memory>

namespace ph::editor::ghi
{

PH_DECLARE_LOG_GROUP(GHI);

class Mesh;

/*! @brief Graphics API abstraction.
@exception PlatformException When error occurred and the platform must terminate its operations.
*/
class GHI : private INoCopyAndMove
{
public:
	static std::string toString(EGraphicsAPI apiType);

public:
	explicit GHI(EGraphicsAPI apiType);
	virtual ~GHI();

	/*! @brief Load and initiate the GHI.
	load() and unload() must be called on the same thread. All remaining methods must be 
	called between calls to load() and unload() (except for ctor and dtor).
	*/
	virtual void load() = 0;

	/*! @brief Unload and clean up the GHI.
	Must be called on the same thread as load(). See load() for more information on the 
	requirements of this method.
	*/
	virtual void unload() = 0;

	/*! @brief Set how NDC corresponds to pixels on the display.
	@param xPx X coordinate of the lower-left corner of the viewport, in pixels.
	@param yPx Y coordinate of the lower-left corner of the viewport, in pixels.
	@param widthPx Width of the viewport, in pixels.
	@param heightPx Height of the viewport, in pixels.
	*/
	virtual void setViewport(uint32 xPx, uint32 yPx, uint32 widthPx, uint32 heightPx) = 0;

	virtual void clearBuffer(EClearTarget target) = 0;

	virtual void setClearColor(const math::Vector4F& color) = 0;

	virtual void draw(Mesh& mesh, EMeshDrawMode drawMode) = 0;

	virtual void swapBuffers() = 0;

	/*! @brief Try to upload pixel data to a texture.
	@return Whether the data is uploaded. If the texture does not exist, `false` is returned.
	*/
	virtual bool tryUploadPixelData(
		TextureHandle handle,
		TSpanView<std::byte> pixelData,
		EPixelFormat pixelFormat,
		EPixelComponent pixelComponent) = 0;

	virtual bool tryUploadPixelDataTo2DRegion(
		TextureHandle handle,
		const math::Vector2UI& regionOriginPx,
		const math::Vector2UI& regionSizePx,
		TSpanView<std::byte> pixelData,
		EPixelFormat pixelFormat,
		EPixelComponent pixelComponent) = 0;

	virtual TextureNativeHandle tryGetTextureNativeHandle(TextureHandle handle) = 0;

	virtual void attachTextureToFramebuffer(
		uint32 attachmentIdx,
		TextureHandle textureHandle,
		FramebufferHandle framebufferHandle) = 0;

	virtual std::shared_ptr<Shader> createShader(
		std::string name, 
		EShadingStage shadingStage,
		std::string shaderSource) = 0;

	virtual std::shared_ptr<ShaderProgram> createShaderProgram(
		std::string name,
		const ShaderSetInfo& shaders) = 0;

	virtual std::shared_ptr<VertexStorage> createVertexStorage(
		const VertexGroupFormatInfo& format,
		std::size_t numVertices,
		EStorageUsage usage) = 0;

	virtual std::shared_ptr<IndexStorage> createIndexStorage(
		EStorageElement indexType,
		std::size_t numIndices,
		EStorageUsage usage) = 0;

	virtual std::shared_ptr<Mesh> createMesh(
		const MeshVertexLayoutInfo& layout,
		TSpanView<std::shared_ptr<VertexStorage>> vertexStorages,
		const std::shared_ptr<IndexStorage>& indexStorage) = 0;

	virtual DeviceCapabilityInfo getDeviceCapabilities();

	template<EGraphicsAPI API_TYPE, typename CommandCallingFunctor>
	void rawCommand(
		CommandCallingFunctor functor,
		std::source_location  srcLocation = std::source_location::current());

private:
	/*!
	Allows the implementer to save/cache internal states before raw commands are inserted manually, 
	and restore the states afterwards.
	*/
	///@{
	virtual void beginRawCommand() = 0;
	virtual void endRawCommand() = 0;
	///@}

	EGraphicsAPI m_apiType;
};

template<EGraphicsAPI API_TYPE, typename CommandCallingFunctor>
inline void GHI::rawCommand(
	CommandCallingFunctor      functor,
	const std::source_location srcLocation)
{
	static_assert(std::is_invocable_v<CommandCallingFunctor>,
		"must be callable without any argument");

	if(API_TYPE != m_apiType)
	{
		PH_LOG_ERROR(GHI,
			"cannot call raw command of type {} under current GHI type {}, "
			"skipping command at {}({}:{})",
			toString(API_TYPE), toString(m_apiType), srcLocation.file_name(), srcLocation.line(), srcLocation.column());
		
		return;
	}

	beginRawCommand();
	functor();
	endRawCommand();
}

}// end namespace ph::editor::ghi
