//
//  SpriteTest.h
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#ifndef KLibSample_SpriteTest_h
#define KLibSample_SpriteTest_h

#include "KLObj.h"
#include "KLSprite.h"

enum{
	SPRITE_TEST_BMP,
	SPRITE_TEST_GIF,
	SPRITE_TEST_JPEG,
	SPRITE_TEST_PNG,
	SPRITE_TEST_MAX
};

typedef struct{
	
	KLObj	core;
	
	
	// KLibSceneは構造の先頭がKLObjのヒープをワークサイズと合わせて確保する
	// そのため任意構造体にキャストする事で予め確保されたヒープを使う事ができる
	KLSprite	sprites[SPRITE_TEST_MAX];
	wdeg		rot;
	
}SpriteTest;

void SpriteTest_Init( KLObj* pObj, void* pParam );

#endif
