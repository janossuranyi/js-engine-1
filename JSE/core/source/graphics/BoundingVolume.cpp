#include "graphics/BoundingVolume.h"

// RELEVANT IMPLEMENTATION:

// compute frustum planes from view and projection matrices
Frustum::Frustum( const glm::mat4 &v, const glm::mat4 &p ) : BoundingVolume()
{
	glm::mat4 clipMatrix;

	clipMatrix[0][0] = v[0][0]*p[0][0]+v[0][1]*p[1][0]+v[0][2]*p[2][0]+v[0][3]*p[3][0];
	clipMatrix[1][0] = v[0][0]*p[0][1]+v[0][1]*p[1][1]+v[0][2]*p[2][1]+v[0][3]*p[3][1];
	clipMatrix[2][0] = v[0][0]*p[0][2]+v[0][1]*p[1][2]+v[0][2]*p[2][2]+v[0][3]*p[3][2];
	clipMatrix[3][0] = v[0][0]*p[0][3]+v[0][1]*p[1][3]+v[0][2]*p[2][3]+v[0][3]*p[3][3];
	clipMatrix[0][1] = v[1][0]*p[0][0]+v[1][1]*p[1][0]+v[1][2]*p[2][0]+v[1][3]*p[3][0];
	clipMatrix[1][1] = v[1][0]*p[0][1]+v[1][1]*p[1][1]+v[1][2]*p[2][1]+v[1][3]*p[3][1];
	clipMatrix[2][1] = v[1][0]*p[0][2]+v[1][1]*p[1][2]+v[1][2]*p[2][2]+v[1][3]*p[3][2];
	clipMatrix[3][1] = v[1][0]*p[0][3]+v[1][1]*p[1][3]+v[1][2]*p[2][3]+v[1][3]*p[3][3];
	clipMatrix[0][2] = v[2][0]*p[0][0]+v[2][1]*p[1][0]+v[2][2]*p[2][0]+v[2][3]*p[3][0];
	clipMatrix[1][2] = v[2][0]*p[0][1]+v[2][1]*p[1][1]+v[2][2]*p[2][1]+v[2][3]*p[3][1];
	clipMatrix[2][2] = v[2][0]*p[0][2]+v[2][1]*p[1][2]+v[2][2]*p[2][2]+v[2][3]*p[3][2];
	clipMatrix[3][2] = v[2][0]*p[0][3]+v[2][1]*p[1][3]+v[2][2]*p[2][3]+v[2][3]*p[3][3];
	clipMatrix[0][3] = v[3][0]*p[0][0]+v[3][1]*p[1][0]+v[3][2]*p[2][0]+v[3][3]*p[3][0];
	clipMatrix[1][3] = v[3][0]*p[0][1]+v[3][1]*p[1][1]+v[3][2]*p[2][1]+v[3][3]*p[3][1];
	clipMatrix[2][3] = v[3][0]*p[0][2]+v[3][1]*p[1][2]+v[3][2]*p[2][2]+v[3][3]*p[3][2];
	clipMatrix[3][3] = v[3][0]*p[0][3]+v[3][1]*p[1][3]+v[3][2]*p[2][3]+v[3][3]*p[3][3];

	m_planes[PLANE_RIGHT].x = clipMatrix[3][0]-clipMatrix[0][0];
	m_planes[PLANE_RIGHT].y = clipMatrix[3][1]-clipMatrix[0][1];
	m_planes[PLANE_RIGHT].z = clipMatrix[3][2]-clipMatrix[0][2];
	m_planes[PLANE_RIGHT].w = clipMatrix[3][3]-clipMatrix[0][3];

	m_planes[PLANE_LEFT].x = clipMatrix[3][0]+clipMatrix[0][0];
	m_planes[PLANE_LEFT].y = clipMatrix[3][1]+clipMatrix[0][1];
	m_planes[PLANE_LEFT].z = clipMatrix[3][2]+clipMatrix[0][2];
	m_planes[PLANE_LEFT].w = clipMatrix[3][3]+clipMatrix[0][3];

	m_planes[PLANE_BOTTOM].x = clipMatrix[3][0]+clipMatrix[1][0];
	m_planes[PLANE_BOTTOM].y = clipMatrix[3][1]+clipMatrix[1][1];
	m_planes[PLANE_BOTTOM].z = clipMatrix[3][2]+clipMatrix[1][2];
	m_planes[PLANE_BOTTOM].w = clipMatrix[3][3]+clipMatrix[1][3];

	m_planes[PLANE_TOP].x = clipMatrix[3][0]-clipMatrix[1][0];
	m_planes[PLANE_TOP].y = clipMatrix[3][1]-clipMatrix[1][1];
	m_planes[PLANE_TOP].z = clipMatrix[3][2]-clipMatrix[1][2];
	m_planes[PLANE_TOP].w = clipMatrix[3][3]-clipMatrix[1][3];

	m_planes[PLANE_BACK].x = clipMatrix[3][0]-clipMatrix[2][0];
	m_planes[PLANE_BACK].y = clipMatrix[3][1]-clipMatrix[2][1];
	m_planes[PLANE_BACK].z = clipMatrix[3][2]-clipMatrix[2][2];
	m_planes[PLANE_BACK].w = clipMatrix[3][3]-clipMatrix[2][3];

	m_planes[PLANE_FRONT].x = clipMatrix[3][0]+clipMatrix[2][0];
	m_planes[PLANE_FRONT].y = clipMatrix[3][1]+clipMatrix[2][1];
	m_planes[PLANE_FRONT].z = clipMatrix[3][2]+clipMatrix[2][2];
	m_planes[PLANE_FRONT].w = clipMatrix[3][3]+clipMatrix[2][3];

	for( int i = 0; i < 6; i++ )
	{
		m_planes[i] = glm::normalize( m_planes[i] );
	}
}

const glm::vec4& Frustum::getPlane(const int plane) const {
	return m_planes[plane];
}

BoundingVolume::TestResult Frustum::testIntersection(const glm::vec3& point) const
{
	TestResult result = TEST_INSIDE;

	for (int i = 0; i < 6; i++)
	{
		const float pos = m_planes[i].w;
		const glm::vec3 normal = glm::vec3(m_planes[i]);

		if (glm::dot(normal, point) + pos < 0.0f)
		{
			return TEST_OUTSIDE;
		}
	}

	return result;
}

// check whether an AABB intersects the frustum
BoundingVolume::TestResult Frustum::testIntersection( const BoundingBox& box ) const
{
	TestResult result = TEST_INSIDE;

	for( int i = 0; i < 6; i++ )
	{
		const float pos = m_planes[i].w;
		const glm::vec3 normal = glm::vec3(m_planes[i]);

		if( glm::dot(normal, box.getPositiveVertex(normal))+pos < 0.0f )
		{
			return TEST_OUTSIDE;
		}

		if( glm::dot(normal, box.getNegativeVertex(normal))+pos < 0.0f )
		{
			result = TEST_INTERSECT;
		}
	}

	return result;
}

glm::vec3 BoundingBox::getPositiveVertex( const glm::vec3 &normal ) const 
{
	glm::vec3 positiveVertex = minimum;

	if( normal.x >= 0.0f ) positiveVertex.x = maximum.x;
	if( normal.y >= 0.0f ) positiveVertex.y = maximum.y;
	if( normal.z >= 0.0f ) positiveVertex.z = maximum.z;

	return position+positiveVertex;
}

glm::vec3 BoundingBox::getNegativeVertex( const glm::vec3 &normal ) const
{
	glm::vec3 negativeVertex = maximum;

	if( normal.x >= 0.0f ) negativeVertex.x = minimum.x;
	if( normal.y >= 0.0f ) negativeVertex.y = minimum.y;
	if( normal.z >= 0.0f ) negativeVertex.z = minimum.z;

	return position+negativeVertex;
}

void BoundingBox::AddPoint(const glm::vec3 point)
{
    if (point.x > maximum.x) maximum.x = point.x;
    if (point.y > maximum.y) maximum.y = point.y;
    if (point.z > maximum.z) maximum.z = point.z;
    if (point.x < minimum.x) minimum.x = point.x;
    if (point.y < minimum.y) minimum.y = point.y;
    if (point.z < minimum.z) minimum.z = point.z;

}

BoundingVolume::TestResult BoundingBox::testIntersection(const glm::vec3& point) const
{
	const glm::vec3 max = maximum + position;
	const glm::vec3 min = minimum + position;

	if (point.x > max.x || point.x > max.y || point.z > max.z || 
		point.x < min.x || point.y < min.y || point.z < min.z)
		return TEST_OUTSIDE;

	if (point.x > min.x || point.y > min.y || point.z > min.z)
		return TEST_INSIDE;

	return TEST_INTERSECT;
}

BoundingVolume::TestResult BoundingBox::testIntersection(const BoundingBox& box) const
{
	const glm::vec3 min2 = box.minimum + box.position;
	const glm::vec3 max1 = maximum + position;

	const bool out = (min2.x > max1.x || min2.y > max1.y || min2.z > max1.z);

	if (out) return TEST_OUTSIDE;

	const bool inside = (min2.x <= max1.x && min2.y <= max1.y && min2.z <= max1.z);
	
	if (inside) return TEST_INSIDE;

	return TEST_INTERSECT;

}
