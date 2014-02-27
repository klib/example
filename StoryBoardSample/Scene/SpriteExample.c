//
//  SpriteTest.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "SpriteExample.h"
#include "Game.h"
#include "EmptyScene.h"

// 終了処理
static void SpriteTest_Unload( KLObj* pObj, void* pParam ){

	SpriteTest* p = (SpriteTest*)pObj;
	
	// シーン終了時に行いたい処理
	for( int i=0; i<SPRITE_TEST_MAX; ++i )
	{
		// シーンが持つスプライトはこのシーンでしか使わないのでテクスチャも破棄する
		KLSprite_UnloadWithTexture(&p->sprites[i]);
	}
	
}

// メインループ
static void SpriteTest_Update( KLObj* pObj, void* pParam ){
	
	SpriteTest*	p	= (SpriteTest*)pObj;
	int i = 0;
	
	p->label.fontsize = 36;
	
	// 回転(1度ずつ回転)
	p->rot += KL_WDEG_CYCLE*1/360;
	
	// 適当に座標を決めて描画
	for( i=0; i<SPRITE_TEST_MAX; ++i )
	{
		// スプライトの軸を左上にセット（デフォルト）
		// 軸を原点として回転＆座標セットするようになる
		KLSprite_SetCenter(&p->sprites[i], 0, 0);
		
		KLSprite_SetXY(&p->sprites[i], 128 + i%2*160, 128 + i/2*160 );
		KLSprite_SetRot(&p->sprites[i], p->rot);
		KLSprite_Draw(&p->sprites[i]);
		
		// スプライトの軸を中央にセット(中央を軸にくるくる回転)
		KLSprite_SetCenter(&p->sprites[i], p->sprites[i].w*0.5f, p->sprites[i].h*0.5f);
		
		KLSprite_SetXY(&p->sprites[i], 320 + i%2*160, 400 + i/2*160 );
		KLSprite_SetRot(&p->sprites[i], p->rot);
		KLSprite_Draw(&p->sprites[i]);
	}
	
	i = SPRITE_TEST_PNG;
	
	// 画面中央に縮小して表示
	KLSprite_SetWH( &p->sprites[i], 64, 64 );
	KLSprite_SetCenter(&p->sprites[i], p->sprites[i].w*0.5f, p->sprites[i].h*0.5f);
	KLSprite_SetXY(&p->sprites[i], klib.device.w*0.5f, klib.device.h*0.5f );
	KLSprite_SetRot(&p->sprites[i], 0);
	KLSprite_Draw(&p->sprites[i]);
	
	// このままでは次のループのfor（上に書いたやつ）でも縮小されたままなので本来のサイズに戻す
	KLSprite_SetWH( &p->sprites[i], p->sprites[i].w_src, p->sprites[i].h_src );
	
}

void SpriteTest_Init( KLObj* pObj, void* pParam ){
	
	// KLibは予め確保したsizeof(KLObj)+シーンワークサイズ(KLConfig.hで調整可)分のヒープを
	// 引数のpObjとして。シーンワークサイズを超えない構造体であれば
	// そのヒープをキャストするだけで自由に扱えます。
	SpriteTest* p = (SpriteTest*)pObj;
	
	
	// シーン初期化時に行いたい処理
	//==========================================================
	
	// このシーンは黒いフェードインで開始
	KLScene_FadeIn( NULL, pObj, (Rgba){0,0,0,0}, 0);
	
	// スプライトを初期化＆読み込み
	char* pPath[SPRITE_TEST_MAX] = {
		"example.bmp",	// BMP
		"example.gif",	// GIF
		"example.jpg",	// JPEG
		"example.png"	// PNG
	};
	
	for( int i=0; i<SPRITE_TEST_MAX; ++i )
	{
		KLSprite_LoadFile(&p->sprites[i], pPath[i]);
	}
	
	p->rot = 0;
	
	// 毎メインループで都度この関数が呼ばれて何度も初期化しても困るので
	// 次のメインループでは違う関数(初期化後の処理)をする関数に切り替えます。
	KLObj_SetFunction( pObj, SpriteTest_Update );
	
	
	// シーンが切り替わる際、このシーンのUnload処理を行う関数を設定します。
	KLObj_SetUnload( pObj, SpriteTest_Unload );
	
}


