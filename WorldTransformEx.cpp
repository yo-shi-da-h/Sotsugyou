#include "3d/WorldTransform.h"
#include "afine.h"
#include <KamataEngine.h>

void KamataEngine::WorldTransform::UpdateMatarix() {
	// スケール、回転、平行移動を合成して行列を計算する
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

	if (parent_) {
		matWorld_ *= parent_->matWorld_;
	}

	// 定数バッファに転送する
	TransferMatrix();
}