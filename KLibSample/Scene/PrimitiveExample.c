//
//  PrimitiveTest.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "FirstScene.h"
#include "PrimitiveExample.h"
#include "KLDrawQueue.h"

// 終了処理
static void PrimitiveTest_Unload( KLObj* pObj, void* pParam ){

	// シーン終了時に行いたい処理
	
}

static void PrimitiveTest_FadeOutEnd( KLObj* pObj, void* pParam ){

	KLScene_ChangeTo( FirstScene_Init, NULL );
	
}

// メインループ
static void PrimitiveTest_Update( KLObj* pObj, void* pParam ){
	
	//PrimitiveTest*	p	= (PrimitiveTest*)pObj;
	char* str = "Back";
	
	app.label.fontsize = 36;
	
	// charが1文字何byteか不定なので描画文字数指定はstrlenではなくcstrlenにする事！strlenはcharの総byte数で文字数ではない
	KLLabel_Draw(&app.label, str, cstrlen(str), 16, 16, 0, (Rgba){255,255,255,255}, KLLABEL_ALIGN_L);
	
	if( KLTouch_IsStart(0, 0, 128, 32) )
	{
		KLScene_FadeOut( PrimitiveTest_FadeOutEnd, pObj, RGBA(0,0,0,0), 0);
	}
	
	KLPos3d pos[4] = {
		
		{32, 96, 0}, {240, 96, 0}, {240, 160, 0}, {320, 160, 0}
	
	};
	
	KL_DrawLine( pos, RGBA(255,0,0,255), 4, 4 );
	
	KL_DrawRect( 64, 160, 64, 64, RGBA(192,192,255,255) );
	
	KL_DrawPolygon( 128, 320, 96, 5, KL_WDEG_CYCLE*0.75f, RGBA(32,255,64,255) );
	
	KL_DrawPolygon( 420, 320, 128, 0, 0, RGBA(0,0,240,255) );
	
	KL_DrawGradationX( 218, 240, 128, 128, RGBA(0,0,0,255), RGBA(255,255,255,255) );
	
	KL_DrawGradation(360, 32, 128, 128, RGBA(255,0,0,255), RGBA(0,255,0,255), RGBA(0,0,255,255), RGBA(0,0,0,0) );
	
	Rgba rgba[4] = {
		
		{255,0,0,255}, {0,255,0,255}, {0,0,255,255}, {0,0,0,0}
		
	};
	
	KLPos3d pos2[4] = {
		
		{32, 480, 0}, {320, 460, 0}, {320, 520, 0}, {480, 480, 0}
		
	};
	
	KL_DrawLineMultiColor(pos2, rgba, 4, 8);
	
}

void PrimitiveTest_Init( KLObj* pObj, void* pParam ){
	
	// KLibは予め確保したsizeof(KLObj)+シーンワークサイズ(KLConfig.hで調整可)分のヒープを
	// 引数のpObjとして。シーンワークサイズを超えない構造体であれば
	// そのヒープをキャストするだけで自由に扱えます。
	//PrimitiveTest* p = (PrimitiveTest*)pObj;
	
	
	// シーン初期化時に行いたい処理
	//==========================================================
	
	// このシーンは黒いフェードインで開始
	KLScene_FadeIn( NULL, pObj, (Rgba){0,0,0,0}, 0);
	
	// 毎メインループで都度この関数が呼ばれて何度も初期化しても困るので
	// 次のメインループでは違う関数(初期化後の処理)をする関数に切り替えます。
	KLObj_SetFunction( pObj, PrimitiveTest_Update );
	
	
	// シーンが切り替わる際、このシーンのUnload処理を行う関数を設定します。
	KLObj_SetUnload( pObj, PrimitiveTest_Unload );
	
}


