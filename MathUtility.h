#include "AABB.h"
#include "afine.h"
#include <math/Matrix4x4.h>
#include <math/Vector3.h>

KamataEngine::Vector3& operator+=(KamataEngine::Vector3& lhv, const KamataEngine::Vector3& rhv);

float easeInOutSine(float x1, float x2, float t);

float Lerp(float x1, float x2, float t);

KamataEngine::Vector3 Lerp(const KamataEngine::Vector3& v1, const KamataEngine::Vector3& v2, float t);

KamataEngine::Vector3 TransformNormal(const KamataEngine::Vector3& v, const KamataEngine::Matrix4x4& m);

bool CheckAABBOverlap(const AABB& a, const AABB& b);

float EaseInOut(float x1, float x2, float t);