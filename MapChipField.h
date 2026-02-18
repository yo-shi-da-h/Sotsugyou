#include <3d/Camera.h>
#include <3d/Model.h>
#include <3d/WorldTransform.h>
#include <math/Vector3.h>
#include <string>
#include <vector>

enum class MapChipType {
	kBlank,  // 空白
	kBlock,  // ブロック
	

};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {

public:
	~MapChipField();

	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	// 範囲矩形
	struct Rect {
		float left;
		float right;
		float top;
		float bottom;
	};

	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData_;

	void LoadMapChipCsv(const std::string& filePath);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	void DeleteMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	uint32_t GetNumBlockVirtical() const;
	uint32_t GetNumBlockHorizontal() const;

	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	void SetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex, MapChipType type);


private:
	void ResetMapChipData();

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;
	KamataEngine::Camera* viewProjection_ = nullptr;
	// モデル
	KamataEngine::Model* model_ = nullptr;
};
