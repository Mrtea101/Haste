#pragma once

#include "KeyCodes.h"
#include "MouseCodes.h"

#include <glm/glm.hpp>

namespace Core {

	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static glm::vec2 GetMousePosition();
		static void SetMousePosition(glm::vec2 pos);
		static float GetMouseX();
		static float GetMouseY();

		static bool GetMouseLocked();
		static void SetMouseLocked(bool value);
	};
}
