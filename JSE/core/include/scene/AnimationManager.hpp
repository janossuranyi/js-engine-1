#ifndef JSE_ANIMATION_MGR_H
#define JSE_ANIMATION_MGR_H

#include "system/SystemTypes.hpp"
#include "scene/Animation.hpp"
#include <vector>

namespace jse {

	class AnimationManager
	{
	public:
		AnimationManager();
		~AnimationManager();

		void AddAnimation(Animation* aX);
		void UpdateState(const float aFrameStep);

	private:
		std::vector<Animation*> mAnimVec;
		using AnimVecIt = std::vector<Animation*>::iterator;

		double mFrame;
		float mTicksPerSec;
		float mMaxFrameTime;
	};

}

#endif