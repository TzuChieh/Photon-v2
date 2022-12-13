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
	widthPx = safe_number_cast<GLsizei>(attachments.sizePx.x());
	heightPx = safe_number_cast<GLsizei>(attachments.sizePx.y());
	numSamples = safe_number_cast<GLsizei>(attachments.numSamples);

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

OpenglFramebuffer::OpenglFramebuffer()
	: GHIFramebuffer()
	, m_attachments()
	, m_colorTextureIDs{}
	, m_depthStencilTextureID(0)
	, m_framebufferID(0)
{
	glCreateFramebuffers(1, &m_framebufferID);
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

void OpenglFramebuffer::setAttachments(const GHIInfoFramebufferAttachment& attachments)
{
	const OpenglFramebufferAttachmentInfo newAttachments(attachments);

	// Need to update all textures if these parameters changed
	if(newAttachments.widthPx != m_attachments.widthPx ||
	   newAttachments.heightPx != m_attachments.heightPx ||
	   newAttachments.numSamples != m_attachments.numSamples)
	{
		for(uint32 attachmentIdx = 0; attachmentIdx < m_attachments.colorFormats.size(); ++attachmentIdx)
		{
			updateDeviceColorTexture(attachmentIdx, newAttachments);
		}

		updateDeviceDepthStencilTexture(newAttachments);

		// Update locally-tracked states
		m_attachments.widthPx = newAttachments.widthPx;
		m_attachments.heightPx = newAttachments.heightPx;
		m_attachments.numSamples = newAttachments.widthPx;
	}
	// Just update textures where format has changed
	else
	{
		for(uint32 attachmentIdx = 0; attachmentIdx < m_attachments.colorFormats.size(); ++attachmentIdx)
		{
			if(newAttachments.colorFormats[attachmentIdx] != m_attachments.colorFormats[attachmentIdx])
			{
				updateDeviceColorTexture(attachmentIdx, newAttachments);
			}
		}

		if(newAttachments.depthStencilFormat != m_attachments.depthStencilFormat)
		{
			updateDeviceDepthStencilTexture(newAttachments);
		}
	}
}

void OpenglFramebuffer::clearColor(const uint32 attachmentIndex, const math::Vector4F& color)
{
	PH_ASSERT(!m_attachments.colorFormats[attachmentIndex].isEmpty());

	const std::array<GLfloat, 4> values = {
		safe_number_cast<GLfloat>(color.r()),
		safe_number_cast<GLfloat>(color.g()),
		safe_number_cast<GLfloat>(color.b()), 
		safe_number_cast<GLfloat>(color.a())};

	glClearNamedFramebufferfv(
		m_framebufferID, 
		GL_COLOR, 
		safe_number_cast<GLint>(attachmentIndex),
		values.data());
}

void OpenglFramebuffer::clearDepthStencil(const float32 depth, const uint8 stencil)
{
	// Reference: https://registry.khronos.org/OpenGL-Refpages/gl4/html/glClearBuffer.xhtml

	PH_ASSERT(!m_attachments.depthStencilFormat.isEmpty());

	const auto depthValue = safe_number_cast<GLfloat>(depth);
	const auto stencilValue = safe_number_cast<GLint>(stencil);

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

	return std::make_shared<OpenglFramebufferBackedTexture2D>(
		getSharedPtrFromThis(),
		m_colorTextureIDs[attachmentIndex],
		attachmentIndex,
		false);
}

void OpenglFramebuffer::updateDeviceColorTexture(const uint32 attachmentIndex, const OpenglFramebufferAttachmentInfo& newAttachment)
{
	const OpenglFramebufferFormat& newColorFormat = newAttachment.colorFormats[attachmentIndex];
	OpenglFramebufferFormat& oldColorFormat = m_attachments.colorFormats[attachmentIndex];

	PH_ASSERT(opengl::is_color_format(newColorFormat.internalFormat));

	// Possibly detach and delete the texture
	if(newColorFormat.isEmpty() && !oldColorFormat.isEmpty())
	{
		PH_ASSERT_NE(m_colorTextureIDs[attachmentIndex], 0);

		// Detach texture from framebuffer
		glNamedFramebufferTexture(m_framebufferID, opengl::to_color_attachment(attachmentIndex), 0, 0);

		// Delete texture
		glDeleteTextures(1, &m_colorTextureIDs[attachmentIndex]);
		m_colorTextureIDs[attachmentIndex] = 0;
	}

	// Possibly create new texture and attach it
	if(!newColorFormat.isEmpty() && oldColorFormat.isEmpty())
	{
		PH_ASSERT_EQ(m_colorTextureIDs[attachmentIndex], 0);

		// Create texture
		glCreateTextures(GL_TEXTURE_2D, 1, &m_colorTextureIDs[attachmentIndex]);

		// Attach texture to framebuffer
		glNamedFramebufferTexture(
			m_framebufferID, 
			opengl::to_color_attachment(attachmentIndex),
			m_colorTextureIDs[attachmentIndex],
			0);
	}

	// Update device texture parameters if available and attached
	if(m_colorTextureIDs[attachmentIndex] != 0)
	{
		const GLuint textureID = m_colorTextureIDs[attachmentIndex];

		// Ordinary texture
		if(newAttachment.numSamples == 1)
		{
			// Need bind since we are using non-DSA calls here (for mutable textures)
			glBindTexture(GL_TEXTURE_2D, textureID);

			glTexImage2D(
				GL_TEXTURE_2D,
				0, 
				newColorFormat.internalFormat, 
				newAttachment.widthPx,
				newAttachment.heightPx,
				0,
				GL_RED,  //
				GL_FLOAT,// Just some dummy values, we are not specifying any input data
				nullptr);//

			glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, newColorFormat.sampleState.filterType);
			glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, newColorFormat.sampleState.filterType);

			glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, newColorFormat.sampleState.wrapType);
			glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, newColorFormat.sampleState.wrapType);
		}
		// Multi-sampled texture
		else
		{
			PH_ASSERT_GT(newAttachment.numSamples, 1);

			// Need bind since we are using non-DSA calls here (for mutable textures)
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);

			glTexImage2DMultisample(
				GL_TEXTURE_2D_MULTISAMPLE, 
				newAttachment.numSamples, 
				newColorFormat.internalFormat, 
				newAttachment.widthPx, 
				newAttachment.heightPx,
				GL_FALSE);
		}
	}

	// Finally update locally-tracked states
	oldColorFormat = newColorFormat;
}

void OpenglFramebuffer::updateDeviceDepthStencilTexture(const OpenglFramebufferAttachmentInfo& newAttachment)
{
	const OpenglFramebufferFormat& newFormat = newAttachment.depthStencilFormat;
	OpenglFramebufferFormat& oldFormat = m_attachments.depthStencilFormat;

	PH_ASSERT(!opengl::is_color_format(newFormat.internalFormat));

	// Possibly detach and delete the texture
	if(newFormat.isEmpty() && !oldFormat.isEmpty())
	{
		PH_ASSERT_NE(m_depthStencilTextureID, 0);

		// Detach texture from framebuffer
		glNamedFramebufferTexture(
			m_framebufferID, 
			m_attachments.depthStencilAttachment,// note: use old attachment info
			0, 
			0);

		// Delete texture
		glDeleteTextures(1, &m_depthStencilTextureID);
		m_depthStencilTextureID = 0;
	}

	// Possibly create new texture and attach it
	if(!newFormat.isEmpty() && oldFormat.isEmpty())
	{
		PH_ASSERT_EQ(m_depthStencilTextureID, 0);

		// Create texture
		glCreateTextures(GL_TEXTURE_2D, 1, &m_depthStencilTextureID);

		// Attach texture to framebuffer
		glNamedFramebufferTexture(
			m_framebufferID, 
			newAttachment.depthStencilAttachment,
			m_depthStencilTextureID,
			0);
	}

	// Update device texture parameters if available and attached
	if(m_depthStencilTextureID != 0)
	{
		const GLuint textureID = m_depthStencilTextureID;

		// Ordinary texture
		if(newAttachment.numSamples == 1)
		{
			// Need bind since we are using non-DSA calls here (for mutable textures)
			glBindTexture(GL_TEXTURE_2D, textureID);

			glTexImage2D(
				GL_TEXTURE_2D,
				0, 
				newFormat.internalFormat, 
				newAttachment.widthPx,
				newAttachment.heightPx,
				0,
				GL_RED,  //
				GL_FLOAT,// Just some dummy values, we are not specifying any input data
				nullptr);//

			glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, newFormat.sampleState.filterType);
			glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, newFormat.sampleState.filterType);

			glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, newFormat.sampleState.wrapType);
			glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, newFormat.sampleState.wrapType);
		}
		// Multi-sampled texture
		else
		{
			PH_ASSERT_GT(newAttachment.numSamples, 1);

			// Need bind since we are using non-DSA calls here (for mutable textures)
			glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureID);

			glTexImage2DMultisample(
				GL_TEXTURE_2D_MULTISAMPLE, 
				newAttachment.numSamples, 
				newFormat.internalFormat,
				newAttachment.widthPx, 
				newAttachment.heightPx,
				GL_FALSE);
		}
	}

	// Finally update locally-tracked states
	oldFormat = newFormat;
}

}// end namespace ph::editor
