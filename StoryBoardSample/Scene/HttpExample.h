//
//  HttpExample.h
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#ifndef KLibSample_HttpExample_h
#define KLibSample_HttpExample_h

#include "KLObj.h"
#include "KLLabel.h"
#include "KLFont.h"
#include "KLNetwork.h"

// シーンオブジェクト
typedef struct{
	
	KLObj	core;
	
	
	// KLibSceneは構造の先頭がKLObjのヒープをワークサイズと合わせて確保する
	// そのため任意構造体にキャストする事で予め確保されたヒープを使う事ができる
	KLFont	font;
	KLLabel	label;
	
	KLSprite	sprite;
	
	KLHttpReq*	pCurrentReq;
	KLHttpReq	getReq, postReq;
	
	float	rot;
	
}HttpExample;

void HttpExample_Init( KLObj* pObj, void* pParam );


#endif
