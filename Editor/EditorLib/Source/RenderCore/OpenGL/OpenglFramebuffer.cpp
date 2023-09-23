#include "RenderCore/OpenGL/OpenglFramebuffer.h"
#include "RenderCore/ghi_infos.h"
#include "RenderCore/OpenGL/OpenglTexture.h"
#include "RenderCore/OpenGL/opengl_config.h"

#include <Utility/utility.h>
#include <Common/assertion.h>
#include <Common/logging.h>



namespace ph::editor::ghi
{

//OpenglFramebuffer::OpenglFramebuffer(const GHIInfoFramebufferAttachment& attachments)
//
//	: GHIFramebuffer(attachments)
//
//	, m_attachments(attachments)
//	, m_colorTextureIDs{}
//	, m_depthStencilTextureID(0)
//	, m_framebufferID(0)
//{
//	glCreateFramebuffers(1, &m_framebufferID);
//
//	for(uint32 attachmentIdx = 0; attachmentIdx < m_attachments.colorFormats.size(); ++attachmentIdx)
//	{
//		createDeviceColorTexture(attachmentIdx);
//	}
//	createDeviceDepthStencilTexture();
//
//	if(glCheckNamedFramebufferStatus(m_framebufferID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//	{
//		PH_DEFAULT_LOG_ERROR("[OpenglFramebuffer] {}", getFramebufferStatusInfo(m_framebufferID));
//	}
//}

void OpenglFramebuffer::createBuffer(const FramebufferDesc& desc)
{
	PH_ASSERT(!hasResource());

	widthPx = lossless_cast<GLsizei>(desc.sizePx.x());
	heightPx = lossless_cast<GLsizei>(desc.sizePx.y());
	numSamples = lossless_cast<GLsizei>(desc.numSamples);

	for(int fi = 0; fi < desc.colorFormats.size(); ++fi)
	{
		if(fi >= colorAttachments.size() && !desc.colorFormats[fi].isEmpty())
		{
			PH_DEFAULT_LOG_ERROR(
				"Too many color attachments, max = {}, ignoring the rest.", MAX_COLOR_ATTACHMENTS);
			break;
		}

		colorAttachments[fi].internalFormat = opengl::to_internal_format(desc.colorFormats[fi].pixelFormat);
	}

	depthStencilAttachment.internalFormat = opengl::to_internal_format(desc.depthStencilFormat.pixelFormat);
	if(depthStencilAttachment.internalFormat != GL_NONE)
	{
		switch(depthStencilAttachment.internalFormat)
		{
		case GL_DEPTH_COMPONENT32F:
		case GL_DEPTH_COMPONENT24:
		case GL_DEPTH_COMPONENT16:
			depthStencilAttachment.attachmentType = GL_DEPTH_ATTACHMENT;
			break;
		
		case GL_DEPTH24_STENCIL8:
		case GL_DEPTH32F_STENCIL8:
			depthStencilAttachment.attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
			break;
		
		// Other stencil bitdepths are strongly unrecommended, see
		// https://www.khronos.org/opengl/wiki/Image_Format#Stencil_only
		case GL_STENCIL_INDEX8:
			depthStencilAttachment.attachmentType = GL_STENCIL_ATTACHMENT;
			break;
		
		default:
			PH_ASSERT_UNREACHABLE_SECTION();
			break;
		}
	}

	glCreateFramebuffers(1, &framebufferID);
	
	// Make the empty framebuffer valid for use. None of the fragment shader outputs will be written
	// to anywhere in this case as nothing is attached to the framebuffer yet, but rendering can still
	// proceed without problems. These parameters are ignored if textures are attached later.
	glNamedFramebufferParameteri(framebufferID, GL_FRAMEBUFFER_DEFAULT_WIDTH, widthPx);
	glNamedFramebufferParameteri(framebufferID, GL_FRAMEBUFFER_DEFAULT_HEIGHT, heightPx);

#if PH_DEBUG_OPENGL
	checkCompleteness();
#endif
}

void OpenglFramebuffer::attachColor(
	uint32 attachmentIdx,
	const OpenglTexture& colorTexture,
	TextureHandle handle)
{
	PH_ASSERT(hasResource());

	if(attachmentIdx >= colorAttachments.size() ||
	   colorAttachments[attachmentIdx].internalFormat == GL_NONE)
	{
		PH_DEFAULT_LOG_ERROR(
			"Too many color attachments (index = {}, empty = {}, max = {}), ignoring.",
			attachmentIdx,
			attachmentIdx >= colorAttachments.size() ? true : colorAttachments[attachmentIdx].internalFormat == GL_NONE,
			MAX_COLOR_ATTACHMENTS);
		return;
	}

	if(!colorTexture.hasResource() || !handle)
	{
		PH_DEFAULT_LOG(
			"Nullifying color attachment {}.", attachmentIdx);

		// Attach empty texture to framebuffer (DSA)
		glNamedFramebufferTexture(
			framebufferID,
			opengl::to_color_attachment(attachmentIdx),
			0,
			0);
		return;
	}

	if(!colorTexture.isColor())
	{
		PH_DEFAULT_LOG_WARNING(
			"Texture for color attachment {} is not a color format.", attachmentIdx);
	}

	if(widthPx != colorTexture.widthPx ||
	   heightPx != colorTexture.heightPx ||
	   numSamples != colorTexture.numSamples ||
	   colorAttachments[attachmentIdx].internalFormat != colorTexture.internalFormat)
	{
		PH_DEFAULT_LOG(
			"Using heterogeneous color attachment on {}.", attachmentIdx);
	}

	colorAttachments[attachmentIdx].handle = handle;

	// Attach texture to framebuffer (DSA)
	glNamedFramebufferTexture(
		framebufferID,
		opengl::to_color_attachment(attachmentIdx),
		colorTexture.textureID,
		0);

#if PH_DEBUG_OPENGL
	checkCompleteness();
#endif
}

void OpenglFramebuffer::attachDepthStencil(
	const OpenglTexture& depthStencilTexture,
	TextureHandle handle)
{
	PH_ASSERT(hasResource());

	if(depthStencilAttachment.internalFormat == GL_NONE)
	{
		PH_DEFAULT_LOG_ERROR(
			"The buffer did not declare a depth stencil attachment, ignoring.");
		return;
	}

	if(!depthStencilTexture.hasResource() || !handle)
	{
		PH_DEFAULT_LOG(
			"Nullifying depth stencil attachment.");

		// Attach empty texture to framebuffer (DSA)
		glNamedFramebufferTexture(
			framebufferID,
			depthStencilAttachment.attachmentType,
			0,
			0);
		return;
	}

	if(depthStencilTexture.isColor())
	{
		PH_DEFAULT_LOG_WARNING(
			"Texture for depth stencil attachment is a color format.");
	}

	if(widthPx != depthStencilTexture.widthPx ||
	   heightPx != depthStencilTexture.heightPx ||
	   numSamples != depthStencilTexture.numSamples ||
	   depthStencilAttachment.internalFormat != depthStencilTexture.internalFormat)
	{
		PH_DEFAULT_LOG(
			"Using heterogeneous depth stencil attachment.");
	}

	depthStencilAttachment.handle = handle;

	// Attach texture to framebuffer (DSA)
	glNamedFramebufferTexture(
		framebufferID,
		depthStencilAttachment.attachmentType,
		depthStencilTexture.textureID,
		0);

#if PH_DEBUG_OPENGL
	checkCompleteness();
#endif
}

void OpenglFramebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glViewport(0, 0, widthPx, heightPx);
}

void OpenglFramebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenglFramebuffer::clearColor(const uint32 attachmentIdx, const math::Vector4F& color)
{
	PH_ASSERT_LT(attachmentIdx, colorAttachments.size());
	PH_ASSERT(colorAttachments[attachmentIdx].isAttached());

	const std::array<GLfloat, 4> values = {
		lossless_cast<GLfloat>(color.r()),
		lossless_cast<GLfloat>(color.g()),
		lossless_cast<GLfloat>(color.b()),
		lossless_cast<GLfloat>(color.a())};

	glClearNamedFramebufferfv(
		framebufferID, 
		GL_COLOR, 
		lossless_cast<GLint>(attachmentIdx),
		values.data());
}

void OpenglFramebuffer::clearDepthStencil(const float32 depth, const uint8 stencil)
{
	PH_ASSERT(depthStencilAttachment.isAttached());

	// Reference: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glClearBuffer.xhtml

	const auto depthValue = lossless_cast<GLfloat>(depth);
	const auto stencilValue = lossless_cast<GLint>(stencil);

	if(depthStencilAttachment.attachmentType == GL_DEPTH_ATTACHMENT)
	{
		glClearNamedFramebufferfv(
			framebufferID,
			GL_DEPTH,
			0,
			&depthValue);
	}
	else if(depthStencilAttachment.attachmentType == GL_DEPTH_STENCIL_ATTACHMENT)
	{
		glClearNamedFramebufferfi(
			framebufferID,
			GL_DEPTH_STENCIL,
			0,
			depthValue,
			stencilValue);
	}
	else
	{
		PH_ASSERT_EQ(depthStencilAttachment.attachmentType, GL_STENCIL_ATTACHMENT);

		glClearNamedFramebufferiv(
			framebufferID,
			GL_STENCIL,
			0,
			&stencilValue);
	}
}

void OpenglFramebuffer::destroy()
{
	glDeleteFramebuffers(1, &framebufferID);
	framebufferID = 0;

	// Note: We are not deleting attached textures here, as those should be managed (deleted) by their
	// corresponding owner. In OpenGL, deleting a texture while a framebuffer is still in use is fine,
	// texture storage will still exist until the framebuffer is deleted. However, the texture ID may
	// be reused once it is deleted and should not be relied upon.
}

bool OpenglFramebuffer::checkCompleteness()
{
	if(glCheckNamedFramebufferStatus(framebufferID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		PH_DEFAULT_LOG_ERROR(
			"Framebuffer incomplete: {}", getFramebufferStatusInfo(framebufferID));
		return false;
	}
	else
	{
		return true;
	}
}

bool OpenglFramebuffer::hasResource() const
{
	return framebufferID != 0;
}

std::string OpenglFramebuffer::getFramebufferStatusInfo(const GLuint framebufferID)
{
	std::string messagePrefix = "ID <" + std::to_string(framebufferID) + "> status: ";

	GLenum statusCode = glCheckNamedFramebufferStatus(framebufferID, GL_FRAMEBUFFER);

	std::string message;
	switch(statusCode)
	{
	case GL_FRAMEBUFFER_COMPLETE:
		message = "Complete, no error.";
		break;

	case GL_FRAMEBUFFER_UNDEFINED:
		message = "The specified framebuffer is the default read or draw framebuffer, but "
		          "the default framebuffer does not exist.";
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		message = "One or more of the framebuffer attachment points are framebuffer incomplete.";
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		message = "The framebuffer does not have at least one image attached to it.";
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		message = "The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color "
		          "attachment point(s) named by GL_DRAW_BUFFERi.";
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		message = "GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE "
		          "is GL_NONE for the color attachment point named by GL_READ_BUFFER.";
		break;

	case GL_FRAMEBUFFER_UNSUPPORTED:
		message = "The combination of internal formats of the attached images violates an "
		          "implementation-dependent set of restrictions.";
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		message = "Two possible errors: 1. The value of GL_RENDERBUFFER_SAMPLES is not the same for "
		          "all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all "
		          "attached textures; or, if the attached images are a mix of renderbuffers and textures, "
		          "the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES. "
		          "2. The value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached "
		          "textures; or, if the attached images are a mix of renderbuffers and textures, the "
		          "value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures.";
		break;

	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		message = "One or more framebuffer attachment are layered, and one or more populated attachment "
		          "are not layered, or if all populated color attachments are not from textures of "
		          "the same target.";
		break;

	default:
		message = "Unknown error.";
		break;
	}

	return messagePrefix + message;
}

}// end namespace ph::editor::ghi
