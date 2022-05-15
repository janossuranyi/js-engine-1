#ifndef JSE_ANIMATION_H
#define JSE_ANIMATION_H

#include <vector>
#include "system/SystemTypes.hpp"

namespace jse {

	class AnimationTrack;

	typedef std::vector<AnimationTrack*> tAnimationTrackVec;

	class Animation {
	public:
		Animation(const String& aName);
		~Animation();

		inline float GetLength() const { return mLength; }
		void SetLength(const float aLength) { mLength = aLength; }
		AnimationTrack* CreateTrack(const String& aName);
		AnimationTrack* GetTrack(const int aIdx);
		AnimationTrack* GetTrackByName(const String& aName);
		inline int GetTrackNum() const { return mTracks.size(); };
		inline const String& GetName() const { return mName; }
	private:
		float mLength;
		String mName;
		tAnimationTrackVec mTracks;
	};
}

#endif