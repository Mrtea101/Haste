#pragma once

#include <Core/Core.h>
#include <Core/UI/Layer.h>

class TestLayer : public Layer
{
public:
	TestLayer();
	~TestLayer() = default;

	void OnAttach() override;
	void OnDetach() override;

	void OnUIRender() override;

	uint32_t m_DockspaceID = 0;
};
