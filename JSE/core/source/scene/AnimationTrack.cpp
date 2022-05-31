#include <glm/gtc/quaternion.hpp>
#include <algorithm>
#include "system/SystemTypes.hpp"
#include "system/Logger.hpp"
#include "graphics/GraphicsTypes.hpp"
#include "math/Interp.hpp"
#include "scene/AnimationTrack.hpp"
#include "scene/Animation.hpp"
#include "scene/Node3d.hpp"


namespace jse 
{
	AnimationTrack::AnimationTrack(const String& aName, Animation* aParent, const AnimationTrackType aType)
	{
		mType = aType;
		mName = aName;
		mParent = aParent;
		mMaxFrameTime = 0.0f;
		mUseLinearInterp = true;
		mNode = nullptr;
	}
	
	Keyframe& AnimationTrack::CreateKeyframe(const float aTime)
	{
		assert(mMaxFrameTime <= aTime);

		Keyframe n;

		n.time = aTime;


		mKeyframes.push_back(n);
		mMaxFrameTime = aTime;

		return mKeyframes.back();
	}

	void AnimationTrack::Clear()
	{
		mKeyframes.clear();
		mMaxFrameTime = 0.0;
	}
	Keyframe& AnimationTrack::GetKey(const int aIndex)
	{
		assert(aIndex >= 0 && aIndex < mKeyframes.size());

		return mKeyframes[aIndex];
	}

	void AnimationTrack::ApplyOnNode(const float aTime, const float aWeight, const bool aLoop)
	{
		if (mKeyframes.empty() || !mNode)
			return;

		mNode->SetAnimated(true);

		Keyframe Frame = GetInterpolatedKeyframe(aTime, aLoop);

		//Quat rot = glm::normalize( glm::slerp(Quat(), Frame.rotation, aWeight) );
		//Vector3f trans = aWeight * Frame.position;

		if (mType == AnimationTrackType_Position)
		{
			//mNode->SetPosition(Vector3f(0.0f));
			mNode->SetPosition(Frame.v);
		}
		else if (mType == AnimationTrackType_Rotation)
		{
			//mNode->SetRotation(Quat(1, 0, 0, 0));
			mNode->SetRotation(Frame.q);
		}

		//Info("Time: %.2f, Pos: [%.2f,%.2f,%.2f]", aTime, Frame.position.x, Frame.position.y, Frame.position.z);
	}

	Keyframe AnimationTrack::GetInterpolatedKeyframe_Linear(const float aTime, bool aLoop)
	{
		Keyframe r;
		r.time = aTime;

		Keyframe kA, kB;
		int keyIndex = -1;

		const float T = GetKeyframesAtTime(aTime, kA, kB, keyIndex, aLoop);

		if (mType == AnimationTrackType_Position)
		{
			if (mKeyframes.empty())
			{
				r.v = vec3(0.f);
				return r;
			}

			if (T == 0.0f)
			{
				r.v = kA.v;
			}
			else
			{
				r.v = LinearInterp(kA.v, kB.v, T);
			}
		}
		else if (mType == AnimationTrackType_Rotation)
		{
			//Info("T != 0.0 (%.4f)", T);
			if (T == 0.0f)
			{
				r.q = kA.q;
			}
			else
			{
				r.q = glm::normalize( glm::slerp(kA.q, kB.q, T) );
			}
		}
		return r;
	}

	Keyframe AnimationTrack::GetInterpolatedKeyframe(const float aTime, bool aLoop)
	{
		if (mUseLinearInterp)
			return GetInterpolatedKeyframe_Linear(aTime, aLoop);

		Keyframe r;
		r.time = aTime;

		Keyframe kA, kB;
		int keyIndex = -1;

		const float T = GetKeyframesAtTime(aTime, kA, kB, keyIndex, aLoop);

		if (mType == AnimationTrackType_Position)
		{
			if (mKeyframes.empty())
			{
				r.v = vec3(0.f);
				return r;
			}

			if (T == 0.0f)
			{
				r.v = kA.v;
			}
			else
			{
				//Info("T != 0.0 (%.4f)", T);
				if (keyIndex == 0 || keyIndex == mKeyframes.size() - 1)
				{
					r.v = LinearInterp(kA.v, kB.v, T);
				}
				else
				{
					r.v = HermiteInterp(mKeyframes[keyIndex - 1].v, kA.v, kB.v, mKeyframes[keyIndex + 1].v, T, 0.f, 0.f);
				}
			}
		}
		else if (mType == AnimationTrackType_Rotation)
		{
			//Info("T != 0.0 (%.4f)", T);
			if (T == 0.0f)
			{
				r.q = kA.q;
			}
			else
			{
				if (keyIndex == 0 || keyIndex == mKeyframes.size() - 1)
				{
					r.q = glm::slerp(kA.q, kB.q, T);
				}
				else
				{
					r.q = HermiteInterp(mKeyframes[keyIndex - 1].q, kA.q, kB.q, mKeyframes[keyIndex + 1].q, T, 0.f, 0.f);
				}

				r.q = glm::normalize(r.q);
			}
		}

		return r;
	}

	float AnimationTrack::GetKeyframesAtTime(const float aTime, Keyframe& aKeyframeA, Keyframe& aKeyframeB, int& aIndex, const bool aLoop)
	{
		const float animLength = mParent->GetLength();

		float time = aTime < 0.0f ? 0.0f : (aTime > animLength ? animLength : aTime);

		if (time >= mMaxFrameTime)
		{
			aKeyframeA = mKeyframes[mKeyframes.size() - 1];
			aKeyframeB = mKeyframes[0];
			aIndex = mKeyframes.size();
			return 0.0f;

		}

		const size_t lsize = mKeyframes.size();
		int idxB = -1;
		for (int i = 0; i < lsize; i++)
		{
			if (time < mKeyframes[i].time)
			{
				idxB = i;
				break;
			}
		}

		assert(idxB >= 0);

		if (idxB == 0)
		{
			aIndex = 0;
			aKeyframeA = mKeyframes[0];
			aKeyframeB = mKeyframes[0];

			return 0.0f;
		}

		aIndex = idxB - 1;
		aKeyframeA = mKeyframes[aIndex];
		aKeyframeB = mKeyframes[idxB];

		const float dt = aKeyframeB.time - aKeyframeA.time;


		return (time - aKeyframeA.time) / dt;
	}
}
