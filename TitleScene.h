#pragma once
#include "input/Input.h"
#include <2d/Sprite.h>
#include <3d/Camera.h>
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include <KamataEngine.h>
#include <audio/Audio.h>

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene {

public:
	~TitleScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	bool IsSelectFinished() const { return isFinished_; }
	bool IsGameFinished() const { return isGameFinished_; }

	// 現在表示されているスプライトを管理する変数
	size_t currentSpriteIndex = 0;
	std::vector<KamataEngine::Sprite*> sprites;

	// スプライトの初期化メソッド
	void InitializeSprites();

private:
	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = 5.0f;
	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = -5.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalklMotionTime = 1.0f;
	// タイマー
	float Timer_ = 0.0f;

	bool isFinished_ = false;
	bool isGameFinished_ = false;
	KamataEngine::DirectXCommon* dxCommon_ = nullptr;
	KamataEngine::WorldTransform titleWorldTransform_;
	KamataEngine::WorldTransform titleWorldTransformFont_;
	KamataEngine::WorldTransform titleskydome;
	KamataEngine::Camera Camera_;
	KamataEngine::Model* titlemodel_ = nullptr;
	KamataEngine::Model* titlemodelFont_ = nullptr;
	KamataEngine::Model* TitleSkydome_ = nullptr;
	uint32_t textureHandle_ = 0;
	KamataEngine::Sprite* sprite_ = nullptr;
	uint32_t textureHandle2_ = 0;
	KamataEngine::Sprite* sprite2_ = nullptr;
	uint32_t textureHandle3_ = 0;
	KamataEngine::Sprite* sprite3_ = nullptr;
	uint32_t textureHandle4_ = 0;
	KamataEngine::Sprite* sprite4_ = nullptr;

	KamataEngine::Model* modelSkydome_ = nullptr;

	KamataEngine::Input* input_ = nullptr;
	KamataEngine::Audio* audio_ = nullptr;

	uint32_t TitleSEHandle_ = 0;
	uint32_t TitleSEHandle2_ = 0;
	uint32_t TitleSEHandle3_ = 0;
	uint32_t voiceHandle_ = 0;
	uint32_t voiceHandle2_ = 0;
};