#pragma once

#include <DirectXMath.h>
#include <Eigen/Dense>

DirectX::XMMATRIX ConvertMatrix(Eigen::Matrix4f eignMatrix);

Eigen::Matrix4f ConvertMatrix(DirectX::XMMATRIX directXMatrix);