//
//  FirstScene.h
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#ifndef KLibSample_FirstScene_h
#define KLibSample_FirstScene_h

#include "KLObj.h"
#include "KLLabel.h"
#include "KLFont.h"
#include "KLLogo.h"
#include "KLString.h"
#include "KLSprite.h"

// シーンオブジェクト
typedef struct{
	
	KLObj	core;
	
	
	// KLibSceneは構造の先頭がKLObjのヒープをワークサイズと合わせて確保する
	// そのため任意構造体にキャストする事で予め確保されたヒープを使う事ができる
	KLObjFunc pNextSceneInitFunc; ///< フェードアウト後の次のシーン設定用ポインタ
	KLSprite spLogo;
	
}FirstScene;

void FirstScene_Init( KLObj* pObj, void* pParam );


#endif
