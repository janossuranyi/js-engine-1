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
		AnimationTrack(const String& aName, Animation* aParent, const AnimationTrackType aType);

		size_t GetKeyframeNum() const { return mKeyframes.size(); }
		Keyframe& CreateKeyframe(const float aTime);
		void Clear();
		Keyframe& GetKey(const int aIndex);
		void ApplyOnNode(const float aTime, const float aWeight, const bool aLoop = true);
		float GetKeyframesAtTime(const float aTime, Keyframe& aKeyframeA, Keyframe& aKeyframeB, int& aIndex, const bool aLoop = true);
		const String& GetName() const { return mName; }
		const tKeyframeVec& GetKeyframes() const { return mKeyframes; }
		void SetNode(Node3d* aNode) { mNode = aNode; }
		Node3d* GetNode() const { return mNode; }

	private:
		Keyframe GetInterpolatedKeyframe(const float aTime, bool aLoop = true);
		Keyframe GetInterpolatedKeyframe_Linear(const float aTime, bool aLoop);

		AnimationTrackType mType;
		String mName;
		Animation* mParent;
		tKeyframeVec mKeyframes;
		float mMaxFrameTime;
		bool mUseLinearInterp;

		Node3d* mNode;

	};
}

#endif