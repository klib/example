//
//  FileEncWithKbd.h
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#ifndef KLibSample_FileEncWithKbd_h
#define KLibSample_FileEncWithKbd_h

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
	
	char* pStr;
	
	
}FileEncWithKbd;

void FileEncWithKbd_Init( KLObj* pObj, void* pParam );


#endif
