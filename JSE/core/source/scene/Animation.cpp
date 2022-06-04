#include <algorithm>
#include <cassert>

#include "scene/Animation.hpp"
#include "scene/AnimationTrack.hpp"
#include "scene/Node3d.hpp"
#include "graphics/GraphicsTypes.hpp"

namespace jse {

	Animation::Animation(const String& aName)
	{
		mLength = 0.0f;
		mName = aName;
		mTicksPerSec = 25.0f;
	}

	Animation::~Animation()
	{
		//std::for_each(mTracks.begin(), mTracks.end(), [](AnimationTrack* _t) { delete _t; });
	}

	void Animation::SetTicksPerSec(const float a0)
	{
		mTicksPerSec = a0;
	}

	AnimationTrack& Animation::CreateTrack(const String& aName, const AnimationTrackType aType, Node3d* aNode)
	{
		mTracks.emplace_back(aName, this, aType);

		AnimationTrack& t = mTracks.back();
		t.SetNode(aNode);

		return t;

	}

	AnimationTrack& Animation::GetTrack(const size_t aIdx)
	{
		assert(aIdx < mTracks.size());

		return mTracks[aIdx];
	}

	const AnimationTrack* Animation::GetTrackByName(const String& aName) const
	{
		for (auto it = mTracks.begin(); it != mTracks.end(); it++)
		{
			if (it->GetName() == aName)
			{
				return &(*it);
			}
		}

		return nullptr;
	}

	void Animation::ApplyFrame(const float aTime)
	{
		for (auto& it : mTracks)
		{
			it.ApplyOnNode(aTime, 1.0f);
		}
	}
}
