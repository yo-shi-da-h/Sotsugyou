#pragma once
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include <3d/Camera.h>

// すべてのキャラクターの「親」となるクラス
class Character {
public:
    Character() = default;
    // 【重要】ポリモーフィズムを使う場合、デストラクタは必ず virtual にする
    virtual ~Character() = default;

    // 初期化（共通の処理なら仮想関数にしない、または virtual でオーバーライド可能にする）
    virtual void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position);

    // 【純粋仮想関数】中身は子クラス（PlayerやEnemy）で必ず別々に実装させる
    virtual void Update() = 0; 
    
    // 【仮想関数】基本の処理は書くが、子クラスで上書き（変更）してもいい関数
    virtual void Draw();

    // 共通のゲッター
    const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

protected:
    // 子クラス（Playerなど）から直接アクセスできるようにメンバ変数は protected にする
    KamataEngine::WorldTransform worldTransform_;
    KamataEngine::Model* model_ = nullptr;
    KamataEngine::Camera* camera_ = nullptr;
};
