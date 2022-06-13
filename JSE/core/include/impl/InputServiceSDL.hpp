#ifndef JSE_INPUTDRV_SDL_H
#define JSE_INPUTDRV_SDL_H

#include "input/InputService.hpp"
#include "input/InputTypes.hpp"
#include "graphics/GraphicsTypes.hpp"
#include <list>
#include <vector>
#include <mutex>
#include <SDL.h>

namespace jse {

    class InputServiceSDL : public InputService
    {
    public:
        InputServiceSDL();
        ~InputServiceSDL();

        bool IsKeyDown(const Key aKey);
        bool IsButtonDown(const MouseButton aButton) const;
        KeyPress GetKeyPressed();
        KeyPress GetKeyReleased();
        bool IsKeyPressed();
        bool IsKeyReleased() ;
        Vector2l GetMousePosition() const;

        void SetRelativeMouseMode(const bool a0);
        Vector2l GetRelativeMousePosition() const;

        bool IsWheelUpMoved() const;
        bool IsWheelDownMoved() const;

        void Update();

        bool IsQuitMessagePosted();
    private:
        Key SDLToKey(const int aKey);
        MouseButton SDLToButton(const int aBtn);
        void UpdateKeyboard();
        void UpdateMouse();
        void AddKeyToList(int aSDLMod, Key aKey, int aUnicode, std::list<KeyPress>& aKeys);

        bool mQuitMessagePosted;
        bool mWheelUpMoved;
        bool mWheelDownMoved;

        Vector2l mMousePos;
        std::list<SDL_Event> mEvents;
        std::list<KeyPress> mKeysPressed;
        std::list<KeyPress> mKeysReleased;
        std::vector<bool> mKeyState;
        std::vector<bool> mMouseState;
        std::mutex mtx;
    };

}

#endif