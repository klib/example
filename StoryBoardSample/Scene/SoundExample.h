//
//  SoundTest.h
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#ifndef KLibSample_SoundTest_h
#define KLibSample_SoundTest_h

#include "KLObj.h"
#include "KLAL.h"
#include "KLLabel.h"
#include "KLFont.h"

enum{
	SOUND_TEST_WAV,
	SOUND_TEST_MP3,
	SOUND_TEST_OGG,
	SOUND_TEST_MAX
};

typedef struct{
	
	KLObj	core;
	
	
	// KLibSceneは構造の先頭がKLObjのヒープをワークサイズと合わせて確保する
	// そのため任意構造体にキャストする事で予め確保されたヒープを使う事ができる
	KLAL	sounds[SOUND_TEST_MAX];
	KLLabel	label;
	KLFont	font;
	
}SoundTest;

void SoundTest_Init( KLObj* pObj, void* pParam );

#endif
