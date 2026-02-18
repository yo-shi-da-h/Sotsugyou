#pragma once
#include <math/Vector3.h>

struct AABB {
	KamataEngine::Vector3 min; // AABBの最小座標
	KamataEngine::Vector3 max; // AABBの最大座標
};

// 衝突判定関数
bool IsCollisionAABB(const AABB& aabb1, const AABB& aabb2);