#include "EigenToFromDirectX.h"

DirectX::XMMATRIX ConvertMatrix(const Eigen::Matrix4f& eignMatrix)
{
    DirectX::XMFLOAT4X4 floatMatrix;
    for(int r = 0; r<4; ++r)
    {
        for(int c = 0; c<4; ++c)
        {
            floatMatrix.m[r][c] = eignMatrix(c, r);
        }
    }

    DirectX::XMMATRIX directXMatrix = DirectX::XMLoadFloat4x4(&floatMatrix);

    return directXMatrix;
}

Eigen::Matrix4f ConvertMatrix(DirectX::XMMATRIX directXMatrix)
{
    Eigen::Matrix4f eignMatrix;

    DirectX::XMFLOAT4X4 floatMatrix;
    DirectX::XMStoreFloat4x4(&floatMatrix, directXMatrix);
    for(int r = 0; r<4; ++r)
    {
        for(int c = 0; c<4; ++c)
        {
            eignMatrix(c, r) = floatMatrix.m[r][c];
        }
    }

    return eignMatrix;
}
