#include "Framebuffer.h"
#include "RendererAPI.h"
#include "OpenGL/OpenGLFramebuffer.h"

namespace Core {
	
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:    CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFramebuffer>(spec);
		}

		CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}

