//
//  MapChipTest.h
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#ifndef KLibSample_MapChipTest_h
#define KLibSample_MapChipTest_h

#include "KLObj.h"
#include "KLSprite.h"

#define MAPCHIPW (32)
#define MAPCHIPH (32)

typedef struct{
	
	KLObj	core;
	
	
	// KLibSceneは構造の先頭がKLObjのヒープをワークサイズと合わせて確保する
	// そのため任意構造体にキャストする事で予め確保されたヒープを使う事ができる
	KLSprite	sprite;
	
}MapChipTest;

void MapChipTest_Init( KLObj* pObj, void* pParam );

#endif
