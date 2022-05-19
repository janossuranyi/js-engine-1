#ifndef JSE_ANIMATION_TRACK_H
#define JSE_ANIMATION_TRACK_H

#include <vector>
#include "system/SystemTypes.hpp"

namespace jse {
	class Animation;
	class Keyframe;
	class Node3d;

	typedef std::vector<Keyframe*> tKeyframeVec;

	class AnimationTrack
	{
	public:
		AnimationTrack(const String& aName, Animation* aParent);
		~AnimationTrack();

		Keyframe* CreateKeyframe(const float aTime);
		void Clear();
		Keyframe* GetKey(const int aIndex);
		void ApplyOnNode(Node3d* aNode, const float aTime, const float aWeight, const bool aLoop = true);
		Keyframe GetInterpolatedKeyframe(const float aTime, bool aLoop = true);
		float GetKeyframesAtTime(const float aTime, Keyframe** aKeyframeA, Keyframe** aKeyframeB, int& aIndex, const bool aLoop = true);
		const String& GetName() const { return mName; }

	private:
		String mName;
		tKeyframeVec mKeyframes;
		float mMaxFrameTime;
		Animation* mParent;
		bool mUseLinearInterp;
	};
}

#endif