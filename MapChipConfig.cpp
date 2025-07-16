#include "MapChipConfig.h"
#include <windows.h>
#include <string>  


MapChipConfig LoadMapChipConfig(const std::string& iniPath)
{
	MapChipConfig cfg;
	cfg.TILE_PIX_SIZE =		 GetPrivateProfileInt("MapChip", "TILE_SIZE",	255, "./mySetup.ini");
	cfg.TILES_X =				 GetPrivateProfileInt("MapChip", "TILES_X",		255, "./mySetup.ini");
	cfg.TILES_Y =				 GetPrivateProfileInt("MapChip", "TILES_Y",		255, "./mySetup.ini");
	cfg.MAPCHIP_VIEW_X =	 GetPrivateProfileInt("MapChip", "VIEW_X",		255, "./mySetup.ini");
	cfg.MAPCHIP_VIEW_Y =	 GetPrivateProfileInt("MapChip", "VIEW_Y",		255, "./mySetup.ini");
	cfg.MAPCHIP_WIN_WIDTH = cfg.TILE_PIX_SIZE * cfg.MAPCHIP_VIEW_X;
	cfg.MAPCHIP_WIN_HEIGHT = cfg.TILE_PIX_SIZE * cfg.MAPCHIP_VIEW_Y;
	return cfg;
}

const MapChipConfig& GetMapChipConfig()
{
	static MapChipConfig config = LoadMapChipConfig("./mySetup.ini");
	return config;
}