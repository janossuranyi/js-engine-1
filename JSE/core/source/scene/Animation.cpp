#include "scene/Animation.hpp"
#include "scene/AnimationTrack.hpp"

#include <algorithm>

namespace jse {

	Animation::Animation(const String& aName)
	{
		mLength = 0.0f;
		mName = aName;
	}

	Animation::~Animation()
	{
		std::for_each(mTracks.begin(), mTracks.end(), [](AnimationTrack* _t) { delete _t; });
	}

	AnimationTrack* Animation::CreateTrack(const String& aName)
	{
		AnimationTrack* t = new AnimationTrack(aName, this);
		mTracks.push_back(t);

		return t;

	}

	AnimationTrack* Animation::GetTrack(const int aIdx)
	{
		return mTracks[aIdx];
	}

	AnimationTrack* Animation::GetTrackByName(const String& aName)
	{
		auto it = mTracks.begin();
		for (; it != mTracks.end(); it++)
		{
			if ((*it)->GetName() == aName)
				return *it;
		}

		return nullptr;
	}

}
