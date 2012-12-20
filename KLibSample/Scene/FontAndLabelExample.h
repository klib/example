//
//  FontAndLabel.h
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#ifndef KLibSample_FontAndLabel_h
#define KLibSample_FontAndLabel_h

#include "KLObj.h"
#include "KLFont.h"
#include "KLLabel.h"

enum{
	
	LABEL_ARIAL,
	LABEL_COURIER,
	LABEL_HIRAGINO3,
	LABEL_GEORGIA,
	LABEL_HIRAGINO6,
	LABEL_MAX,
	
};

typedef struct{
	
	KLObj	core;
	
	
	// KLibSceneは構造の先頭がKLObjのヒープをワークサイズと合わせて確保する
	// そのため任意構造体にキャストする事で予め確保されたヒープを使う事ができる
	KLFont	font;
	KLLabel	label[LABEL_MAX];
	u32		clen;
	s16		fontsize, scaledir;
	u8		wait;
	
}FontAndLabel;

void FontAndLabel_Init( KLObj* pObj, void* pParam );

#endif
