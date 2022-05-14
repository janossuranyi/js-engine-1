#ifndef JSE_MATERIAL_H
#define JSE_MATERIAL_H

#include "GraphicsTypes.hpp"

namespace jse {

	class Material
	{
	public:

		Material() {
			ambient = Color3(0.0f);
			diffuse = Color3(0.0f);
			specular = Color3(0.0f);
			specularIntesity = 0.0f;
			alpha = 1.0f;
			type = MaterialType_Diffuse;
		}

		MaterialType type;
		Color3 ambient;
		Color3 diffuse;
		Color3 specular;
		float specularIntesity;
		float alpha;
	};
}

#endif