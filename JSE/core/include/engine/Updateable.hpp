#ifndef JSE_UPDATEABLE_H
#define JSE_UPDATEABLE_H

#include "system/SystemTypes.hpp"

namespace jse {

    enum UpdateMessage
    {
        UpdateMessage_Update,
        UpdateMessage_PreUpdate,
        UpdateMessage_PostUpdate,
        UpdateMessage_OnDraw,
        UpdateMessage_OnStart,
        UpdateMessage_OnExit,
        UpdateMessage_Reset,
        UpdateMessage_LastEnum
    };

    class Updateable
    {
    public:
        Updateable(const String& aName) : mName(aName) {}
        virtual ~Updateable() {}
        virtual void PreUpdate(float aTimeStep) {}
        virtual void Update(float aTimeStep) {}
        virtual void PostUpdate(float aTimeStep) {}
        virtual void OnStart() {}
        virtual void OnExit() {}
        virtual void OnDraw(float aFrameTime) {}
        virtual void Reset() {}
        const String& GetName() const { return mName; }

        void RunMessage(const UpdateMessage aMessage, const float a0)
        {
            switch (aMessage)
            {
            case UpdateMessage_Update:
                Update(a0); break;
            case UpdateMessage_PreUpdate:
                PreUpdate(a0); break;
            case UpdateMessage_PostUpdate:
                PostUpdate(a0); break;
            case UpdateMessage_OnDraw:
                OnDraw(a0); break;
            case UpdateMessage_OnStart:
                OnStart(); break;
            case UpdateMessage_OnExit:
                OnExit(); break;
            case UpdateMessage_Reset:
                Reset(); break;                
            }
        }

    private:
        String mName;
    };
}

#endif
