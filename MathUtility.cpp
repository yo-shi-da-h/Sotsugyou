#include "AABB.h"
#include "MathUtility.h"
#include <cmath>
#include <numbers>

KamataEngine::Vector3& operator+=(KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv) {
	// TODO: return ステートメントをここに挿入します
	lhv.x += rhv.x;
	lhv.y += rhv.y;
	lhv.z += rhv.z;
	return lhv;
}

float easeInOutSine(float x1, float x2, float t) {

	float easeT = -(std::cosf(std::numbers::pi_v<float> * t) - 1.0f) / 2.0f;
	return Lerp(x1, x2, easeT);
}

float Lerp(float x1, float x2, float t) {
	return x1 + t * (x2 - x1); // 修正された補間公式
}

KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t) { return KamataEngine::Vector3(Lerp(v1.x, v2.x, t), Lerp(v1.y, v2.y, t), Lerp(v1.z, v2.z, t)); }
KamataEngine::Vector3 TransformNormal(const KamataEngine::Vector3& v, const KamataEngine::Matrix4x4& m) {
	KamataEngine::Vector3 result{v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0], v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1], v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2]};

	return result;
}

bool CheckAABBOverlap(const AABB& a, const AABB& b) { return (a.min.x <= b.max.x && a.max.x >= b.min.x && a.min.y <= b.max.y && a.max.y >= b.min.y && a.min.z <= b.max.z && a.max.z >= b.min.z); }

float EaseInOut(float x1, float x2, float t) {
	float easedT = -(std::cosf(std::numbers::pi_v<float> * t) - 1.0f) / 2.0f;
	return Lerp(x1, x2, easedT);
}
