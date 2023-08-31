
#ifndef UTILS
#define UTILS

#include <Eigen/Dense>

inline Eigen::Matrix4f GetMatPerspectiveProjection(float fovY, float aspect, float near, float far)
{
	float theta = fovY * 0.5;
	float range = far - near;
	float invtan = 1. / tan(theta);
	Eigen::Matrix4f translate;
    translate <<
		invtan / aspect, 0, 0, 0,
		0, invtan, 0, 0,
		0, 0, -(near + far) / range, -2 * near * far / range,
		0, 0, -1, 0;
	return translate;
}

inline Eigen::Matrix4f GetMatTranslation(float dx, float dy, float dz)
{
	Eigen::Matrix4f translate;
	translate <<
		1, 0, 0, dx,
		0, 1, 0, dy,
		0, 0, 1, dz,
		0, 0, 0, 1;
	return translate;
}


#endif