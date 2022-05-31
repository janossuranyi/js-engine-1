#include "scene/AnimationManager.hpp"

namespace jse {

	AnimationManager::AnimationManager()
	{
		mFrame = 0.0;
		mMaxFrameTime = 0.0f;
		mTicksPerSec = 1.0f;
	}

	AnimationManager::~AnimationManager()
	{
	}

	void AnimationManager::AddAnimation(Animation* aX)
	{
		mAnimVec.push_back(aX);
		mMaxFrameTime = mMaxFrameTime < aX->GetLength() ? aX->GetLength() : mMaxFrameTime;
		mTicksPerSec = aX->GetTicksPerSec();
	}

	void AnimationManager::UpdateState(const float aFrameStep)
	{
		for (auto it : mAnimVec)
		{
			if (it->GetLength() >= mFrame) it->ApplyFrame(mFrame);
		}

		mFrame += aFrameStep * mTicksPerSec;

		if (mFrame > mMaxFrameTime)
			mFrame -= mMaxFrameTime;

	}


}
