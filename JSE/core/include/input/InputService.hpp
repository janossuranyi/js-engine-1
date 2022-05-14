#ifndef JSE_INPUT_SERVICE_H
#define JSE_INPUT_SERVICE_H

#include "input/InputTypes.hpp"
#include "graphics/GraphicsTypes.hpp"

namespace jse {


	class InputService
	{
	public:
		virtual ~InputService() {};

		virtual void Update() = 0;
		virtual bool IsKeyDown(const Key aKey) const = 0;

		virtual bool IsButtonDown(const MouseButton aButton) const = 0;
		virtual KeyPress GetKeyPressed() = 0;
		virtual KeyPress GetKeyReleased() = 0;
		virtual bool IsKeyPressed() = 0;
		virtual bool IsKeyReleased() = 0;

		virtual Vector2l GetMousePosition() const = 0;

		virtual void SetRelativeMouseMode(const bool a0) = 0;
		virtual Vector2l GetRelativeMousePosition() const = 0;

		virtual bool IsWheelUpMoved() const = 0;
		virtual bool IsWheelDownMoved() const = 0;

		bool IsMouseInputHandled() const { return mIsMouseInputHandled; }
		void SetMouseInputHandled(const bool a0) { mIsMouseInputHandled = a0; }
		bool IsKeyboardInputHandled() const { return mIsKeyboardInputHandled; }
		void SetKeyboardInputHandled(const bool a0) { mIsKeyboardInputHandled = a0; }

	private:
		bool mIsMouseInputHandled;
		bool mIsKeyboardInputHandled;
	};

}
#endif
