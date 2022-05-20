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
	AnimationTrack::AnimationTrack(const String& aName, Animation* aParent)
	{
		mName = aName;
		mParent = aParent;
		mMaxFrameTime = 0.0f;
		mUseLinearInterp = true;
	}
	
	AnimationTrack::~AnimationTrack()
	{
		//std::for_each(mKeyframes.begin(), mKeyframes.end(), [](const Keyframe* p) {delete p; });
	}

	Keyframe& AnimationTrack::CreateKeyframe(const float aTime)
	{
		Keyframe n;

		n.time = aTime;

		if (aTime > mMaxFrameTime || mKeyframes.empty())
		{
			mKeyframes.push_back(n);
			mMaxFrameTime = aTime;

			return mKeyframes.back();
		}
		else
		{
			auto it = mKeyframes.begin();
			for (; it != mKeyframes.end(); it++)
			{
				if ((it)->time > aTime)
				{
					break;
				}
			}
			auto e = mKeyframes.insert(it, n);
			return *e;
		}
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

	void AnimationTrack::ApplyOnNode(Node3d* aNode, const float aTime, const float aWeight, const bool aLoop)
	{
		if (mKeyframes.empty())
			return;

		Keyframe Frame = GetInterpolatedKeyframe(aTime, aLoop);

		//Quat rot = glm::normalize( glm::slerp(Quat(), Frame.rotation, aWeight) );
		//Vector3f trans = aWeight * Frame.position;

		aNode->AddPosition(Frame.position);
		aNode->AddRotation(Frame.rotation);

		//Info("Time: %.2f, Pos: [%.2f,%.2f,%.2f]", aTime, Frame.position.x, Frame.position.y, Frame.position.z);
	}
	
	Keyframe AnimationTrack::GetInterpolatedKeyframe(const float aTime, bool aLoop)
	{
		Keyframe r;
		r.time = aTime;

		if (mKeyframes.empty())
		{
			r.position = Vector3f(0.0f);
			r.rotation = Quat(1, 0, 0, 0);
			return r;
		}

		Keyframe kA, kB;
		size_t keyIndex = -1;

		const float T = GetKeyframesAtTime(aTime, kA, kB, keyIndex, aLoop);


		if (T == 0.0f)
		{
			r.rotation = kA.rotation;
			r.position = kA.position;
		}
		else
		{
			//Info("T != 0.0 (%.4f)", T);
			if (!mUseLinearInterp)
			{
				if (keyIndex == 0 || keyIndex == mKeyframes.size() - 1)
				{
					r.rotation = glm::slerp(kA.rotation, kB.rotation, T);
					r.position = LinearInterp(kA.position, kB.position, T);
				}
				else
				{
					r.rotation = HermiteInterp(mKeyframes[keyIndex - 1].rotation, kA.rotation, kB.rotation, mKeyframes[keyIndex + 1].rotation, T, 0.f,0.f);
					r.position = HermiteInterp(mKeyframes[keyIndex - 1].position, kA.position, kB.position, mKeyframes[keyIndex + 1].position, T, 0.f,0.f);
				}
			}
			else
			{
				r.rotation = glm::slerp(kA.rotation, kB.rotation, T);
				r.position = LinearInterp(kA.position, kB.position, T);
			}
		}

		r.rotation = glm::normalize(r.rotation);

		return r;
	}
	
	float AnimationTrack::GetKeyframesAtTime(const float aTime, Keyframe& aKeyframeA, Keyframe& aKeyframeB, size_t& aIndex, const bool aLoop)
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
