#ifndef JSE_ANIMATION_H
#define JSE_ANIMATION_H

#include <vector>
#include "system/SystemTypes.hpp"
#include "scene/AnimationTrack.hpp"

namespace jse {

	class Animation {
	public:
		Animation(const String& aName);
		~Animation();

		inline float GetLength() const { return mLength; }
		void SetLength(const float aLength) { mLength = aLength; }
		void SetTicksPerSec(const float a0);
		AnimationTrack& CreateTrack(const String& aName);
		AnimationTrack& GetTrack(const size_t aIdx);
		const AnimationTrack* GetTrackByName(const String& aName) const;
		size_t GetTrackNum() const { return mTracks.size(); };
		inline float GetTicksPerSec() const { return mTicksPerSec; }
		inline const String& GetName() const { return mName; }
		void ApplyFrame(const float aTime);

		void SetNode(Node3d* aNode);
		Node3d* GetNode();

	private:
		Node3d* mNode;
		float mLength;
		float mTicksPerSec;
		String mName;
		std::vector<AnimationTrack> mTracks;
	};
}

#endif