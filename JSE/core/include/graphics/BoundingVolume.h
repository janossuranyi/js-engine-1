#ifndef __BOUNDING_VOLUME_H__
#define __BOUNDING_VOLUME_H__

#include <glm/glm.hpp>
#include <memory>

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
	
		virtual TestResult testIntersection( const glm::vec3 &point ) const = 0;
		virtual TestResult testIntersection( const BoundingBox& box ) const = 0;
		//virtual TestResult testIntersection( std::shared_ptr<const BoundingSphere> sphere ) const = 0;
};

class BoundingBox : public BoundingVolume
{
	public:
	
		glm::vec3 minimum;
		glm::vec3 maximum;
		
		glm::vec3 position;

	public:

		BoundingBox( glm::vec3 minimum, glm::vec3 maximum, glm::vec3 position = glm::vec3(0.0f) ) : BoundingVolume()
		{
			this->minimum = minimum;
			this->maximum = maximum;
			
			this->position = position;
		}

		virtual glm::vec3 getPositiveVertex( const glm::vec3 &normal ) const;
		virtual glm::vec3 getNegativeVertex( const glm::vec3 &normal ) const;
        virtual void AddPoint( const glm::vec3 point );

		TestResult testIntersection( const glm::vec3& point ) const;
		TestResult testIntersection( const BoundingBox& box ) const;
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

		Frustum( const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix );

		const glm::vec4 &getPlane( const int plane ) const;
		
		TestResult testIntersection( const glm::vec3 &point ) const;
		TestResult testIntersection( const BoundingBox& box ) const;
		//TestResult testIntersection( std::shared_ptr<const BoundingSphere> sphere ) const;

   	protected:

		glm::vec4 m_planes[6];
};
#endif

