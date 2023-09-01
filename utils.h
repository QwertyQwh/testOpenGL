
#ifndef UTILS
#define UTILS

#include <Eigen/Dense>
using Eigen::Vector3f;
using Eigen::Vector4f;
using Eigen::Matrix4f;

inline Matrix4f GetMatPerspectiveProjection(const float fovY, const float aspect, const float near, const float far) 
{
	float theta = fovY * 0.5;
	float range = far - near;
	float invtan = 1. / tan(theta);
	Matrix4f translate;
    translate <<
		invtan / aspect, 0, 0, 0,
		0, invtan, 0, 0,
		0, 0, -(near + far) / range, -2 * near * far / range,
		0, 0, -1, 0;
	return translate;
}

inline Matrix4f GetMatTranslation(const float dx, const float dy, const float dz)
{
	Matrix4f translate;
	translate <<
		1, 0, 0, dx,
		0, 1, 0, dy,
		0, 0, 1, dz,
		0, 0, 0, 1;
	return translate;
}


inline Matrix4f GetLookAtMat(const Vector3f pos, const Vector3f target, const Vector3f up)
{
	const auto camForward = (-target + pos).normalized();
	const auto camRight = camForward.cross(up).normalized();
	const auto camUp = camForward.cross(camRight);

	Eigen::Affine3f transform = Eigen::Affine3f::Identity();
	transform.translation() = -pos;
	Eigen::Matrix3f rot;
	rot.row(0) = camRight;
	rot.row(1) = camUp;
	rot.row(2) = camForward;
	transform.rotate(rot);
	return transform.matrix();
}

#endif