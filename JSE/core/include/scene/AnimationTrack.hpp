#ifndef JSE_ANIMATION_TRACK_H
#define JSE_ANIMATION_TRACK_H

#include <vector>
#include "system/SystemTypes.hpp"
#include "scene/AnimationTrack.hpp"
#include "graphics/GraphicsTypes.hpp"

namespace jse {
	class Animation;
	class Node3d;

	typedef std::vector<Keyframe> tKeyframeVec;

	class AnimationTrack
	{
	public:
		AnimationTrack(const String& aName, Animation* aParent);

		size_t GetKeyframeNum() const { return mKeyframes.size(); }
		Keyframe& CreateKeyframe(const float aTime);
		void Clear();
		Keyframe& GetKey(const int aIndex);
		void ApplyOnNode(Node3d* aNode, const float aTime, const float aWeight, const bool aLoop = true);
		Keyframe GetInterpolatedKeyframe(const float aTime, bool aLoop = true);
		float GetKeyframesAtTime(const float aTime, Keyframe& aKeyframeA, Keyframe& aKeyframeB, size_t& aIndex, const bool aLoop = true);
		const String& GetName() const { return mName; }
		const tKeyframeVec& GetKeyframes() const { return mKeyframes; }

	private:
		String mName;
		tKeyframeVec mKeyframes;
		float mMaxFrameTime;
		Animation* mParent;
		bool mUseLinearInterp;
	};
}

#endif