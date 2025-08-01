#pragma once
#include "Library\GameObject.h"
#include <vector>
#include "globals.h"
#include "MapEditConfig.h"

namespace
{
	const int MAP_WIDTH = { 20 };
	const int MAP_HEIGHT = { 20 };
	const int MAP_IMAGE_SIZE = { 32 }; // 画像のサイズ
	const int LEFT_MARGIN = { 100 };
	const int TOP_MARGIN = { 40 };
}



class MapEdit :
    public GameObject
{
public:
	MapEdit();
	~MapEdit();
	
	void SetMap(Point p, int value);
	int GetMap(Point p) const;
	bool IsInMapEditArea() const { return isInMapEditArea_; } //マップエディタ領域内にいるかどうかを取得する

	void Update() override;
	void Draw() override;
	void SaveMapData();
	void LoadMapData();
private:
	std::vector<int> myMap_; //マップの配列
	Rect mapEditRect_; //マップ領域の矩形
	Rect drawAreaRect_; //描画領域の矩形
	bool isInMapEditArea_; //マップエディタ領域内にいるかどうか
};