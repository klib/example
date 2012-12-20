//
//  Logo.h
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#ifndef KLibSample_Logo_h
#define KLibSample_Logo_h

#include "KLObj.h"
#include "KLLabel.h"
#include "KLFont.h"

// シーンオブジェクト
typedef struct{
	
	KLObj	core;
	
	
	// KLibSceneは構造の先頭がKLObjのヒープをワークサイズと合わせて確保する
	// そのため任意構造体にキャストする事で予め確保されたヒープを使う事ができる
	KLSprite spLogo;
	u32 wait;
	bl isfade;
	
}Logo;

void Logo_Init( KLObj* pObj, void* pParam );


// フォントやラベルなど全シーン共通で参照する変数をまとめる
typedef struct{

	KLLabel	label;
	KLFont	font;
	bl		loaded;
	
}App;

extern App app;

#endif
