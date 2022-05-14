#ifndef JSE_KEYBOARD_SDL_H
#define JSE_KEYBOARD_SDL_H

#include "input/Keyboard.hpp"
#include "input/InputDriver.hpp"

namespace jse {

	class KeyboardSDL : public Keyboard
	{
		KeyboardSDL(InputDriver* aInputDriver);
		void Update();

		
	};
}

#endif