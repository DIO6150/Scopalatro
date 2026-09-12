
#include <core/Logger.hpp>

#include <opengl/FBO.hpp>

#include <glad/glad.h>

using namespace TLOT;

// TODO-fix : get max color attachment and limit it
FBO::FBO (int32_t width, int32_t height):
	mWidth  {width},
	mHeight {height}	
{
	mColorAttachments.resize (1);

	glGenFramebuffers (1, &mFrameBuffer);
	glBindFramebuffer (GL_FRAMEBUFFER, mFrameBuffer);

	glGenTextures (mColorAttachments.size (), mColorAttachments.data ());
	int idx = 0;
	for (auto & attachement : mColorAttachments) {
		glBindTexture (GL_TEXTURE_2D, attachement);
		glTexImage2D  (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture (GL_TEXTURE_2D, 0);	
		glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + idx++, GL_TEXTURE_2D, attachement, 0);
	}
	
	glGenRenderbuffers (1, &mRenderBuffer);
	glBindRenderbuffer (GL_RENDERBUFFER, mRenderBuffer);
	glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer (GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderBuffer);

	if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		Logger::log (LogLevel::Error, "FrameBuffer is not complete");
	}
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
}

FBO::~FBO ()
{
	glDeleteFramebuffers	(1, &mFrameBuffer);
	glDeleteRenderbuffers	(1, &mRenderBuffer);
	glDeleteTextures	(mColorAttachments.size (), mColorAttachments.data ());
}

void FBO::AddColorAttachment ()
{
	// TODO: should return binding point
	mColorAttachments.emplace_back (0);

	glGenTextures   (1, &mColorAttachments.back ());
	glBindTexture   (GL_TEXTURE_2D, mColorAttachments.back ());
	glTexImage2D    (GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture   (GL_TEXTURE_2D, 0);	
	glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + mColorAttachments.size (), GL_TEXTURE_2D, mColorAttachments.back (), 0);
}

uint32_t FBO::Get () const
{
	return (mFrameBuffer);
}

uint32_t FBO::GetColorAttachment (uint32_t binding) const
{
	return (mColorAttachments.at (binding));
}