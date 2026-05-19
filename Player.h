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
#include "Character.h"

class MapChipField;

class Player : public Character{
public:
	// 左右（外部の描画クラス等で使う可能性があるならpublicでOK）
	enum class LRDirection { kRight, kLeft };

	// 外部から呼び出す基本関数
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* viewProjection, const KamataEngine::Vector3& position)override;
	void Update()override;
	void Draw()override;

	// 外部に安全に公開するGetter / Setter
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }
	const KamataEngine::Vector3& GetPlayerPosition() const { return worldTransform_.translation_; }
	KamataEngine::Vector3 GetWorldPosition();
	AABB GetAABB();

	// 状態のGetter
	bool IsDead() const { return isDead_; }
	bool IsClear() const { return isClear_; }
	bool IsDoorInteract() const { return isDoorInteract_; }
	int GetItemCount() const { return itemCount_; }
	int GetItemBatteryCount() const { return itemBatteryCount_; }

	// 外部から状態を変化させるための適切な関数
	void AddItem() { itemCount_++; }
	void AddBattery() { itemBatteryCount_++; }
	bool Dead();

private:
	// 内部だけで使う角の定義
	enum Corner {
		kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner
	};

	// 内部だけで使う衝突用構造体
	struct CollisionMapInfo {
		bool hitCeilingFlag = false;
		bool landingFlag = false;
		bool wallContactFlag = false;
		KamataEngine::Vector3 movement;
	};

	// 内部の更新用サブ関数（すべてprivateに隠蔽）
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
	void UpdateBlockAndTrapPosition(KamataEngine::WorldTransform& worldTransform, MapChipField* mapChipField, float fallSpeed);

private:
	// メンバ変数はすべてprivate
	KamataEngine::Vector3 worldPos_;
	float radius_;
	bool onGround_ = true;
	bool isDead_ = false;
	bool isClear_ = false;
	bool isDoorInteract_ = false;

	int itemCount_ = 0;
	int itemBatteryCount_ = 0;
	int switchNumber_ = 0;
	int GetSwitchNumber() const { return switchNumber_; }
	bool isFrontDoor_ = false;
	bool isFrontDoor1_ = false;

	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;

	// ワールド変換データ・モデル等
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::WorldTransform worldTransform2_;
	KamataEngine::WorldTransform worldTransform3_;
	KamataEngine::WorldTransform worldTransform4_;
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Model* model2_ = nullptr;
	KamataEngine::Camera* playerCamera_ = nullptr;

	KamataEngine::Vector3 velocity_ = {};
	LRDirection lrDirection_ = LRDirection::kRight;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	bool isItemActive_ = false;
	float deltaTime = 0.0f;
	MapChipField* mapChipField_ = nullptr;

	// 音声
	uint32_t soundSE_ = 0;
	uint32_t soundSEHanlde_ = 0;
	bool soundFlag = false;
	uint32_t shotSE_ = 0;
	uint32_t shotHandle_ = 0;

	// 定数群
	static inline const float kAcceleration = 1.0f;
	static inline const float kAttenuation = 0.2f;
	static inline const float kJumpAcceleration = 0.9f;
	static inline const float kGravityAcceleration = 0.04f;
	static inline const float kAttenuationWall = 0.2f;
	static inline const float kAttenuationLanding = 0.7f;
	static inline const float kLimitFallSpeed = 1.0f;
	static inline const float kLimitRunSpeed = 0.5f;
	static inline const float kTimeTurn = 0.5f;
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 1.0f;
	static inline const float kBlank = 2.0f;
	static inline const float kGroundSearchHeight = 0.0f;
};