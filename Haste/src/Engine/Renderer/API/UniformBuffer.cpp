#include "UniformBuffer.h"

#include "RendererAPI.h"
#include "OpenGL/OpenGLUniformBuffer.h"

namespace Core {

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:    CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLUniformBuffer>(size, binding);
		}

		CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
