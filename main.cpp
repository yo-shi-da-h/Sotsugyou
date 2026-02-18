#include "GameScene.h"
#include "KamataEngine.h"

#include <Windows.h>

using namespace KamataEngine;

// ファイル先頭付近に追加
enum class Scene { Title, Game };

Scene scene = Scene::Title;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// 初期化処理
	//  // エンジンの初期化

	KamataEngine::Initialize(L"LE3D_23_ヨシダ_ハルト");

	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// main関数の前に
	//TitleScene* titleScene = nullptr;
	
	// タイトルシーンの初期化
	/*titleScene = new TitleScene();
	titleScene->Initialize();*/

	GameScene* gameScnce = new GameScene();
	gameScnce->Initialize();

	// メインループ
	while (true) {

		if (KamataEngine::Update()) {
			break;
		}

		dxCommon->PreDraw();

		// シーンごとに処理を分岐
		/*if (scene == Scene::Title) {
			titleScnce->Update();
			titleScnce->Draw();
			if (titleScnce->IsSelectFinished()) {
				scene = Scene::Game;
			}
		} else if (scene == Scene::Game) {
			gameScnce->Update();
			gameScnce->Draw();
		}*/
		gameScnce->Update();
		gameScnce->Draw();
		dxCommon->PostDraw();
	}

	delete gameScnce;
	gameScnce = nullptr;
	/*delete titleScnce;
	titleScnce = nullptr;*/
	// ここでゲームシーンの終了処理を行う

	// 終了処理
	KamataEngine::Finalize();
	return 0;
}