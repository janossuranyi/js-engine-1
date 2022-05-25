#ifndef JSE_FRUSTUM_H
#define JSE_FRUSTUM_H

#include "graphics/GraphicsTypes.hpp"

namespace jse {
	class Frustum
	{
	public:
		Frustum(const Matrix& aProjMtx);
	private:
		Vector4f mPlanes[6];
	};
}

#endif