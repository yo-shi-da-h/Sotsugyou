
#include "TitleScene.h"
#include <numbers>

TitleScene::~TitleScene() {
	delete titlemodel_;
	delete titlemodelFont_;
	delete TitleSkydome_;
	delete sprite_;
	delete sprite2_;
	delete sprite3_;
	delete sprite4_;
}

void TitleScene::Initialize() {
	dxCommon_ = KamataEngine::DirectXCommon::GetInstance();
	input_ = KamataEngine::Input::GetInstance();
	audio_ = KamataEngine::Audio::GetInstance();

	textureHandle_ = KamataEngine::TextureManager::Load("Title/Title1.png");
	sprite_ = KamataEngine::Sprite::Create(textureHandle_, {0, 0});

	textureHandle2_ = KamataEngine::TextureManager::Load("Title/ShotGame.png");
	sprite2_ = KamataEngine::Sprite::Create(textureHandle2_, {0, 0});

	textureHandle3_ = KamataEngine::TextureManager::Load("Title/HitEnter.png");
	sprite3_ = KamataEngine::Sprite::Create(textureHandle3_, {0, 0});

	titleskydome.Initialize();
	Camera_.Initialize();

	Timer_ = 0.0f;

	// タイトルを中央に寄せるために調整
	titleWorldTransform_.translation_ = {0.0f, 0.0f, 0.0f}; // x, y, zの値を調整

	titleWorldTransformFont_.translation_ = {0.0f, 0.0f, 0.0f}; // x, y, zの値を調整

	titleskydome.translation_ = {0.0f, 0.0f, 0.0f};

	// sprite2_の初期位置を画面外（上）に設定
	if (sprite2_) {
		sprite2_->SetPosition({0, -static_cast<float>(1280)}); // 画面上部外へ（高さは適宜調整）
	}

	// スプライトの初期化
	InitializeSprites();
}

void TitleScene::InitializeSprites() {
	sprites.push_back(sprite_);
	sprites.push_back(sprite2_);
	sprites.push_back(sprite3_);
	sprites.push_back(sprite4_);
}

void TitleScene::Update() {
	Timer_ += 1.0f; // フレームごとに加算

	// Enterキーでタイトル終了
	if (input_->TriggerKey(DIK_RETURN) || input_->TriggerKey(DIK_RETURN)) {
		// audio_->PlayWave(TitleSEHandle3_, false); // スタートSE
		isFinished_ = true;
	}

	// 降下アニメーション
	if (sprite2_) {
		auto pos = sprite2_->GetPosition();
		float targetY = 0.0f; // 目的のY座標
		float speed = 20.0f;  // 降りてくる速さ（調整可）

		if (pos.y < targetY) {
			pos.y += speed;
			if (pos.y > targetY)
				pos.y = targetY; // 行き過ぎ防止
			sprite2_->SetPosition(pos);
		}
	}
}

void TitleScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 3Dオブジェクト描画
	KamataEngine::Model::PreDraw();
	KamataEngine::Model::PostDraw();

	// スプライト描画
	KamataEngine::Sprite::PreDraw(commandList);

	// ★ここで背景スプライトを全画面に描画
	// 例: 1280x720の青色背景
	// KamataEngine::Sprite::DrawRect({0, 0}, {1280, 720}, {0.4f, 0.6f, 0.9f, 1.0f}); // RGBA

	// タイトル画像（降下アニメーション位置で表示）
	if (sprite2_) {
		sprite2_->Draw();
	}

	// 「Hit Enter」点滅（30フレームごとにON/OFF）
	if (sprite3_ && static_cast<int>(Timer_) % 60 < 30) {
		sprite3_->SetPosition({0, 0});
		sprite3_->Draw();
	}

	KamataEngine::Sprite::PostDraw();
}
