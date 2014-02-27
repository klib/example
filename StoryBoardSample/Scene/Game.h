//
//  Game.h
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#ifndef KLibSample_Game_h
#define KLibSample_Game_h

#include "KLObj.h"
#include "KLLabel.h"
#include "KLFont.h"
#include "KLString.h"
#include "KLSprite.h"

// 各サンプルシーン共通の設定やバッファ
typedef struct {
	
	KLFont	font;
	KLLabel label;
	bl		loaded;
	
} App;


/// SceneID
enum {
	
	EX_DEVICEINFO,
	EX_FONTS_AND_LABELS,
	EX_PRIMITIVES,
	EX_SPRITES,
	EX_MAPCHIP,
	EX_OBJCONTAINERS,
	EX_TOUCHES,
	EX_SOUNDS,
	EX_HTTP,
	EX_FILENCKBD,
	EX_MAX,
	
};

/// 各シーンの名前と初期化関数
typedef struct{
	
	char*		pName;
	KLObjFunc	func;
	
} SceneInfo;

void APP_Init ( bl force );

extern SceneInfo gSceneInfo[EX_MAX];

#endif
