#include "RenderCore/OpenGL/OpenglFramebuffer.h"
#include "RenderCore/OpenGL/OpenglFramebufferBackedTexture2D.h"

#include <Utility/utility.h>
#include <Common/assertion.h>

namespace ph::editor
{

OpenglFramebufferFormat::OpenglFramebufferFormat()
	: internalFormat(0)
	, sampleState()
{}

OpenglFramebufferFormat::OpenglFramebufferFormat(const GHIInfoFramebufferFormat& format)
	: OpenglFramebufferFormat()
{
	internalFormat = opengl::to_internal_format(format.pixelFormat);
	sampleState = OpenglSampleState(format.sampleState);
}

OpenglFramebufferAttachmentInfo::OpenglFramebufferAttachmentInfo()
	: widthPx(0)
	, heightPx(0)
	, numSamples(1)
	, colorFormats()
	, depthStencilFormat()
	, depthStencilAttachment(0)
{}

OpenglFramebufferAttachmentInfo::OpenglFramebufferAttachmentInfo(const GHIInfoFramebufferAttachment& attachments)
	: OpenglFramebufferAttachmentInfo()
{
	widthPx = lossless_cast<GLsizei>(attachments.sizePx.x());
	heightPx = lossless_cast<GLsizei>(attachments.sizePx.y());
	numSamples = lossless_cast<GLsizei>(attachments.numSamples);

	for(std::size_t i = 0; i < attachments.colorFormats.size(); ++i)
	{
		colorFormats[i] = OpenglFramebufferFormat(attachments.colorFormats[i]);
	}

	depthStencilFormat = OpenglFramebufferFormat(attachments.depthStencilFormat);

	if(!depthStencilFormat.isEmpty())
	{
		switch(depthStencilFormat.internalFormat)
		{
		case GL_DEPTH_COMPONENT32F:
		case GL_DEPTH_COMPONENT24:
		case GL_DEPTH_COMPONENT16:
			depthStencilAttachment = GL_DEPTH_ATTACHMENT;
			break;

		case GL_DEPTH24_STENCIL8:
		case GL_DEPTH32F_STENCIL8:
			depthStencilAttachment = GL_DEPTH_STENCIL_ATTACHMENT;
			break;

		// Other stencil bitdepths are strongly unrecommended, see
		// https://www.khronos.org/opengl/wiki/Image_Format#Stencil_only
		case GL_STENCIL_INDEX8:
			depthStencilAttachment = GL_STENCIL_ATTACHMENT;
			break;

		default:
			PH_ASSERT_UNREACHABLE_SECTION();
			break;
		}
	}
}

OpenglFramebuffer::OpenglFramebuffer(const GHIInfoFramebufferAttachment& attachments)

	: GHIFramebuffer(attachments)

	, m_attachments(attachments)
	, m_colorTextureIDs{}
	, m_depthStencilTextureID(0)
	, m_framebufferID(0)
{
	glCreateFramebuffers(1, &m_framebufferID);

	for(uint32 attachmentIdx = 0; attachmentIdx < m_attachments.colorFormats.size(); ++attachmentIdx)
	{
		createDeviceColorTexture(attachmentIdx);
	}

	createDeviceDepthStencilTexture();
}

OpenglFramebuffer::~OpenglFramebuffer()
{
	for(const GLuint colorTextureID : m_colorTextureIDs)
	{
		glDeleteTextures(1, &colorTextureID);
	}

	glDeleteTextures(1, &m_depthStencilTextureID);

	glDeleteFramebuffers(1, &m_framebufferID);
}

void OpenglFramebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);
	glViewport(0, 0, m_attachments.widthPx, m_attachments.heightPx);
}

void OpenglFramebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenglFramebuffer::clearColor(const uint32 attachmentIndex, const math::Vector4F& color)
{
	PH_ASSERT(!m_attachments.colorFormats[attachmentIndex].isEmpty());

	const std::array<GLfloat, 4> values = {
		lossless_cast<GLfloat>(color.r()),
		lossless_cast<GLfloat>(color.g()),
		lossless_cast<GLfloat>(color.b()),
		lossless_cast<GLfloat>(color.a())};

	glClearNamedFramebufferfv(
		m_framebufferID, 
		GL_COLOR, 
		lossless_cast<GLint>(attachmentIndex),
		values.data());
}

void OpenglFramebuffer::clearDepthStencil(const float32 depth, const uint8 stencil)
{
	// Reference: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glClearBuffer.xhtml

	PH_ASSERT(!m_attachments.depthStencilFormat.isEmpty());

	const auto depthValue = lossless_cast<GLfloat>(depth);
	const auto stencilValue = lossless_cast<GLint>(stencil);

	if(m_attachments.depthStencilAttachment == GL_DEPTH_ATTACHMENT)
	{
		glClearNamedFramebufferfv(
			m_framebufferID,
			GL_DEPTH,
			0,
			&depthValue);
	}
	else if(m_attachments.depthStencilAttachment == GL_DEPTH_STENCIL_ATTACHMENT)
	{
		glClearNamedFramebufferfi(
			m_framebufferID,
			GL_DEPTH_STENCIL,
			0,
			depthValue,
			stencilValue);
	}
	else
	{
		PH_ASSERT_EQ(m_attachments.depthStencilAttachment, GL_STENCIL_ATTACHMENT);

		glClearNamedFramebufferiv(
			m_framebufferID,
			GL_STENCIL,
			0,
			&stencilValue);
	}
}

std::shared_ptr<GHITexture2D> OpenglFramebuffer::createTextureFromColor(const uint32 attachmentIndex)
{
	PH_ASSERT_LT(attachmentIndex, m_colorTextureIDs.size());

	// The texture holds a reference to `this`--this will ensure that the OpenGL texture will not 
	// be deleted until the texture is released. Relied on `this` being RAII.
	return std::make_shared<OpenglFramebufferBackedTexture2D>(
		std::static_pointer_cast<OpenglFramebuffer>(getSharedPtrFromThis()),
		m_colorTextureIDs[attachmentIndex],
		attachmentIndex,
		false);
}

void OpenglFramebuffer::createDeviceColorTexture(const uint32 attachmentIndex)
{
	// Must have valid framebuffer
	PH_ASSERT_NE(m_framebufferID, 0);

	const OpenglFramebufferFormat& format = m_attachments.colorFormats[attachmentIndex];
	if(format.isEmpty())
	{
		// No need to create texture for an empty attachment
		return;
	}
	PH_ASSERT(opengl::is_color_format(format.internalFormat));

	// Create new texture and attach it
	
	// Must not already created
	PH_ASSERT_EQ(m_colorTextureIDs[attachmentIndex], 0);

	// Create texture
	GLuint& textureID = m_colorTextureIDs[attachmentIndex];
	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

	// Update texture parameters
	
	PH_ASSERT_NE(textureID, 0);

	// Ordinary texture
	if(m_attachments.numSamples == 1)
	{
		// Need bind since we are using non-DSA calls here (for mutable textures)
		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexImage2D(
			GL_TEXTURE_2D,
			0, 
			format.internalFormat,
			m_attachments.widthPx,
			m_attachments.heightPx,
			0,
			GL_RED,  //
			GL_FLOAT,// Just some dummy values, we are not specifying any input data
			nullptr);//

		glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, format.sampleState.filterType);
		glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, format.sampleState.filterType);

		glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, format.sampleState.wrapType);
		glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, format.sampleState.wrapType);
	}
	// Multi-sampled texture
	else
	{
		PH_ASSERT_GT(m_attachments.numSamples, 1);

		// Need bind since we are using non-DSA calls here (for mutable textures)
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);

		glTexImage2DMultisample(
			GL_TEXTURE_2D_MULTISAMPLE, 
			m_attachments.numSamples,
			format.internalFormat,
			m_attachments.widthPx,
			m_attachments.heightPx,
			GL_FALSE);
	}

	// Attach texture to framebuffer (DSA)
	glNamedFramebufferTexture(
		m_framebufferID,
		opengl::to_color_attachment(attachmentIndex),
		textureID,
		0);
}

void OpenglFramebuffer::createDeviceDepthStencilTexture()
{
	// Must have valid framebuffer
	PH_ASSERT_NE(m_framebufferID, 0);

	const OpenglFramebufferFormat& format = m_attachments.depthStencilFormat;
	if(format.isEmpty())
	{
		// No need to create texture for an empty attachment
		return;
	}
	PH_ASSERT(!opengl::is_color_format(format.internalFormat));

	// Create new texture and attach it

	// Must not already created
	PH_ASSERT_EQ(m_depthStencilTextureID, 0);

	// Create texture
	glCreateTextures(GL_TEXTURE_2D, 1, &m_depthStencilTextureID);

	// Update texture parameters

	PH_ASSERT_NE(m_depthStencilTextureID, 0);

	// Ordinary texture
	if(m_attachments.numSamples == 1)
	{
		// Need bind since we are using non-DSA calls here (for mutable textures)
		glBindTexture(GL_TEXTURE_2D, m_depthStencilTextureID);

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			format.internalFormat,
			m_attachments.widthPx,
			m_attachments.heightPx,
			0,
			GL_RED,  //
			GL_FLOAT,// Just some dummy values, we are not specifying any input data
			nullptr);//

		glTextureParameteri(m_depthStencilTextureID, GL_TEXTURE_MIN_FILTER, format.sampleState.filterType);
		glTextureParameteri(m_depthStencilTextureID, GL_TEXTURE_MAG_FILTER, format.sampleState.filterType);

		glTextureParameteri(m_depthStencilTextureID, GL_TEXTURE_WRAP_S, format.sampleState.wrapType);
		glTextureParameteri(m_depthStencilTextureID, GL_TEXTURE_WRAP_T, format.sampleState.wrapType);
	}
	// Multi-sampled texture
	else
	{
		PH_ASSERT_GT(m_attachments.numSamples, 1);

		// Need bind since we are using non-DSA calls here (for mutable textures)
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_depthStencilTextureID);

		glTexImage2DMultisample(
			GL_TEXTURE_2D_MULTISAMPLE,
			m_attachments.numSamples,
			format.internalFormat,
			m_attachments.widthPx,
			m_attachments.heightPx,
			GL_FALSE);
	}

	// Attach texture to framebuffer (DSA)
	glNamedFramebufferTexture(
		m_framebufferID,
		m_attachments.depthStencilAttachment,
		m_depthStencilTextureID,
		0);
}

}// end namespace ph::editor
