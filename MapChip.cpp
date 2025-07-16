#define NOMINMAX
#include "MapChip.h"
#include "Source\Screen.h"
#include "Input.h"
#include "ImGui/imgui.h"
#include <algorithm>

namespace
{
	//同じ名前の変数はあとでまとめましょう
	//const int IMAGE_SIZE = { 32 }; // 画像のサイズ
	//const int MAP_CHIP_WIDTH = { 16 };//チップの横並び数
	//const int MAP_CHIP_HEIGHT = { 12 };//チップの縦並び数
	//const int MAP_CHIP_NUM_X = { 8 };//マップチップウィンドウの横並び数
	//const int MAP_CHIP_NUM_Y = { 24 };//マップチップウィンドウの縦並び数
	//const int MAP_CHIP_WIN_WIDTH = { IMAGE_SIZE * MAP_CHIP_NUM_X };//ウィンドウの横幅
	//const int MAP_CHIP_WIN_HEIGHT = { IMAGE_SIZE * MAP_CHIP_NUM_Y };//ウィンドウの縦幅

	//// ドラッグ開始と判定する移動量の閾値
	//const int DRAG_THRESHOLD = 5;








}

MapChip::MapChip()
	: GameObject(),cfg_(GetMapChipConfig()), isUpdate_(false), isInMapChipArea_(false), selectedIndex_(-1)
	, bgHandle(cfg_.TILES_X* cfg_.TILES_Y, -1), selected_({ 0,0 }), isHold_(false), ScrollOffset_({ 0,0 })
{
	LoadDivGraph("./bg.png", cfg_.TILES_X * cfg_.TILES_Y,
		cfg_.TILES_X, cfg_.TILES_Y,
		cfg_.TILE_PIX_SIZE, cfg_.TILE_PIX_SIZE, bgHandle.data());
	

	//LUT(Look Up Table) 作成
	for (int i = 0; i < bgHandle.size(); i++)
	{
		HandleToIndex[bgHandle[i]] = i;
	}
}


MapChip::~MapChip()
{
	for (int i = 0; i < cfg_.TILES_X * cfg_.TILES_Y; i++) {
		if (bgHandle[i] != -1) {
			DeleteGraph(bgHandle[i]);
			bgHandle[i] = -1;
		}
	}
}

Point MapChip::GetViewOrigin() const
{
	//int MAP_CHIP_WIN_WIDTH = cfg_.MAPCHIP_WIN_WIDTH;
	//int MAP_CHIP_WIN_HEIGHT = cfg_.MAPCHIP_WIN_HEIGHT;
	return { Screen::WIDTH - cfg_.MAPCHIP_WIN_WIDTH,0 };
}


bool MapChip::IsInChipArea(const Point& mouse) const
{
	return  (mouse.x > GetViewOrigin().x && mouse.x < Screen::WIDTH
		&& mouse.y > GetViewOrigin().y && mouse.y < Screen::HEIGHT);
}

Point MapChip::ScreenToChipIndex(const Point& mouse) const
{
	int localX = (mouse.x - GetViewOrigin().x) / cfg_.TILE_PIX_SIZE;
	int localY = mouse.y / cfg_.TILE_PIX_SIZE;
	return Point();
}

void MapChip::Update()
{
	Point mousePos;
	if (GetMousePoint(&mousePos.x, &mousePos.y) == -1) {
		return;
	}

	isInMapChipArea_ = IsInChipArea(mousePos);

	if (isInMapChipArea_) {
		if (Input::IsButtonDown(KEY_INPUT_LEFT))
			ScrollOffset_.x = std::min(std::max(0, cfg_.TILES_X - cfg_.MAPCHIP_VIEW_X), ScrollOffset_.x);
			//ScrollOffset_.x = ScrollOffset_.x + 1;
		if (Input::IsButtonDown(KEY_INPUT_RIGHT))
			ScrollOffset_.x = std::max(0,ScrollOffset_.x - 1);
			//ScrollOffset_.x = ScrollOffset_.x - 1;

		selected_ = ScreenToChipIndex(mousePos);
		int index = selected_.y * cfg_.TILES_X + selected_.x;

		if (Input::IsButtonDown(MOUSE_INPUT_LEFT))
		{
			isHold_ = true;
			selectedIndex_ = bgHandle[index];
		}
	}
	else
	{
		isInMapChipArea_ = false;
	}
}

void MapChip::Draw()
{
	//マップチップ領域表示
	for (int j = 0; j < cfg_.TILES_Y; j++) {
		for (int i = 0; i < cfg_.TILES_X; i++) {
			int index = i + ScrollOffset_.x + j * cfg_.TILES_X;
			if (index < 0 || index >= bgHandle.size()) continue; //範囲外チェック
			DrawGraph(GetViewOrigin().x + i * cfg_.TILE_PIX_SIZE, 
							   GetViewOrigin().y + j * cfg_.TILE_PIX_SIZE,
							   bgHandle[index], TRUE);
		}
	}

	//マップチップの選択範囲をハイライト表示
	if (isInMapChipArea_)
	{
		int px = GetViewOrigin().x + selected_.x * cfg_.TILE_PIX_SIZE;
		int py = selected_.y * cfg_.TILE_PIX_SIZE;
	
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		DrawBox(px, py ,
			px + cfg_.TILE_PIX_SIZE , py + cfg_.TILE_PIX_SIZE,
			GetColor(255, 255, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawBox(px, py, px + cfg_.TILE_PIX_SIZE, py + cfg_.TILE_PIX_SIZE,
			GetColor(255, 0, 0), FALSE, 2);
	}

	//ホールド中のマップチップを表示
	if (isHold_)
	{
		Point mousePos;
		if (GetMousePoint(&mousePos.x, &mousePos.y) != -1)
		{
			DrawExtendGraph(mousePos.x, mousePos.y,
				mousePos.x + cfg_.TILE_PIX_SIZE, mousePos.y + cfg_.TILE_PIX_SIZE, selectedIndex_, TRUE);
		}
		if (Input::IsButtonUP(MOUSE_INPUT_RIGHT))
		{
			isHold_ = false; //マウスのボタンが離されたら持っている状態を解除
			selectedIndex_ = -1; //選択したインデックスをリセット
		}
	}

}

bool MapChip::IsHold()
{
	return isHold_;
}

int MapChip::GetHoldImage()
{
	if (isHold_)
	{
		return selectedIndex_;
	}
	else
	{
		return -1; //持っていない場合は-1を返す
	}
}

int MapChip::GetChipIndex(int handle)
{
	return HandleToIndex[handle];
}


