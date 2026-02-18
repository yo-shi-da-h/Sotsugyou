#include "GameScene.h"
#include "3d/AxisIndicator.h"
#include <cassert>

#ifdef DEBUG

#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

#endif // DEBUG


GameScene::GameScene() {}

GameScene::~GameScene() {
	delete modelPlayer_;
	//delete modelEnemy_; // 追加
	delete player_;
	delete modelBlock_;
	delete debugCamera_;
	delete mapChipField_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
	

	}


void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/mapClassroom1.csv");


	player_ = new Player();
	player_->SetMapChipField(mapChipField_);
	// enemy_ = new Enemy();
	// skydome_ = new Skydome();
	//  3Dモデルの生成
	modelPlayer_ = KamataEngine::Model::CreateFromOBJ("player", true);
	modelBlock_ = KamataEngine::Model::CreateFromOBJ("block", true);
	

	// ビュープロジェクションの初期化
	camera_.Initialize();

	// playerPos.z = 0;
	Vector3 playerPos = mapChipField_->GetMapChipPositionByIndex(2, 18);
	player_->Initialize(modelPlayer_, &camera_, playerPos);
	// enemy_->Initialize(modelEnemy_, &camera_, enemyPos);
	// skydome_->Initialize(modelSkydome_, &camera_);

	/*cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->setTarget(player_);
	CameraController::Rect cameraAreaa = {12.0f, 100 - 10.0f, 6.0f, 24.0f};
	cameraController_->SetMovableArea(cameraAreaa);
	cameraController_->Reset();*/
	camera_.translation_ = { 14.0f, 8.0f, -20.0f };
    camera_.rotation_ = { 0.0f, 0.0f, 0.0f };
    camera_.UpdateMatrix();
    camera_.TransferMatrix();
	


	debugCamera_ = new DebugCamera(1280, 720);

	GenerateBlocks();
}


void GameScene::Update() {

	SwitchStage();

	//cameraController_->Update();
	player_->Update();
	//enemy_->Update();
	debugCamera_->Update();
	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			worldTransformBlock->UpdateMatarix();
		}
	}
	
	



#ifdef _DEBUG

	if (input_->TriggerKey(DIK_V)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		camera_.TransferMatrix();

	} /*else {
		cameraController_->Update();
		camera_.matView = cameraController_->GetViewProjection().matView;
		camera_.matProjection = cameraController_->GetViewProjection().matProjection;
	}*/
	camera_.TransferMatrix();
	
	ScrollBlocks();

	RecycleBlockColumn();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	

	Sprite::PostDraw();
	dxCommon_->ClearDepthBuffer();
	KamataEngine::Model::PreDraw();

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる

	player_->Draw();
	//enemy_->Draw();
	//skydome_->Draw();

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	

	KamataEngine::Model::PostDraw();

	Sprite::PreDraw(commandList);

	Sprite::PostDraw();
}

void GameScene::GenerateBlocks() {
	uint32_t kNumBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(kNumBlockVirtical);

	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}


void GameScene::SwitchStage() {

	if (player_->switchNumber_ == 1) {


		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				delete worldTransformBlock;
			}
			worldTransformBlockLine.clear();
		}
		
		worldTransformBlocks_.clear();
		mapChipField_->LoadMapChipCsv("Resources/MapTest.csv");
		GenerateBlocks();

		player_->switchNumber_ = 1;

		if (input_->TriggerKey(DIK_H)) {
			player_->switchNumber_ = 0;
		}
	}

	if (player_->switchNumber_ == 2) {
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				delete worldTransformBlock;
			}
			worldTransformBlockLine.clear();
		}
		
		worldTransformBlocks_.clear();
		mapChipField_->LoadMapChipCsv("Resources/MapTest1.csv");
		GenerateBlocks();

		player_->switchNumber_ = 2;

		if (input_->TriggerKey(DIK_H)) {
			player_->switchNumber_ = 1;
		}
	}

	

}
void GameScene::ScrollBlocks() {
	for (uint32_t i = 0; i < worldTransformBlocks_.size(); i++) {
		for (uint32_t j = 0; j < worldTransformBlocks_[i].size(); j++) {
			if (!worldTransformBlocks_[i][j]) continue;

			worldTransformBlocks_[i][j]->translation_.x -= kScrollSpeed;
			worldTransformBlocks_[i][j]->UpdateMatarix();
		}
	}
}

void GameScene::RecycleBlockColumn() {

	// 一番左のXを取得
	float leftMostX = FLT_MAX;
	
	const float kBlockWidth = 1.0f; // ← MapChip1マス分


	for (uint32_t j = 0; j < worldTransformBlocks_[0].size(); j++) {
		for (uint32_t i = 0; i < worldTransformBlocks_.size(); i++) {
			if (worldTransformBlocks_[i][j]) {
				float x = worldTransformBlocks_[i][j]->translation_.x;
				if (x < leftMostX) {
					leftMostX = x;
					leftColumn = j;
				}
			}
		}
	}

	// 画面外に出たら
	if (leftMostX < -kBlockWidth) {

		// 一番右のXを探す
		float rightMostX = -FLT_MAX;
		for (uint32_t j = 0; j < worldTransformBlocks_[0].size(); j++) {
			for (uint32_t i = 0; i < worldTransformBlocks_.size(); i++) {
				if (worldTransformBlocks_[i][j]) {
					rightMostX = max(rightMostX,
						worldTransformBlocks_[i][j]->translation_.x);
				}
			}
		}

		// 左端列を右端へ移動
		for (uint32_t i = 0; i < worldTransformBlocks_.size(); i++) {
			if (worldTransformBlocks_[i][leftColumn]) {
				worldTransformBlocks_[i][leftColumn]->translation_.x =
					rightMostX + kBlockWidth;
				worldTransformBlocks_[i][leftColumn]->UpdateMatarix();
			}
		}
	}
}
