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
	
	// textureHandle_ = textureHandle;
	playerCamera_ = viewProjection;
	input_ = KamataEngine::Input::GetInstance();
	audio_ = KamataEngine::Audio::GetInstance();

	/*soundSE_ = audio_->LoadWave("Audio/ItemGet.wav");
	shotSE_ = audio_->LoadWave("Audio/ShotSE.wav");*/
}

void Player::Update() {

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	MovePlayer();

	worldTransform2_.scale_ = {0.1f, 0.1f, 0.1f};

	worldTransform3_.scale_ = {0.1f, 0.1f, 0.1f};
	worldTransform3_.rotation_.y += 0.1f;
	worldTransform4_.scale_ = {0.1f, 0.1f, 0.1f};
	worldTransform4_.rotation_.y += 0.1f;

	// if (itemCount >= 10) {
	worldTransform2_.translation_ = worldTransform_.translation_;
	worldTransform2_.translation_.y += 1;
	worldTransform3_.translation_ = worldTransform_.translation_;
	worldTransform3_.translation_.y += 2;
	worldTransform4_.translation_ = worldTransform_.translation_;
	worldTransform4_.translation_.y += 3;
	//}

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
	

	//// 音の再生
	//if (soundFlag) {
	//	// 音を再生
	//	soundSEHanlde_ = audio_->PlayWave(soundSE_, false, 1.0f);

	//	// 音を再生したらフラグをリセット
	//	soundFlag = false;
	//}

	/*if (soundFlag == false) {
		audio_->StopWave(soundSE_);
	}*/

	// Rotate();

	worldTransform2_.UpdateMatarix();
	worldTransform3_.UpdateMatarix();
	worldTransform4_.UpdateMatarix();

#ifdef DEBUG

	ImGui::Begin("a");
	ImGui::InputInt("2", &switchNumber_);
	ImGui::End();

#endif // DEBUG

}

void Player::Draw() {

	// 3Dモデルを描画
	model_->Draw(worldTransform_, *playerCamera_);

	if (isFrontDoor == true) {
		model2_->Draw(worldTransform2_, *playerCamera_);
	}
	if (isFrontDoor1 == true) {
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

void Player::MovePlayer() {
	// 落下中の死亡チェック
	const float kDeathHeight = 0.0f; // 死亡する高さ

	// 移動入力
	// 接地状態
	if (onGround_) {
		// 左右移動操作
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_D) || KamataEngine::Input::GetInstance()->PushKey(DIK_A)) {
			// 左右加速
			KamataEngine::Vector3 acceleration = {};
			if (KamataEngine::Input::GetInstance()->PushKey(DIK_D)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (KamataEngine::Input::GetInstance()->PushKey(DIK_A)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			// 加速/減速
			velocity_.x += acceleration.x;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_W)) {
			// ジャンプ初速
			velocity_.y += kJumpAcceleration;
		}
	} else {

		// 落下速度
		velocity_.y += -kGravityAcceleration;

		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);

		// プレイヤーが死亡する高さを下回った場合
		if (worldTransform_.translation_.y < kDeathHeight) {
			// 死亡フラグを立てる
			isDead_ = true;

			// 死亡処理 (例: コンソールに表示)
			std::cout << "Player has fallen and died." << std::endl;

			// リスポーンするなどの処理をここに追加
		}
	}
	if (!onGround_) {
		if (velocity_.x >= 0.1f) {
			velocity_.x = 0.1f;
		}
		if (velocity_.x <= -0.1f) {
			velocity_.x = -0.1f;
		}
		// 左右移動操作
		if (KamataEngine::Input::GetInstance()->PushKey(DIK_D) || KamataEngine::Input::GetInstance()->PushKey(DIK_A)) {
			// 左右加速
			KamataEngine::Vector3 acceleration = {};
			if (KamataEngine::Input::GetInstance()->PushKey(DIK_D)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kLimitRunSpeed;
				}
			} else if (KamataEngine::Input::GetInstance()->PushKey(DIK_A)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kLimitRunSpeed;
				}
			}
			// 加速/減速
			velocity_.x += acceleration.x;
			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		}
	}
}

void Player::CeilingContact(const CollisionMapInfo& info) {
	// 天井に当たった?
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

	// 上昇あり？
	if (info.movement.y <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<KamataEngine::Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;

	// 真上の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);

	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex + 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {

		hit = true;
	}


	// ブロックにヒット
	if (hit) {

		MapChipField::IndexSet indexSetNow;

		// めり込みを排除する方向に移動量を設定する
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

	// 下降アリ？
	if (info.movement.y >= 0) {
		return;
	}
	// 移動後４つの角の座標
	std::array<KamataEngine::Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + KamataEngine::Vector3(0, info.movement.y, 0), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真上の当たり判定を行う
	bool hit = false;

	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);

	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	

	// ブロックにヒット？
	if (hit) {
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + KamataEngine::Vector3(0, -kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// めり込みを排除する方向に移動量を設定する
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.movement + KamataEngine::Vector3(0, -kHeight / 2.0f, 0));
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			info.movement.y = std::min(0.0f, (rect.top - worldTransform_.translation_.y) + ((kHeight / 2.0f) + kBlank));
			// 地面に当たったことを記録する
			info.landingFlag = true;
		}
	}
}

void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	// 移動後の4つの角の座標
	std::array<KamataEngine::Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + KamataEngine::Vector3(info.movement.x, 0, 0), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;

	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	

	// 左上点の判定
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
	// 移動後の4つの角の座標
	std::array<KamataEngine::Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + KamataEngine::Vector3(info.movement.x, 0, 0), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;

	// 右下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	

	// 右上点の判定
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
	// 接地状態の切り替え処理
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態の移行
			onGround_ = false;
		} else {
			std::array<KamataEngine::Vector3, kNumCorner> positonsNew;

			for (uint32_t i = 0; i < positonsNew.size(); ++i) {
				positonsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
			}

			MapChipType mapChipType;
			bool hit = false;

			// 左下点の判定
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kLeftBottom] + KamataEngine::Vector3(0, 0, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positonsNew[kRightBottom] + KamataEngine::Vector3(0, 0, 0));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}

			// 落下開始
			if (!hit) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	} else {
		// 着地
		if (info.landingFlag) {
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuationLanding);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 着地状態に切り替える
			onGround_ = true;
		}
	}
}

void Player::JudgmentMove(const CollisionMapInfo& info) {
	// 移動
	worldTransform_.translation_.x += info.movement.x;
	worldTransform_.translation_.y += info.movement.y;
	worldTransform_.translation_.z += info.movement.z;
}

void Player::TurnControll() {
	// 旋回制御
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定するa
		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
		;
	}
}

KamataEngine::Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Corner corner) {

	KamataEngine::Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kleftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}

bool Player::Dead() {
	isDead_ = true;

	return false;
}