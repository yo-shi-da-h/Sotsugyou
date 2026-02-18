#pragma once
#include <math/Matrix4x4.h>
#include <math/Vector3.h>
#include <cmath>
#include <numbers>

KamataEngine::Matrix4x4 MatrixMultipry(const KamataEngine::Matrix4x4& matrix1, const KamataEngine::Matrix4x4& matrix2);

KamataEngine::Matrix4x4 ExtractRotation(const KamataEngine::Matrix4x4& matWorld);

KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rotate, const KamataEngine::Vector3& translate);

KamataEngine::Matrix4x4 MakeRotateZMatrix(float angle);

KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vec, const KamataEngine::Matrix4x4& mat);

KamataEngine::Vector3 Subtract(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2);

KamataEngine::Vector3 Normalize(const KamataEngine::Vector3& v);