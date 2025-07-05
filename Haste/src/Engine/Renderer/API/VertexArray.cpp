#include "VertexArray.h"
#include "RendererAPI.h"
#include "OpenGL/OpenGLVertexArray.h"

namespace Core {

	Ref<VertexArray> VertexArray::Create()
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None:    CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLVertexArray>();
		}

		CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
