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

	//int TILE_PIX_SIZE;	// 1タイルのピクセルサイズ
	//int TILES_X;		// マップチップの横並び数
	//int TILES_Y;		// マップチップの縦並び数
	//int MAPCHIP_VIEW_X;	// マップチップウィンドウの横並び数
	//int MAPCHIP_VIEW_Y;	// マップチップウィンドウの縦並び数
	//int MAPCHIP_WIN_WIDTH;
	//int MAPCHIP_WIN_HEIGHT;
}

MapChip::MapChip()
	: GameObject(), cfg_(GetMapChipConfig()), isUpdate_(false), isInMapChipArea_(false), selectedIndex_(-1),
	isHold_(false), bgHandle(cfg_.TILES_X* cfg_.TILES_Y, -1), selected_({ 0,0 }), ScrollOffset_({ 0,0 }) //初期値を-1で16*12の配列を初期化する
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
	return { Screen::WIDTH - cfg_.MAPCHIP_WIN_WIDTH, 0 };
}

bool MapChip::IsInChipArea(const Point& mouse) const
{
	return (mouse.x > GetViewOrigin().x && mouse.x < Screen::WIDTH
		&& mouse.y > GetViewOrigin().y && mouse.y < Screen::HEIGHT);
}

Point MapChip::ScreenToChipIndex(const Point& mouse) const
{
	int localX = (mouse.x - GetViewOrigin().x) / cfg_.TILE_PIX_SIZE;
	int localY = mouse.y / cfg_.TILE_PIX_SIZE;
	return { localX, localY };
}

void MapChip::Update()
{
	Point mousePos;
	if (GetMousePoint(&mousePos.x, &mousePos.y) == -1) {
		return;
	}

	isInMapChipArea_ = IsInChipArea(mousePos);


	if (isInMapChipArea_) {
		if (Input::IsKeyDown(KEY_INPUT_LEFT))
		{
			ScrollOffset_.x = std::max(0, ScrollOffset_.x - 1);
		}
		if (Input::IsKeyDown(KEY_INPUT_RIGHT))
		{
			ScrollOffset_.x = std::min(cfg_.TILES_X - cfg_.MAPCHIP_VIEW_X, ScrollOffset_.x + 1);
		}
		if (Input::IsKeyDown(KEY_INPUT_UP))
		{
			ScrollOffset_.y = std::max(0, ScrollOffset_.y - 1);
		}
		if (Input::IsKeyDown(KEY_INPUT_DOWN))
		{
			ScrollOffset_.y = std::min(cfg_.TILES_Y - cfg_.MAPCHIP_VIEW_Y, ScrollOffset_.y + 1);
		}



		int i = (mousePos.x - GetViewOrigin().x) / cfg_.TILE_PIX_SIZE;
		int j = mousePos.y / cfg_.TILE_PIX_SIZE;
		selected_ = ScreenToChipIndex(mousePos);
		int index = (j + ScrollOffset_.y) * 
			cfg_.TILES_X + (i + ScrollOffset_.x);

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
	for (int i = 0; i < cfg_.MAPCHIP_VIEW_X; i++) {
		for (int j = 0; j < cfg_.MAPCHIP_VIEW_Y; j++) {
			int index = (j + ScrollOffset_.y) * cfg_.TILES_X + (i + ScrollOffset_.x);
			if (index < 0 || index >= bgHandle.size())
				continue;
			DrawGraph(
				GetViewOrigin().x + i * cfg_.TILE_PIX_SIZE,
				GetViewOrigin().y + j * cfg_.TILE_PIX_SIZE,
				bgHandle[index],
				TRUE
			);
		}
	}

	//マップチップの選択範囲をハイライト表示
	if (isInMapChipArea_)
	{
		int px = GetViewOrigin().x + selected_.x * cfg_.TILE_PIX_SIZE;
		int py = selected_.y * cfg_.TILE_PIX_SIZE;

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);
		DrawBox(px, py,
			px + cfg_.TILE_PIX_SIZE, py + cfg_.TILE_PIX_SIZE,
			GetColor(255, 255, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawBox(px, py, px + cfg_.TILE_PIX_SIZE, py + cfg_.TILE_PIX_SIZE,
			GetColor(255, 0, 0), FALSE, 2);

	}

	//ホールド中のマップチップを描画
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

bool MapChip::isHold()
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
		return -1; //持ってない場合は-1を返す
	}
}

int MapChip::GetChipIndex(int handle)
{
	return HandleToIndex[handle];
}
