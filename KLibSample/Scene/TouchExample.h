//
//  TouchTest.h
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#ifndef KLibSample_TouchTest_h
#define KLibSample_TouchTest_h

#include "KLObj.h"
#include "KLLabel.h"
#include "KLFont.h"
#include "KLTouch.h"

typedef struct{
	
	KLObj	core;
	
	
	// KLibSceneは構造の先頭がKLObjのヒープをワークサイズと合わせて確保する
	// そのため任意構造体にキャストする事で予め確保されたヒープを使う事ができる
	KLLabel label;
	
}TouchTest;

void TouchTest_Init( KLObj* pObj, void* pParam );

#endif
