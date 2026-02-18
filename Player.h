#pragma once
#include "AABB.h"
#include "afine.h"
#include <2d/DebugText.h>
#include <3d/Camera.h>
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include <audio/Audio.h>
#include <cassert>
#include <input/Input.h>
#include <iostream>
#include <list>
#include <numbers>

class MapChipField;

class Player {
public:
	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};

	// 角
	struct CollisionMapInfo {
		bool hitCeilingFlag = false;
		bool landingFlag = false;
		bool wallContactFlag = false;
		KamataEngine::Vector3 movement;
	};

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* viewProjection, const KamataEngine::Vector3& position);

	void Update();

	void Draw();

	void UpdateBlockAndTrapPosition(KamataEngine::WorldTransform& worldTransform, MapChipField* mapChipField, float fallSpeed);


	// void Rotate();

	// seteer
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	// getter
	const KamataEngine::WorldTransform& GetWorldTransform() { return worldTransform_; }
	KamataEngine::Vector3& GetVelocity() { return velocity_; }

	const KamataEngine::Vector3& GetPlayerPosition() const { return worldTransform_.translation_; }

	// 2-10
	//   ワールド座標を取得する関数
	KamataEngine::Vector3 GetWorldPosition();
	// AABBを取得する関数
	AABB GetAABB();

	void MovePlayer();

	void CheckMapCollision(CollisionMapInfo& info);
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);

	void JudgmentMove(const CollisionMapInfo& info);

	void CeilingContact(const CollisionMapInfo& info);

	void GraundSetting(const CollisionMapInfo& info);

	void TurnControll();

	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

	bool Dead();

	// デスフラグのgetter
	bool IsDead() const { return isDead_; }

	// クリアフラグのgetter
	bool IsClear() const { return isClear_; }

	bool IsDoorInteract() const { return isDoorInteract_; }

	int itemCount = 0;
	int itemBateryCount = 0;

	int switchNumber_ = 0;

	bool isFrontDoor = false;
	bool isFrontDoor1 = false;

private:
	KamataEngine::Vector3 worldPos_;
	float radius_;

	bool onGround_ = true;
	// bool TamaFlag_ = false;

	bool isDead_ = false;
	bool isClear_ = false;
	bool isDoorInteract_=false;

	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform worldTransform2_;
	KamataEngine::WorldTransform worldTransform3_;
	KamataEngine::WorldTransform worldTransform4_;

	// モデル
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* model2_ = nullptr;//interact
	KamataEngine::Camera* playerCamera_ = nullptr;

	// KamataEngine::Model* bulletmodel_ = nullptr;
	//CameraController* camera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {};

	LRDirection lrDirection_ = LRDirection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;

	bool isItemActive_ = false;
	float deltaTime = 0.0f;

	MapChipField* mapChipField_ = nullptr;

	uint32_t soundSE_ = 0;
	uint32_t soundSEHanlde_ = 0;
	bool soundFlag = false;

	uint32_t shotSE_ = 0;
	uint32_t shotHandle_ = 0;

	// プレイヤーの移動加速度
	static inline const float kAcceleration = 1.0f;
	// プレイヤーが停止する際の減速率
	static inline const float kAttenuation = 0.2f;
	// ジャンプ時の加速度
	static inline const float kJumpAcceleration = 0.9f;
	// 重力による加速度
	static inline const float kGravityAcceleration = 0.04f;
	// 壁に衝突した際の減速率
	static inline const float kAttenuationWall = 0.2f;
	// 着地時の減速率
	static inline const float kAttenuationLanding = 0.7f;
	// 落下速度の制限値
	static inline const float kLimitFallSpeed = 1.0f;
	// 走行速度の制限値
	static inline const float kLimitRunSpeed = 0.5f;
	// ターンするのにかかる時間
	static inline const float kTimeTurn = 0.5f;
	// プレイヤーの幅
	static inline const float kWidth = 0.8f;
	// プレイヤーの高さ
	static inline const float kHeight = 1.0f;
	// 隙間の幅（適切な値に修正する必要あり）
	static inline const float kBlank = 2.0f;
	// 地面を探す際の高さ
	static inline const float kGroundSearchHeight = 0.0f;
};