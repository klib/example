//
//  TouchTest.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "FirstScene.h"
#include "TouchExample.h"
#include "KLDrawQueue.h"

// 終了処理
static void TouchTest_Unload( KLObj* pObj, void* pParam ){

	// シーン終了時に行いたい処理
	TouchTest*	p	= (TouchTest*)pObj;
	KLLabel_Unload(&p->label);
	
}

static void TouchTest_FadeOutEnd( KLObj* pObj, void* pParam ){

	KLScene_ChangeTo( FirstScene_Init, NULL );
	
}

// メインループ
static void TouchTest_Update( KLObj* pObj, void* pParam ){
	
	TouchTest*	p	= (TouchTest*)pObj;
	char* str = "Back";
	
	app.label.fontsize = 36;
	
	// charが1文字何byteか不定なので描画文字数指定はstrlenではなくcstrlenにする事！strlenはcharの総byte数で文字数ではない
	KLLabel_Draw(&app.label, str, cstrlen(str), 16, 16, 0, (Rgba){255,255,255,255}, KLLABEL_ALIGN_L);
	
	if( KLTouch_IsStart(0, 0, 128, 32) )
	{
		KLScene_FadeOut( TouchTest_FadeOutEnd, pObj, RGBA(0,0,0,0), 0);
	}
	
	
	// 画面をタッチしている指の位置を全部取得
	u32* fingers = KLTouch_IsTouchingAll( 0, 0, klib.device.w, klib.device.h );
	
	for( int i=0; fingers && fingers[i]; ++i )
	{
		// 指IDから指情報の詳細を得る
		KLFinger* pFinger = KLTouch_GetFinger(fingers[i]);
		
		// タッチ箇所に円を描画
		KL_DrawPolygon( pFinger->startx, pFinger->starty, 192, 0, 0, RGBA(255,0,0,96) );
	}
	
	// 移動した指があれば情報を全取得
	fingers = KLTouch_IsMoveAll( 0, 0, klib.device.w, klib.device.h );
	
	for( int i=0; fingers && fingers[i]; ++i )
	{
		// 指IDから指情報の詳細を得る
		KLFinger* pFinger = KLTouch_GetFinger(fingers[i]);
		
		// タッチ開始位置から移動位置まで線を描画
		KLPos3d pos[] = {{pFinger->startx, pFinger->starty, 0}, {pFinger->movex, pFinger->movey}};
		KL_DrawLine( pos, RGBA(0,192,255,255), 2, 8 );
	}
	
	// 説明文
	str = "タッチ時のハッシュで指を判別する事もできます。\n端末の許容最大数を超えた指の数でタッチすると\n全タッチ情報がキャンセルされる仕様になっています。\niPhoneでは5本指でタッチ中に6本目のタッチがあると\nキャンセルが発生します。";
	p->label.fontsize = 24;
	
	// 縁取り
	u32 clen	= cstrlen(str);
	s32 x		= klib.device.w*0.5f;
	s32 y		= 64;
	KLLabel_Draw(&p->label, str, clen, x+1, y, 0, RGBA(0,0,0,255), KLLABEL_ALIGN_CENTER);
	KLLabel_Draw(&p->label, str, clen, x-1, y, 0, RGBA(0,0,0,255), KLLABEL_ALIGN_CENTER);
	KLLabel_Draw(&p->label, str, clen, x,   y+1, 0, RGBA(0,0,0,255), KLLABEL_ALIGN_CENTER);
	KLLabel_Draw(&p->label, str, clen, x,   y-1, 0, RGBA(0,0,0,255), KLLABEL_ALIGN_CENTER);
	
	KLLabel_Draw(&p->label, str, clen, x, y, 0, RGBA(255,255,255,255), KLLABEL_ALIGN_CENTER);
	
	
	char info[1024];
	
	sprintf( info, "Finger count:%u\n",
					klib.touch.nFinger
					);
	KLLabel_Draw(&app.label, info, cstrlen(info), 16, 256, 0, (Rgba){255,255,255,255}, KLLABEL_ALIGN_L);

}

void TouchTest_Init( KLObj* pObj, void* pParam ){
	
	// KLibは予め確保したsizeof(KLObj)+シーンワークサイズ(KLConfig.hで調整可)分のヒープを
	// 引数のpObjとして。シーンワークサイズを超えない構造体であれば
	// そのヒープをキャストするだけで自由に扱えます。
	TouchTest* p = (TouchTest*)pObj;
	
	
	// シーン初期化時に行いたい処理
	//==========================================================
	
	// このシーンは黒いフェードインで開始
	KLScene_FadeIn( NULL, pObj, (Rgba){0,0,0,0}, 0);
	
	// 毎メインループで都度この関数が呼ばれて何度も初期化しても困るので
	// 次のメインループでは違う関数(初期化後の処理)をする関数に切り替えます。
	KLObj_SetFunction( pObj, TouchTest_Update );
	
	// ラベルを作成
	KLFont font;
	KLFont_Load( &font, KLFont_name[KLFontType_HiraKakuProN_W6], FALSE);
	KLLabel_Init(&p->label);
	KLLabel_ToSprite( &p->label, "タッチ時のハッシュで指を判別する事もできます。\n端末で許容されている最大タッチ数を超えた指の数でタッチすると\n全タッチ情報がキャンセルされる仕様になっています。\niPhoneでは5本指でタッチ中に6本目のタッチがあると\nキャンセルが発生します。", &font );
	KLFont_Unload(&font);
	
	// シーンが切り替わる際、このシーンのUnload処理を行う関数を設定します。
	KLObj_SetUnload( pObj, TouchTest_Unload );
	
}


