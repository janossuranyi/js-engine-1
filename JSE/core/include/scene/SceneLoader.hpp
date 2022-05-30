#ifndef JSE_SCENE_LOADER_H
#define JSE_SCENE_LOADER_H

#include "system/SystemTypes.hpp"
#include <map>

namespace jse {

	class Scene;

	typedef std::map<String, AnyValue> ParamsMap;

	class SceneLoader
	{
	public:
		virtual ~SceneLoader() {};
		virtual int LoadScene(const String& aFile) = 0;

		void SetParam(const String& aName, const AnyValue& aValue)
		{
			mParams.insert(ParamsMap::value_type(aName, aValue));
		}

	private:
		ParamsMap mParams;
	};



}
#endif // !JSE_SCENE_LOADER_H
