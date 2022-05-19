#include "scene/Animation.hpp"
#include "scene/AnimationTrack.hpp"
#include "scene/Node3d.hpp"

#include <algorithm>

namespace jse {

	Animation::Animation(const String& aName)
	{
		mLength = 0.0f;
		mName = aName;
		mTicksPerSec = 25.0f;
	}

	Animation::~Animation()
	{
		std::for_each(mTracks.begin(), mTracks.end(), [](AnimationTrack* _t) { delete _t; });
	}

	void Animation::SetTicksPerSec(const float a0)
	{
		mTicksPerSec = a0;
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

	void Animation::ApplyFrame(const float aTime)
	{
		mNode->SetPosition(Vector3f(0.0f));
		mNode->SetRotation(Quat(1, 0, 0, 0));

		std::for_each(mTracks.begin(), mTracks.end(), [&](AnimationTrack* const t)
		{
			t->ApplyOnNode(mNode, aTime, 1.0f);
		});
	}

	void Animation::SetNode(Node3d* aNode)
	{
		mNode = aNode;
	}

	Node3d* Animation::GetNode()
	{
		return mNode;
	}

}
