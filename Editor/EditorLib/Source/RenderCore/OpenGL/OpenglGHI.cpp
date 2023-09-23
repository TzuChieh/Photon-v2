#include "RenderCore/OpenGL/OpenglGHI.h"
#include "Platform/Platform.h"
#include "RenderCore/ghi_infos.h"
#include "RenderCore/OpenGL/OpenglContext.h"
#include "RenderCore/OpenGL/opengl_enums.h"
#include "RenderCore/OpenGL/OpenglTexture.h"
#include "RenderCore/OpenGL/OpenglFramebuffer.h"
#include "RenderCore/OpenGL/Opengl.h"
#include "RenderCore/OpenGL/OpenglMesh.h"
#include "RenderCore/OpenGL/OpenglIndexStorage.h"
#include "RenderCore/OpenGL/OpenglVertexStorage.h"
#include "RenderCore/OpenGL/OpenglShader.h"
#include "RenderCore/OpenGL/OpenglShaderProgram.h"

#include "ThirdParty/glad2_with_GLFW3.h"

#include <Common/assertion.h>
#include <Common/logging.h>
#include <Utility/utility.h>
#include <Utility/TBitFlags.h>

#include <string_view>
#include <cstddef>
#include <format>

namespace ph::editor::ghi
{

PH_DEFINE_INTERNAL_LOG_GROUP(OpenglGHI, GHI);

namespace
{

inline std::string_view GLubyte_to_sv(const GLubyte* const ubytes)
{
	PH_ASSERT(ubytes);
	return std::string_view(reinterpret_cast<const char*>(ubytes));
}

inline std::string_view debug_source_GLenum_to_sv(const GLenum dbgSource)
{
	switch(dbgSource)
	{
	case GL_DEBUG_SOURCE_API:
		return "API";

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return "Window System";

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return "Shader Compiler";

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return "Third Party";

	case GL_DEBUG_SOURCE_APPLICATION:
		return "Application";

	case GL_DEBUG_SOURCE_OTHER:
		return "Other";

	default:
		return "(unknown)";
	}
}

inline std::string_view debug_type_GLenum_to_sv(const GLenum dbgType)
{
	switch(dbgType)
	{
	case GL_DEBUG_TYPE_ERROR:
		return "Error";

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return "Deprecated Behavior";

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return "Undefined Behavior";

	case GL_DEBUG_TYPE_PORTABILITY:
		return "Portability";

	case GL_DEBUG_TYPE_PERFORMANCE:
		return "Performance";

	case GL_DEBUG_TYPE_OTHER:
		return "Other";

	case GL_DEBUG_TYPE_MARKER:
		return "Marker";

	default:
		return "(unknown)";
	}
}

}// end anonymous namespace

}// end namespace ph::editor::ghi

extern "C"
{

inline void APIENTRY ph_editor_OpenGL_debug_callback(
	const GLenum        source,
	const GLenum        type,
	const GLuint        id,
	const GLenum        severity,
	const GLsizei       length,
    const GLchar* const message,
    const void* const   userParam)
{
	using namespace ph::editor::ghi;

	auto debugStr = std::format("{} (source: {}, type: {}, id: {})",
		std::string_view(reinterpret_cast<const char*>(message), length),
		debug_source_GLenum_to_sv(source),
		debug_type_GLenum_to_sv(type),
		id);

	switch(severity)
	{
	// Anything that isn't an error or performance issue
	case GL_DEBUG_SEVERITY_NOTIFICATION:
		// You can uncomment this to enable trace level info logging
		//PH_LOG(OpenglGHI, "{}", debugStr);
		break;

	// Redundant state change performance warning, or unimportant undefined behavior
	case GL_DEBUG_SEVERITY_LOW:
		PH_LOG(OpenglGHI, "{}", debugStr);
		break;

	// Major performance warnings, shader compilation/linking warnings, or the use of deprecated functionality
	case GL_DEBUG_SEVERITY_MEDIUM:
		PH_LOG_WARNING(OpenglGHI, "{}", debugStr);
		break;

	// All OpenGL Errors, shader compilation/linking errors, or highly-dangerous undefined behavior
	case GL_DEBUG_SEVERITY_HIGH:
	default:
		PH_LOG_ERROR(OpenglGHI, "{}", debugStr);
		break;
	}
}

}// end extern "C"

namespace ph::editor::ghi
{

OpenglGHI::OpenglGHI(OpenglContext& ctx, GLFWwindow* const glfwWindow, const bool hasDebugContext)

	: GHI(EGraphicsAPI::OpenGL)

	, m_ctx            (ctx)
	, m_glfwWindow     (glfwWindow)
	, m_hasDebugContext(hasDebugContext)
	, m_isLoaded       (false)
#if PH_DEBUG
	, m_loadThreadId   ()
#endif
	, m_deviceCapability(nullptr)
{}

OpenglGHI::~OpenglGHI()
{
	PH_ASSERT(!m_isLoaded);
}

void OpenglGHI::load()
{
#if PH_DEBUG
	m_loadThreadId = std::this_thread::get_id();
#endif

	if(m_isLoaded)
	{
		return;
	}

	if(!m_glfwWindow)
	{
		throw PlatformException(
			"invalid GLFW window (null)");
	}

	glfwMakeContextCurrent(m_glfwWindow);

	const int version = gladLoadGL(glfwGetProcAddress);
	if(version == 0)
	{
		throw PlatformException(
			"failed to load OpenGL (glad load failed)");
	}

	if(m_hasDebugContext)
	{
		glDebugMessageCallback(ph_editor_OpenGL_debug_callback, nullptr);
	}

	PH_LOG(OpenglGHI,
		"loaded OpenGL {}.{}", 
		GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

	// Log information provided by OpenGL itself
	PH_LOG(OpenglGHI,
		"Vendor: {}, Renderer: {}, Version: {}, GLSL Version: {}",
		GLubyte_to_sv(glGetString(GL_VENDOR)),
		GLubyte_to_sv(glGetString(GL_RENDERER)),
		GLubyte_to_sv(glGetString(GL_VERSION)),
		GLubyte_to_sv(glGetString(GL_SHADING_LANGUAGE_VERSION)));

	m_isLoaded = true;
}

void OpenglGHI::unload()
{
	PH_ASSERT(std::this_thread::get_id() == m_loadThreadId);

	if(m_isLoaded)
	{
		glfwMakeContextCurrent(nullptr);
	}

	m_isLoaded = false;
}

void OpenglGHI::setViewport(uint32 xPx, uint32 yPx, uint32 widthPx, uint32 heightPx)
{
	glViewport(
		lossless_cast<GLint>(xPx),
		lossless_cast<GLint>(yPx),
		lossless_cast<GLsizei>(widthPx),
		lossless_cast<GLsizei>(heightPx));
}

void OpenglGHI::clearBuffer(const EClearTarget targets)
{
	const TEnumFlags<EClearTarget> flags({targets});

	GLbitfield mask = 0;

	if(flags.hasAny({EClearTarget::Color}))
	{
		mask |= GL_COLOR_BUFFER_BIT;
	}

	if(flags.hasAny({EClearTarget::Depth}))
	{
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	if(flags.hasAny({EClearTarget::Stencil}))
	{
		mask |= GL_STENCIL_BUFFER_BIT;
	}

	glClear(mask);
}

void OpenglGHI::setClearColor(const math::Vector4F& color)
{
	glClearColor(
		lossless_cast<GLclampf>(color.r()),
		lossless_cast<GLclampf>(color.g()),
		lossless_cast<GLclampf>(color.b()),
		lossless_cast<GLclampf>(color.a()));
}

void OpenglGHI::draw(Mesh& mesh, const EMeshDrawMode drawMode)
{
	mesh.bind();

	if(mesh.hasIndexStorage())
	{
		const IndexStorage& indexStorage = mesh.getIndexStorage();

		glDrawElements(
			opengl::to_primitive_type(drawMode), 
			lossless_cast<GLsizei>(indexStorage.numIndices()),
			opengl::to_data_type(indexStorage.getIndexType()),
			reinterpret_cast<GLbyte*>(0));
	}
	else if(mesh.numVertexStorages() > 0)
	{
		glDrawArrays(
			opengl::to_primitive_type(drawMode),
			0,
			lossless_cast<GLsizei>(mesh.getVertexStorage(0).numVertices()));
	}
	else
	{
		PH_LOG_ERROR(OpenglGHI,
			"cannot draw mesh; index buffer: {}, # vertex buffers: {}, draw mode: {}",
			mesh.hasIndexStorage(), mesh.numVertexStorages(), enum_to_value(drawMode));
	}
}

void OpenglGHI::swapBuffers()
{
	glfwSwapBuffers(m_glfwWindow);
}

bool OpenglGHI::tryUploadPixelData(
	TextureHandle handle,
	TSpanView<std::byte> pixelData,
	EPixelFormat pixelFormat,
	EPixelComponent pixelComponent)
{
	OpenglTexture* texture = m_ctx.getObjectManager().textures.get(handle);
	if(!texture)
	{
		return false;
	}

	texture->uploadPixelData(pixelData, pixelFormat, pixelComponent);
	return true;
}

TextureNativeHandle OpenglGHI::tryGetTextureNativeHandle(const TextureHandle handle)
{
	OpenglTexture* texture = m_ctx.getObjectManager().textures.get(handle);
	if(!texture || texture->textureID == 0)
	{
		return {};
	}

	return static_cast<uint64>(texture->textureID);
}

void OpenglGHI::attachTextureToFramebuffer(
	uint32 attachmentIdx,
	TextureHandle textureHandle,
	FramebufferHandle framebufferHandle)
{
	OpenglTexture* texture = m_ctx.getObjectManager().textures.get(textureHandle);
	OpenglFramebuffer* framebuffer = m_ctx.getObjectManager().framebuffers.get(framebufferHandle);
	if(!texture || !framebuffer)
	{
		return;
	}

	if(texture->isColor())
	{
		framebuffer->attachColor(attachmentIdx, *texture, textureHandle);
	}
	else
	{
		if(attachmentIdx != 0)
		{
			PH_LOG_WARNING(OpenglGHI,
				"Attempting to attach depth stencil texture to invalid attachment point {}.",
				attachmentIdx);
		}

		framebuffer->attachDepthStencil(*texture, textureHandle);
	}
}

std::shared_ptr<Shader> OpenglGHI::createShader(
	std::string name,
	const EShadingStage shadingStage,
	std::string shaderSource)
{
	return std::make_shared<OpenglShader>(
		std::move(name),
		shadingStage,
		std::move(shaderSource));
}

std::shared_ptr<ShaderProgram> OpenglGHI::createShaderProgram(
	std::string name,
	const ShaderSetInfo& shaders)
{
	/*if(shaders.vertexShader && shaders.fragmentShader)
	{
		return std::make_shared<OpenglShaderProgram>(
			std::move(name),
			shaders.vertexShader,
			shaders.fragmentShader);
	}
	else
	{
		PH_LOG_ERROR(OpenglGHI,
			"cannot create shader program (some shaders are missing)");
		return nullptr;
	}*/
	return nullptr;
}

std::shared_ptr<VertexStorage> OpenglGHI::createVertexStorage(
	const VertexGroupFormatInfo& format,
	const std::size_t numVertices,
	const EStorageUsage usage)
{
	return std::make_shared<OpenglVertexStorage>(
		format,
		numVertices,
		usage);
}

std::shared_ptr<IndexStorage> OpenglGHI::createIndexStorage(
	const EStorageElement indexType,
	const std::size_t numIndices,
	const EStorageUsage usage)
{
	return std::make_shared<OpenglIndexStorage>(
		indexType, 
		numIndices,
		usage);
}

std::shared_ptr<Mesh> OpenglGHI::createMesh(
	const MeshVertexLayoutInfo& layout,
	TSpanView<std::shared_ptr<VertexStorage>> vertexStorages,
	const std::shared_ptr<IndexStorage>& indexStorage)
{
	return std::make_shared<OpenglMesh>(
		layout,
		vertexStorages,
		indexStorage);
}

DeviceCapabilityInfo OpenglGHI::getDeviceCapabilities()
{
	if(m_deviceCapability != nullptr)
	{
		return *m_deviceCapability;
	}

	// Reference: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml

	m_deviceCapability = std::make_unique<DeviceCapabilityInfo>();

	DeviceCapabilityInfo& c = *m_deviceCapability;
	c.maxTextureUnitsForVertexShadingStage = Opengl::getInteger<uint8>(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS);
	c.maxTextureUnitsForFragmentShadingStage = Opengl::getInteger<uint8>(GL_MAX_TEXTURE_IMAGE_UNITS);
	
	// TODO: also see GL_MAX_VERTEX_ATTRIB_BINDINGS
	c.maxVertexAttributes = Opengl::getInteger<uint8>(GL_MAX_VERTEX_ATTRIBS);
	
	return c;
}

void OpenglGHI::beginRawCommand()
{
	// TODO: currently no state yet
}

void OpenglGHI::endRawCommand()
{
	// TODO: currently no state yet
}

}// end namespace ph::editor::ghi
