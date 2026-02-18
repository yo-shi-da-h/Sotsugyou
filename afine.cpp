#include "afine.h"

KamataEngine::Matrix4x4 MatrixMultipry(const KamataEngine::Matrix4x4& matrix1, const KamataEngine::Matrix4x4& matrix2) {
	KamataEngine::Matrix4x4 result;

	for (int i = 0; i < 4; ++i) {

		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = matrix1.m[i][0] * matrix2.m[0][j] + matrix1.m[i][1] * matrix2.m[1][j] + matrix1.m[i][2] * matrix2.m[2][j] + matrix1.m[i][3] * matrix2.m[3][j];
		}
	}

	return result;
}

KamataEngine::Matrix4x4 ExtractRotation(const KamataEngine::Matrix4x4& matWorld) {
	KamataEngine::Matrix4x4 rotation = matWorld;
	rotation.m[0][3] = 0.0f; // 平行移動を無効化
	rotation.m[1][3] = 0.0f;
	rotation.m[2][3] = 0.0f;
	rotation.m[3][0] = 0.0f; // スケールも無効化
	rotation.m[3][1] = 0.0f;
	rotation.m[3][2] = 0.0f;
	rotation.m[3][3] = 1.0f;
	return rotation;
}

KamataEngine::Matrix4x4 MakeAffineMatrix(const KamataEngine::Vector3& scale, const KamataEngine::Vector3& rot, const KamataEngine::Vector3& translate) {

	// エラー対策(使用しない)
	KamataEngine::Vector3 dm = scale;

	// 回転
	KamataEngine::Matrix4x4 RotateMatY = {cosf(rot.y), 0, -sinf(rot.y), 0, 0, 1, 0, 0, sinf(rot.y), 0, cosf(rot.y), 0, 0, 0, 0, 1};

	// 平行移動行列の作成
	KamataEngine::Matrix4x4 TranslateMat = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, translate.x, translate.y, translate.z, 1};

	// 回転*平行移動だけをワールド変換行列に
	KamataEngine::Matrix4x4 ansMat = MatrixMultipry(RotateMatY, TranslateMat);

	return ansMat;
}

KamataEngine::Matrix4x4 MakeRotateZMatrix(float angle) {
	KamataEngine::Matrix4x4 matrix;

	float cosAngle = std::cos(angle);
	float sinAngle = std::sin(angle);

	matrix.m[0][0] = cosAngle;
	matrix.m[0][1] = -sinAngle;
	matrix.m[0][2] = 0.0f;
	matrix.m[0][3] = 0.0f;

	matrix.m[1][0] = sinAngle;
	matrix.m[1][1] = cosAngle;
	matrix.m[1][2] = 0.0f;
	matrix.m[1][3] = 0.0f;

	matrix.m[2][0] = 0.0f;
	matrix.m[2][1] = 0.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[2][3] = 0.0f;

	matrix.m[3][0] = 0.0f;
	matrix.m[3][1] = 0.0f;
	matrix.m[3][2] = 0.0f;
	matrix.m[3][3] = 1.0f;

	return matrix;
}

KamataEngine::Matrix4x4 CreateRotationY(float angle) {
	KamataEngine::Matrix4x4 rotationMatrix;

	float cosAngle = std::cos(angle);
	float sinAngle = std::sin(angle);

	rotationMatrix.m[0][0] = cosAngle;
	rotationMatrix.m[0][1] = 0.0f;
	rotationMatrix.m[0][2] = -sinAngle;
	rotationMatrix.m[0][3] = 0.0f;

	rotationMatrix.m[1][0] = 0.0f;
	rotationMatrix.m[1][1] = 1.0f;
	rotationMatrix.m[1][2] = 0.0f;
	rotationMatrix.m[1][3] = 0.0f;

	rotationMatrix.m[2][0] = sinAngle;
	rotationMatrix.m[2][1] = 0.0f;
	rotationMatrix.m[2][2] = cosAngle;
	rotationMatrix.m[2][3] = 0.0f;

	rotationMatrix.m[3][0] = 0.0f;
	rotationMatrix.m[3][1] = 0.0f;
	rotationMatrix.m[3][2] = 0.0f;
	rotationMatrix.m[3][3] = 1.0f;

	return rotationMatrix;
}

KamataEngine::Vector3 Transform(const KamataEngine::Vector3& vec, const KamataEngine::Matrix4x4& mat) {
	KamataEngine::Vector3 result;

	result.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + mat.m[3][0];
	result.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + mat.m[3][1];
	result.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + mat.m[3][2];

	return result;
}

KamataEngine::Vector3 Subtract(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2) {
	KamataEngine::Vector3 subtract;
	subtract.x = (v1.x - v2.x);
	subtract.y = (v1.y - v2.y);
	subtract.z = (v1.z - v2.z);
	return subtract;
}

KamataEngine::Vector3 Normalize(const KamataEngine::Vector3& v) {
	KamataEngine::Vector3 normalize;
	float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	normalize.x = v.x / length;
	normalize.y = v.y / length;
	normalize.z = v.z / length;
	return normalize;
}


KamataEngine::Matrix4x4 MakeScaleMatrix(const KamataEngine::Vector3& scale) {
	KamataEngine::Matrix4x4 m = {};
	m.m[0][0] = scale.x;
	m.m[1][1] = scale.y;
	m.m[2][2] = scale.z;
	m.m[3][3] = 1;
	return m;
}

KamataEngine::Matrix4x4 MakeTranslateMatrix(const KamataEngine::Vector3& translate) {
	KamataEngine::Matrix4x4 m = {};
	m.m[0][0] = 1;
	m.m[1][1] = 1;
	m.m[2][2] = 1;
	m.m[3][3] = 1;
	m.m[3][0] = translate.x;
	m.m[3][1] = translate.y;
	m.m[3][2] = translate.z;
	return m;
}

KamataEngine::Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	KamataEngine::Matrix4x4 m = {};
	m.m[0][0] = 2 / (right - left);
	m.m[1][1] = 2 / (top - bottom);
	m.m[2][2] = 1 / (farClip - nearClip);
	m.m[3][0] = -(right + left) / (right - left);
	m.m[3][1] = -(top + bottom) / (top - bottom);
	m.m[3][2] = -nearClip / (farClip - nearClip);
	m.m[3][3] = 1;
	return m;
}

KamataEngine::Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	KamataEngine::Matrix4x4 m = {};
	m.m[0][0] = width / 2;
	m.m[1][1] = -height / 2;
	m.m[2][2] = maxDepth - minDepth;
	m.m[3][0] = left + width / 2;
	m.m[3][1] = top + height / 2;
	m.m[3][2] = minDepth;
	m.m[3][3] = 1;
	return m;
}

KamataEngine::Matrix4x4 MakePerspectiveMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	KamataEngine::Matrix4x4 m = {};
	float yScale = 1 / std::tan(fovY / 2);
	float xScale = yScale / aspectRatio;
	m.m[0][0] = xScale;
	m.m[1][1] = yScale;
	m.m[2][2] = farClip / (farClip - nearClip);
	m.m[2][3] = 1;
	m.m[3][2] = -nearClip * farClip / (farClip - nearClip);
	m.m[3][3] = 0;
	return m;
}

KamataEngine::Matrix4x4 Inverse(const KamataEngine::Matrix4x4& m) {
	float A;
	A = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
	    m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] - m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
	    m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +
	    m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
	    m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
	    m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] + m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	KamataEngine::Matrix4x4 m2;

	m2.m[0][0] = 1 / A *
	             (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] -
	              m.m[1][1] * m.m[2][3] * m.m[3][2]);

	m2.m[0][1] = 1 / A *
	             (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] +
	              m.m[0][1] * m.m[2][3] * m.m[3][2]);

	m2.m[0][2] = 1 / A *
	             (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] -
	              m.m[0][1] * m.m[1][3] * m.m[3][2]);

	m2.m[0][3] = 1 / A *
	             (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] +
	              m.m[0][1] * m.m[1][3] * m.m[2][2]);

	m2.m[1][0] = 1 / A *
	             (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] +
	              m.m[1][0] * m.m[2][3] * m.m[3][2]);

	m2.m[1][1] = 1 / A *
	             (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] -
	              m.m[0][0] * m.m[2][3] * m.m[3][2]);

	m2.m[1][2] = 1 / A *
	             (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] +
	              m.m[0][0] * m.m[1][3] * m.m[3][2]);

	m2.m[1][3] = 1 / A *
	             (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] -
	              m.m[0][0] * m.m[1][3] * m.m[2][2]);

	m2.m[2][0] = 1 / A *
	             (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] -
	              m.m[1][0] * m.m[2][3] * m.m[3][1]);

	m2.m[2][1] = 1 / A *
	             (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] +
	              m.m[0][0] * m.m[2][3] * m.m[3][1]);

	m2.m[2][2] = 1 / A *
	             (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] -
	              m.m[0][0] * m.m[1][3] * m.m[3][1]);

	m2.m[2][3] = 1 / A *
	             -(m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] +
	               m.m[0][0] * m.m[1][3] * m.m[2][1]);

	m2.m[3][0] = 1 / A *
	             (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] +
	              m.m[1][0] * m.m[2][2] * m.m[3][1]);

	m2.m[3][1] = 1 / A *
	             (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] -
	              m.m[0][0] * m.m[2][2] * m.m[3][1]);

	m2.m[3][2] = 1 / A *
	             (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] +
	              m.m[0][0] * m.m[1][2] * m.m[3][1]);

	m2.m[3][3] = 1 / A *
	             (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] -
	              m.m[0][0] * m.m[1][2] * m.m[2][1]);

	return m2;
}