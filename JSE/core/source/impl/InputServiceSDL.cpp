#include <SDL.h>
#include <mutex>
#include "impl/InputServiceSDL.hpp"
#include "input/InputTypes.hpp"
#include "system/SystemTypes.hpp"
#include "system/Logger.hpp"


namespace jse {

	InputServiceSDL::InputServiceSDL()
	{
		mQuitMessagePosted = false;
		mKeyState.resize(Key_LastEnum);
		mMouseState.resize(MB_LastEnum);
		mMousePos = Vector2l(0, 0);
		mWheelDownMoved = false;
		mWheelUpMoved = false;

		mMouseState.assign(MB_LastEnum, false);
		mKeyState.assign(mKeyState.size(), false);
	}

	InputServiceSDL::~InputServiceSDL()
	{
	}

	bool InputServiceSDL::IsKeyDown(const Key aKey)
	{
		return mKeyState[aKey];
	}

	KeyPress InputServiceSDL::GetKeyPressed()
	{
		KeyPress res = mKeysPressed.front();
		mKeysPressed.pop_front();

		return res;
	}

	KeyPress InputServiceSDL::GetKeyReleased()
	{
		KeyPress res = mKeysReleased.front();
		mKeysReleased.pop_front();

		return res;
	}

	bool InputServiceSDL::IsKeyPressed()
	{
		return !mKeysPressed.empty();
	}

	bool InputServiceSDL::IsKeyReleased()
	{
		return !mKeysReleased.empty();
	}

	Vector2l InputServiceSDL::GetMousePosition() const
	{
		return mMousePos;
	}

	void InputServiceSDL::SetRelativeMouseMode(const bool a0)
	{
		SDL_SetRelativeMouseMode(a0 ? SDL_TRUE : SDL_FALSE);
	}

	Vector2l InputServiceSDL::GetRelativeMousePosition() const
	{
		int xpos, ypos;
		SDL_GetRelativeMouseState(&xpos, &ypos);
		
		return Vector2l(xpos, ypos);
	}

	bool InputServiceSDL::IsButtonDown(const MouseButton aButton) const
	{
		return mMouseState[aButton];
	}

	void InputServiceSDL::Update()
	{
		SDL_Event e;

		mEvents.clear();
		SetKeyboardInputHandled(false);
		SetMouseInputHandled(false);

		while (SDL_PollEvent(&e) != SDL_FALSE)
		{
			if (e.type == SDL_QUIT)
			{
				mQuitMessagePosted = true;
			}
			else if (e.type == SDL_WINDOWEVENT)
			{
				switch (e.window.event)
				{
				case SDL_WINDOWEVENT_RESIZED:
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					Info("Window %d resized to %dx%d",
						e.window.windowID, e.window.data1,
						e.window.data2);

					mEvents.push_back(e);
					break;
				}
			}
			else
			{
				mEvents.push_back(e);
			}
		}

		UpdateKeyboard();
		UpdateMouse();
	}

	Key InputServiceSDL::SDLToKey(const int alKey)
	{
		switch (alKey)
		{
		case	SDLK_BACKSPACE: return Key_BackSpace;
		case	SDLK_TAB:return  Key_Tab;
		case 	SDLK_CLEAR: return Key_Clear;
		case 	SDLK_RETURN: return Key_Return;
		case 	SDLK_PAUSE: return Key_Pause;
		case 	SDLK_ESCAPE: return Key_Escape;
		case 	SDLK_SPACE: return Key_Space;
		case 	SDLK_EXCLAIM: return Key_Exclaim;
		case 	SDLK_QUOTEDBL: return Key_QuoteDouble;
		case 	SDLK_HASH: return Key_Hash;
		case 	SDLK_DOLLAR: return Key_Dollar;
		case 	SDLK_AMPERSAND: return Key_Ampersand;
		case 	SDLK_QUOTE: return Key_Quote;
		case 	SDLK_LEFTPAREN: return Key_LeftParen;
		case 	SDLK_RIGHTPAREN: return Key_RightParen;
		case 	SDLK_ASTERISK: return Key_Asterisk;
		case 	SDLK_PLUS: return Key_Plus;
		case 	SDLK_COMMA: return Key_Comma;
		case 	SDLK_MINUS: return Key_Minus;
		case 	SDLK_PERIOD: return Key_Period;
		case 	SDLK_SLASH: return Key_Slash;
		case 	SDLK_0: return Key_0;
		case 	SDLK_1: return Key_1;
		case 	SDLK_2: return Key_2;
		case 	SDLK_3: return Key_3;
		case 	SDLK_4: return Key_4;
		case 	SDLK_5: return Key_5;
		case 	SDLK_6: return Key_6;
		case 	SDLK_7: return Key_7;
		case 	SDLK_8: return Key_8;
		case 	SDLK_9: return Key_9;
		case 	SDLK_COLON: return Key_Colon;
		case 	SDLK_SEMICOLON: return Key_SemiColon;
		case 	SDLK_LESS: return Key_Less;
		case 	SDLK_EQUALS: return Key_Equals;
		case 	SDLK_GREATER: return Key_Greater;
		case 	SDLK_QUESTION: return Key_Question;
		case 	SDLK_AT: return Key_At;
		case 	SDLK_LEFTBRACKET: return Key_LeftBracket;
		case 	SDLK_BACKSLASH: return Key_BackSlash;
		case 	SDLK_RIGHTBRACKET: return Key_RightBracket;
		case 	SDLK_CARET: return Key_Caret;
		case 	SDLK_UNDERSCORE: return Key_Underscore;
		case 	SDLK_BACKQUOTE: return Key_BackSlash;
		case 	SDLK_a: return Key_A;
		case 	SDLK_b: return Key_B;
		case 	SDLK_c: return Key_C;
		case 	SDLK_d: return Key_D;
		case 	SDLK_e: return Key_E;
		case 	SDLK_f: return Key_F;
		case 	SDLK_g: return Key_G;
		case 	SDLK_h: return Key_H;
		case 	SDLK_i: return Key_I;
		case 	SDLK_j: return Key_J;
		case 	SDLK_k: return Key_K;
		case 	SDLK_l: return Key_L;
		case 	SDLK_m: return Key_M;
		case 	SDLK_n: return Key_N;
		case 	SDLK_o: return Key_O;
		case 	SDLK_p: return Key_P;
		case 	SDLK_q: return Key_Q;
		case 	SDLK_r: return Key_R;
		case 	SDLK_s: return Key_S;
		case 	SDLK_t: return Key_T;
		case 	SDLK_u: return Key_U;
		case 	SDLK_v: return Key_V;
		case 	SDLK_w: return Key_W;
		case 	SDLK_x: return Key_X;
		case 	SDLK_y: return Key_Y;
		case 	SDLK_z: return Key_Z;
		case 	SDLK_DELETE: return Key_Delete;
#if SDL_VERSION_ATLEAST(2, 0, 0)
		case 	SDLK_KP_0: return Key_KP_0;
		case 	SDLK_KP_1: return Key_KP_1;
		case 	SDLK_KP_2: return Key_KP_2;
		case 	SDLK_KP_3: return Key_KP_3;
		case 	SDLK_KP_4: return Key_KP_4;
		case 	SDLK_KP_5: return Key_KP_5;
		case 	SDLK_KP_6: return Key_KP_6;
		case 	SDLK_KP_7: return Key_KP_7;
		case 	SDLK_KP_8: return Key_KP_8;
		case 	SDLK_KP_9: return Key_KP_9;
#else
		case 	SDLK_KP0: return Key_KP_0;
		case 	SDLK_KP1: return Key_KP_1;
		case 	SDLK_KP2: return Key_KP_2;
		case 	SDLK_KP3: return Key_KP_3;
		case 	SDLK_KP4: return Key_KP_4;
		case 	SDLK_KP5: return Key_KP_5;
		case 	SDLK_KP6: return Key_KP_6;
		case 	SDLK_KP7: return Key_KP_7;
		case 	SDLK_KP8: return Key_KP_8;
		case 	SDLK_KP9: return Key_KP_9;
#endif
		case 	SDLK_KP_PERIOD: return Key_KP_Period;
		case 	SDLK_KP_DIVIDE: return Key_KP_Divide;
		case 	SDLK_KP_MULTIPLY: return Key_KP_Multiply;
		case 	SDLK_KP_MINUS: return Key_KP_Minus;
		case 	SDLK_KP_PLUS: return Key_KP_Plus;
		case 	SDLK_KP_ENTER: return Key_KP_Enter;
		case 	SDLK_KP_EQUALS: return Key_KP_Equals;
		case 	SDLK_UP: return Key_Up;
		case 	SDLK_DOWN: return Key_Down;
		case 	SDLK_RIGHT: return Key_Right;
		case 	SDLK_LEFT: return Key_Left;
		case 	SDLK_INSERT: return Key_Insert;
		case 	SDLK_HOME: return Key_Home;
		case 	SDLK_END: return Key_End;
		case 	SDLK_PAGEUP: return Key_PageUp;
		case 	SDLK_PAGEDOWN: return Key_PageDown;
		case 	SDLK_F1: return Key_F1;
		case 	SDLK_F2: return Key_F2;
		case 	SDLK_F3: return Key_F3;
		case 	SDLK_F4: return Key_F4;
		case 	SDLK_F5: return Key_F5;
		case 	SDLK_F6: return Key_F6;
		case 	SDLK_F7: return Key_F7;
		case 	SDLK_F8: return Key_F8;
		case 	SDLK_F9: return Key_F9;
		case 	SDLK_F10: return Key_F10;
		case 	SDLK_F11: return Key_F11;
		case 	SDLK_F12: return Key_F12;
		case 	SDLK_F13: return Key_F13;
		case 	SDLK_F14: return Key_F14;
		case 	SDLK_F15: return Key_F15;
#if SDL_VERSION_ATLEAST(2, 0, 0)
		case    SDLK_NUMLOCKCLEAR: return Key_NumLock;
		case 	SDLK_SCROLLLOCK: return Key_ScrollLock;
		case 	SDLK_LGUI: return Key_LeftSuper;
		case 	SDLK_RGUI: return Key_RightSuper;
		case    SDLK_PRINTSCREEN: return Key_Print;
#else
		case 	SDLK_NUMLOCK: return Key_NumLock;
		case 	SDLK_SCROLLOCK: return Key_ScrollLock;
		case 	SDLK_LSUPER: return Key_LeftSuper;
		case 	SDLK_RSUPER: return Key_RightSuper;
		case 	SDLK_RMETA: return Key_RightMeta;
		case 	SDLK_LMETA: return Key_LeftMeta;
		case 	SDLK_PRINT: return Key_Print;
		case 	SDLK_BREAK: return Key_Break;
#endif
		case 	SDLK_CAPSLOCK: return Key_CapsLock;
		case 	SDLK_RSHIFT: return Key_RightShift;
		case 	SDLK_LSHIFT: return Key_LeftShift;
		case 	SDLK_RCTRL: return Key_RightCtrl;
		case 	SDLK_LCTRL: return Key_LeftCtrl;
		case 	SDLK_RALT: return Key_RightAlt;
		case 	SDLK_LALT: return Key_LeftAlt;
		case 	SDLK_MODE: return Key_Mode;
		case 	SDLK_HELP: return Key_Help;
		case 	SDLK_SYSREQ: return Key_SysReq;
		case 	SDLK_MENU: return Key_Menu;
		case 	SDLK_POWER: return Key_Power;
#if !SDL_VERSION_ATLEAST(2, 0, 0)
		case 	SDLK_EURO: return Key_Euro;
		case 	SDLK_WORLD_0: return Key_World_0;
		case 	SDLK_WORLD_1: return Key_World_1;
		case 	SDLK_WORLD_2: return Key_World_2;
		case 	SDLK_WORLD_3: return Key_World_3;
		case 	SDLK_WORLD_4: return Key_World_4;
		case 	SDLK_WORLD_5: return Key_World_5;
		case 	SDLK_WORLD_6: return Key_World_6;
		case 	SDLK_WORLD_7: return Key_World_7;
		case 	SDLK_WORLD_8: return Key_World_8;
		case 	SDLK_WORLD_9: return Key_World_9;
		case 	SDLK_WORLD_10: return Key_World_10;
		case 	SDLK_WORLD_11: return Key_World_11;
		case 	SDLK_WORLD_12: return Key_World_12;
		case 	SDLK_WORLD_13: return Key_World_13;
		case 	SDLK_WORLD_14: return Key_World_14;
		case 	SDLK_WORLD_15: return Key_World_15;
		case 	SDLK_WORLD_16: return Key_World_16;
		case 	SDLK_WORLD_17: return Key_World_17;
		case 	SDLK_WORLD_18: return Key_World_18;
		case 	SDLK_WORLD_19: return Key_World_19;
		case 	SDLK_WORLD_20: return Key_World_20;
		case 	SDLK_WORLD_21: return Key_World_21;
		case 	SDLK_WORLD_22: return Key_World_22;
		case 	SDLK_WORLD_23: return Key_World_23;
		case 	SDLK_WORLD_24: return Key_World_24;
		case 	SDLK_WORLD_25: return Key_World_25;
		case 	SDLK_WORLD_26: return Key_World_26;
		case 	SDLK_WORLD_27: return Key_World_27;
		case 	SDLK_WORLD_28: return Key_World_28;
		case 	SDLK_WORLD_29: return Key_World_29;
		case 	SDLK_WORLD_30: return Key_World_30;
		case 	SDLK_WORLD_31: return Key_World_31;
		case 	SDLK_WORLD_32: return Key_World_32;
		case 	SDLK_WORLD_33: return Key_World_33;
		case 	SDLK_WORLD_34: return Key_World_34;
		case 	SDLK_WORLD_35: return Key_World_35;
		case 	SDLK_WORLD_36: return Key_World_36;
		case 	SDLK_WORLD_37: return Key_World_37;
		case 	SDLK_WORLD_38: return Key_World_38;
		case 	SDLK_WORLD_39: return Key_World_39;
		case 	SDLK_WORLD_40: return Key_World_40;
		case 	SDLK_WORLD_41: return Key_World_41;
		case 	SDLK_WORLD_42: return Key_World_42;
		case 	SDLK_WORLD_43: return Key_World_43;
		case 	SDLK_WORLD_44: return Key_World_44;
		case 	SDLK_WORLD_45: return Key_World_45;
		case 	SDLK_WORLD_46: return Key_World_46;
		case 	SDLK_WORLD_47: return Key_World_47;
		case 	SDLK_WORLD_48: return Key_World_48;
		case 	SDLK_WORLD_49: return Key_World_49;
		case 	SDLK_WORLD_50: return Key_World_50;
		case 	SDLK_WORLD_51: return Key_World_51;
		case 	SDLK_WORLD_52: return Key_World_52;
		case 	SDLK_WORLD_53: return Key_World_53;
		case 	SDLK_WORLD_54: return Key_World_54;
		case 	SDLK_WORLD_55: return Key_World_55;
		case 	SDLK_WORLD_56: return Key_World_56;
		case 	SDLK_WORLD_57: return Key_World_57;
		case 	SDLK_WORLD_58: return Key_World_58;
		case 	SDLK_WORLD_59: return Key_World_59;
		case 	SDLK_WORLD_60: return Key_World_60;
		case 	SDLK_WORLD_61: return Key_World_61;
		case 	SDLK_WORLD_62: return Key_World_62;
		case 	SDLK_WORLD_63: return Key_World_63;
		case 	SDLK_WORLD_64: return Key_World_64;
		case 	SDLK_WORLD_65: return Key_World_65;
		case 	SDLK_WORLD_66: return Key_World_66;
		case 	SDLK_WORLD_67: return Key_World_67;
		case 	SDLK_WORLD_68: return Key_World_68;
		case 	SDLK_WORLD_69: return Key_World_69;
		case 	SDLK_WORLD_70: return Key_World_70;
		case 	SDLK_WORLD_71: return Key_World_71;
		case 	SDLK_WORLD_72: return Key_World_72;
		case 	SDLK_WORLD_73: return Key_World_73;
		case 	SDLK_WORLD_74: return Key_World_74;
		case 	SDLK_WORLD_75: return Key_World_75;
		case 	SDLK_WORLD_76: return Key_World_76;
		case 	SDLK_WORLD_77: return Key_World_77;
		case 	SDLK_WORLD_78: return Key_World_78;
		case 	SDLK_WORLD_79: return Key_World_79;
		case 	SDLK_WORLD_80: return Key_World_80;
		case 	SDLK_WORLD_81: return Key_World_81;
		case 	SDLK_WORLD_82: return Key_World_82;
		case 	SDLK_WORLD_83: return Key_World_83;
		case 	SDLK_WORLD_84: return Key_World_84;
		case 	SDLK_WORLD_85: return Key_World_85;
		case 	SDLK_WORLD_86: return Key_World_86;
		case 	SDLK_WORLD_87: return Key_World_87;
		case 	SDLK_WORLD_88: return Key_World_88;
		case 	SDLK_WORLD_89: return Key_World_89;
		case 	SDLK_WORLD_90: return Key_World_90;
		case 	SDLK_WORLD_91: return Key_World_91;
		case 	SDLK_WORLD_92: return Key_World_92;
		case 	SDLK_WORLD_93: return Key_World_93;
		case 	SDLK_WORLD_94: return Key_World_94;
		case 	SDLK_WORLD_95: return Key_World_95;
#endif
		}

		return Key_None;
	}

	void InputServiceSDL::UpdateKeyboard()
	{
		mKeysPressed.clear();
		mKeysReleased.clear();

		for (auto& it : mEvents)
		{
			if (it.type == SDL_KEYDOWN || it.type == SDL_KEYUP)
			{
				Key key = SDLToKey(it.key.keysym.sym);
				mKeyState[key] = it.type == SDL_KEYDOWN ? true : false;
				if (it.type == SDL_KEYDOWN)
				{
					AddKeyToList(it.key.keysym.mod, key, 0, mKeysPressed);
				}
				else
				{
					AddKeyToList(it.key.keysym.mod, key, 0, mKeysReleased);
				}
			}
		}
	}

	MouseButton InputServiceSDL::SDLToButton(const int aBtn)
	{
		switch (aBtn)
		{
			case SDL_BUTTON_LEFT:		return MB_Left;
			case SDL_BUTTON_RIGHT:		return MB_Right;
			case SDL_BUTTON_MIDDLE:		return MB_Middle;
		default:
			return MB_LastEnum;
		}
	}

	bool InputServiceSDL::IsWheelUpMoved() const
	{
		return mWheelUpMoved;
	}

	void InputServiceSDL::AddKeyToList(int aSDLMod, Key aKey, int aUnicode, std::list<KeyPress>& aKeys)
	{
		int mod = 0;

		if (aSDLMod & KMOD_CTRL)		mod |= KeyModifier_Ctrl;
		if (aSDLMod & KMOD_SHIFT)		mod |= KeyModifier_Shift;
		if (aSDLMod & KMOD_ALT)			mod |= KeyModifier_Alt;

		aKeys.push_back(KeyPress(aKey, aUnicode, mod));

		//if(mlstKeysPressed.size()>MAX_KEY_PRESSES) mlstKeysPressed.pop_front();
	}

	void InputServiceSDL::UpdateMouse()
	{
		mWheelDownMoved = false;
		mWheelUpMoved = false;

		for (auto it = mEvents.begin(); it != mEvents.end(); it++)
		{
			if (it->type == SDL_MOUSEBUTTONDOWN || it->type == SDL_MOUSEBUTTONUP)
			{
				const bool bButtonIsDown = it->type == SDL_MOUSEBUTTONDOWN;
				switch (it->button.button)
				{
					case SDL_BUTTON_LEFT:	mMouseState[MB_Left] = bButtonIsDown; break;
					case SDL_BUTTON_MIDDLE:	mMouseState[MB_Middle] = bButtonIsDown; break;
					case SDL_BUTTON_RIGHT:	mMouseState[MB_Right] = bButtonIsDown; break;
				}
			}
			else if (it->type == SDL_MOUSEWHEEL /* && it->wheel.y != 0 */)
			{
				if (it->wheel.y > 0)
				{
					mMouseState[MB_WheelUp] = true;
					mWheelUpMoved = true;
				}
				else 
				{
					mMouseState[MB_WheelDown] = true;
					mWheelDownMoved = true;
				}
			}
			else if (it->type == SDL_MOUSEMOTION)
			{
				mMousePos = Vector2l(it->motion.x, it->motion.y);
			}
		}
	}

	bool InputServiceSDL::IsWheelDownMoved() const
	{
		return mWheelDownMoved;
	}


	bool InputServiceSDL::IsQuitMessagePosted()
	{
		return mQuitMessagePosted;
	}
}