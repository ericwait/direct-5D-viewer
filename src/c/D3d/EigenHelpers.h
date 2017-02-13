#pragma once

#include <Eigen/Geometry>

// Some convenience functions for making useful transform types out of geometry types
inline Eigen::Affine3f RotationTransform(float angle, const Eigen::Vector3f& axis)
{
	return Eigen::Affine3f(Eigen::AngleAxisf(angle, axis));
}
