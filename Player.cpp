#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include "MathUtility.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <numbers>

#ifdef DEBUG
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#endif // DEBUG

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* viewProjection, const KamataEngine::Vector3& position) {
	// NULLチェック
	assert(model);

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	worldTransform2_.Initialize();
	worldTransform2_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransform3_.Initialize();
	worldTransform3_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransform4_.Initialize();
	worldTransform4_.scale_ = {0.5f, 0.5f, 0.5f};

	// 引数の内容をメンバ変数に記録
	model_ = model;
	playerCamera_ = viewProjection;
	input_ = KamataEngine::Input::GetInstance();
	audio_ = KamataEngine::Audio::GetInstance();
}

void Player::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	// 内部処理（カプセル化されたprivate関数）を順番に呼び出し
	MovePlayer();

	worldTransform2_.scale_ = {0.1f, 0.1f, 0.1f};
	worldTransform3_.scale_ = {0.1f, 0.1f, 0.1f};
	worldTransform3_.rotation_.y += 0.1f;
	worldTransform4_.scale_ = {0.1f, 0.1f, 0.1f};
	worldTransform4_.rotation_.y += 0.1f;

	worldTransform2_.translation_ = worldTransform_.translation_;
	worldTransform2_.translation_.y += 1;
	worldTransform3_.translation_ = worldTransform_.translation_;
	worldTransform3_.translation_.y += 2;
	worldTransform4_.translation_ = worldTransform_.translation_;
	worldTransform4_.translation_.y += 3;

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.movement = velocity_;
	collisionMapInfo.landingFlag = false;
	collisionMapInfo.wallContactFlag = false;

	// マップ衝突チェック
	CheckMapCollision(collisionMapInfo);

	JudgmentMove(collisionMapInfo);
	CeilingContact(collisionMapInfo);
	GraundSetting(collisionMapInfo);
	TurnControll();

	// 行列計算
	worldTransform_.UpdateMatarix();

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_);

	MapChipType mapChipType;
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	worldTransform2_.UpdateMatarix();
	worldTransform3_.UpdateMatarix();
	worldTransform4_.UpdateMatarix();

#ifdef DEBUG
	ImGui::Begin("a");
	// パブリック変数からプライベート変数（末尾アンダースコア）に変更したものを参照
	ImGui::InputInt("2", &switchNumber_);
	ImGui::End();
#endif // DEBUG
}

void Player::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *playerCamera_);

	// 隠蔽したメンバ変数名（末尾アンダースコア）に合わせて修正
	if (isFrontDoor_ == true) {
		model2_->Draw(worldTransform2_, *playerCamera_);
	}
	if (isFrontDoor1_ == true) {
		model2_->Draw(worldTransform2_, *playerCamera_);
	}
}

KamataEngine::Vector3 Player::GetWorldPosition() {
	KamataEngine::Vector3 worldPos;
	// ワールド行列から平行移動成分を取り出す
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}

// AABBを安全に取得する関数の実体
AABB Player::GetAABB() {
	AABB aabb;
	KamataEngine::Vector3 pos = worldTransform_.translation_;
	aabb.min = { pos.x - kWidth / 2.0f, pos.y - kHeight / 2.0f, pos.z - 0.5f };
	aabb.max = { pos.x + kWidth / 2.0f, pos.y + kHeight / 2.0f, pos.z + 0.5f };
	return aabb;
}

// 外部から安全に呼び出される死亡フラグ変更処理
bool Player::Dead() {
	isDead_ = true;
	return false;
}

void Player::UpdateBlockAndTrapPosition(KamataEngine::WorldTransform& worldTransform, MapChipField* mapChipField, float fallSpeed) {
	// 必要に応じて実装
}

// --- これ以降は外部から直接呼び出せない private な関数群 ---

void Player::MovePlayer() {
	// 落下中の死亡チェック
	const float kDeathHeight = 0.0f; // 死亡する高さ

	// 接地状態
	if (onGround_) {
		// 左右移動操作
		if (input_->PushKey(DIK_D) || input_->PushKey(DIK_A)) {
			KamataEngine::Vector3 acceleration = {};
			if (input_->PushKey(DIK_D)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (input_->PushKey(DIK_A)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			velocity_.x += acceleration.x;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (input_->PushKey(DIK_W)) {
			velocity_.y += kJumpAcceleration;
		}
	} else {
		// 空中状態
		velocity_.y += -kGravityAcceleration;
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);

		if (worldTransform_.translation_.y < kDeathHeight) {
			isDead_ = true;
			std::cout << "Player has fallen and died." << std::endl;
		}
	}

	if (!onGround_) {
		if (velocity_.x >= 0.1f) {
			velocity_.x = 0.1f;
		}
		if (velocity_.x <= -0.1f) {
			velocity_.x = -0.1f;
		}
		if (input_->PushKey(DIK_D) || input_->PushKey(DIK_A)) {
			KamataEngine::Vector3 acceleration = {};
			if (input_->PushKey(DIK_D)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kLimitRunSpeed;
				}
			} else if (input_->PushKey(DIK_A)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kLimitRunSpeed;
				}
			}
			velocity_.x += acceleration.x;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		}
	}
}

void Player::CeilingContact(const CollisionMapInfo& info) {
	if (info.hitCeilingFlag) {
		KamataEngine::DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	if (info.movement.y <= 0) {
		return;
	}

	std::array<KamataEngine::Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + KamataEngine::Vector3(0, +kHeight / 3.0f, 0));

		if (indexSetNow.yIndex != indexSet.yIndex) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.movement + KamataEngine::Vector3(0, +kHeight / 3.0f, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.movement.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 3.0f + kBlank));
			info.hitCeilingFlag = true;
		}
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
	if (info.movement.y >= 0) {
		return;
	}

	std::array<KamataEngine::Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + KamataEngine::Vector3(0, info.movement.y, 0), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + KamataEngine::Vector3(0, -kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.movement + KamataEngine::Vector3(0, -kHeight / 2.0f, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.movement.y = std::min(0.0f, (rect.top - worldTransform_.translation_.y) + ((kHeight / 2.0f) + kBlank));
			info.landingFlag = true;
		}
	}
}

void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	std::array<KamataEngine::Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + KamataEngine::Vector3(info.movement.x, 0, 0), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = std::max(0.0f, (rect.right - worldTransform_.translation_.x) - (kWidth / 2.0f + kBlank));
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	std::array<KamataEngine::Vector3, kNumCorner> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + KamataEngine::Vector3(info.movement.x, 0, 0), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;

	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = std::min(0.0f, (rect.left - worldTransform_.translation_.x) + (kWidth / 2.0f + kBlank));
	}
}

void Player::GraundSetting(const CollisionMapInfo& info) {
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {
			std::array<KamataEngine::Vector3, kNumCorner> positonsNew;
			for (uint32_t i = 0; i < positonsNew.size(); ++i) {
				positonsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
			}

			MapChipType mapChipType;
			bool hit = false;

			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kLeftBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kRightBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			if (!hit) {
				onGround_ = false;
			}
		}
	} else {
		if (info.landingFlag) {
			velocity_.x *= (1.0f - kAttenuationLanding);
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}
}

void Player::JudgmentMove(const CollisionMapInfo& info) {
	worldTransform_.translation_.x += info.movement.x;
	worldTransform_.translation_.y += info.movement.y;
	worldTransform_.translation_.z += info.movement.z;
}

void Player::TurnControll() {
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}
}

KamataEngine::Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Corner corner) {
	KamataEngine::Vector3 offsetTable[kNumCorner] = {
		{+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
		{-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
		{+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
		{-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}