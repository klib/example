//
//  Game.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "KLDrawQueue.h"
#include "KLMath.h"

#include "Game.h"
#include "DeviceExample.h"
#include "FontAndLabelExample.h"
#include "PrimitiveExample.h"
#include "SpriteExample.h"
#include "MapChipExample.h"
#include "ObjContainerExample.h"
#include "TouchExample.h"
#include "SoundExample.h"
#include "HttpExample.h"
#include "FileEncWithKbdExample.h"

// 各シーンのサンプル名と初期化関数
SceneInfo gSceneInfo[EX_MAX] = {
	
	{ "DeviceInfo",		DeviceTest_Init },
	{ "Fonts & Labels",	FontAndLabel_Init },
	{ "Primitives",		PrimitiveTest_Init },
	{ "Sprites",		SpriteTest_Init },
	{ "Mapchip & Clipping",		MapChipTest_Init },
	{ "ObjContainers",	ObjContainer_Init },
	{ "Touches",		TouchTest_Init },
	{ "Sounds",			SoundTest_Init },
	{ "HttpConnection",	HttpExample_Init },
	{ "FileEncrypt & Keyboard",	FileEncWithKbd_Init },
	
};

//===========================================================
/**
 *	@brief	アプリ終了時のアンロードコールバック
 *
 *	@param	pContext	アンロードコンテキスト
 */
//===========================================================
static void APP_Unload( void* pContext ){
	
	
}

//===========================================================
/**
 *	@brief	アプリ初期化
 *
 *	@param	force	強制的に初期化を行う場合はTRUE
 */
//===========================================================
void APP_Init (bl force)
{
	
	// アプリ終了時に他のアンロードがしたい時は以下のようにコールバックを設定する
	KL_SetUnloadCallback( APP_Unload, NULL ); // コールバック時に欲しいポインタがあればここで渡す
		
		
	// 同様に以下の関数でアプリがバックグラウンドに移るとき/復帰時のコールバックを設定可
	//KL_SetBackgroundCallback( void_yourcallback_voidpointer, pObj/*コールバックに渡したい変数ポインタ*/ );
	//KL_SetForegroundCallback( void_yourcallback_voidpointer, pObj/*コールバックに渡したい変数ポインタ*/ );
		
	// 上記はシーンを問わずどこでもコールバックされるので、
	// シーンオブジェクトをコンテキストに設定してしまうと
	// そのシーンではない時にコールバックされるとメモリ破壊が起こる可能性が出てくる
	// そこで、現在のシーンだけ有効なコールバックをして欲しい時は
	// KL_ではなくKLScene_とするとセットしたシーンが終わっていればコールされない
	//KLScene_SetBackgroundCallback(/*引数同じ*/);
	//KLScene_SetForegroundCallback(/*引数同じ*/);
	
}
