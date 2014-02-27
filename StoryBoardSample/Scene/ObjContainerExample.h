//
//  ObjContainer.h
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#ifndef KLibSample_ObjContainer_h
#define KLibSample_ObjContainer_h

#include "KLObj.h"
#include "KLSprite.h"
#include "KLLabel.h"

#define EXAMPLE_CHILD_MAX (8)
#define EXAMPLE_PARENTS_NUM (3)

#define EVENTID_DESTROYALL (0)
#define EVENTID_TOUCHED_RECT (1)

/// 子オブジェクト
typedef struct{
	
	KLObj	core;
	
	
	// 基本的にシーンオブジェクトと同じで
	// Containerで予め確保したヒープをキャストして使用する
	KLSprite*	pSprite;	///< 同じテクスチャを使うなら全体で１つだけ確保してそれをコピーか参照するほうが良い
	s32			x,y;
	
}ChildObj;


/// 親オブジェクト
typedef struct{
	
	KLObj	core;
	
	
	// 基本的にシーンオブジェクトと同じで
	// Containerで予め確保したヒープをキャストして使用する
	KLContainer	children;	///< 親オブジェクト サンプル用にいくつか
	KLSprite*	pSprite;	///< スプライト
	s32			x,y;
	wdeg		r;			///< 回転角度
	u8			id;			///< 親ID
	
}ParentObj;


typedef struct{
	
	KLObj	core;
	
	
	// KLibSceneは構造の先頭がKLObjのヒープをワークサイズと合わせて確保する
	// そのため任意構造体にキャストする事で予め確保されたヒープを使う事ができる
	KLContainer	parents;	///< 親オブジェクト サンプル用にいくつか
	KLLabel		label;		///< 説明用ラベル
	KLSprite	spParent, spChild;
	
}ObjContainer;

void ObjContainer_Init( KLObj* pObj, void* pParam );

#endif
