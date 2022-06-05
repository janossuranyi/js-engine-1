#ifndef JSE_BOUNDING_VOLUME_H
#define JSE_BOUNDING_VOLUME_H

#include <memory>

#include "graphics/GraphicsTypes.hpp"

namespace jse {
	// DECLARATION:

	class BoundingBox;

	class BoundingVolume
	{

	public:

		enum TestResult
		{
			TEST_OUTSIDE,
			TEST_INTERSECT,
			TEST_INSIDE
		};

	public:

		virtual TestResult testIntersection(const vec3& point) const = 0;
		virtual TestResult testIntersection(const BoundingBox& box) const = 0;
		//virtual TestResult testIntersection( std::shared_ptr<const BoundingSphere> sphere ) const = 0;
	};

	class BoundingBox : public BoundingVolume
	{
	public:

		vec3 minimum;
		vec3 maximum;

		vec3 position;

	public:

		BoundingBox(vec3 minimum, vec3 maximum, vec3 position = vec3(0.0f)) : BoundingVolume()
		{
			this->minimum = minimum;
			this->maximum = maximum;

			this->position = position;
		}

		virtual vec3 getPositiveVertex(const vec3& normal) const;
		virtual vec3 getNegativeVertex(const vec3& normal) const;
		virtual void AddPoint(const vec3 point);

		TestResult testIntersection(const vec3& point) const;
		TestResult testIntersection(const BoundingBox& box) const;
		//TestResult testIntersection( shared_ptr<const BoundingSphere> sphere ) const;
	};

	class Frustum : public BoundingVolume
	{
	public:

		enum Plane
		{
			PLANE_BACK,
			PLANE_FRONT,
			PLANE_RIGHT,
			PLANE_LEFT,
			PLANE_TOP,
			PLANE_BOTTOM
		};

	public:

		Frustum(const mat4& viewMatrix, const mat4& projectionMatrix);

		const vec4& getPlane(const int plane) const;

		TestResult testIntersection(const vec3& point) const;
		TestResult testIntersection(const BoundingBox& box) const;
		//TestResult testIntersection( std::shared_ptr<const BoundingSphere> sphere ) const;

	protected:

		vec4 m_planes[6];
	};
}

#endif

