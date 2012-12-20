//
//  SoundTest.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "FirstScene.h"
#include "SoundExample.h"

// 終了処理
static void SoundTest_Unload( KLObj* pObj, void* pParam ){

	SoundTest*	p	= (SoundTest*)pObj;

	// シーン終了時に行いたい処理
	for( int i=0; i<SOUND_TEST_MAX; ++i)
	{
		KLAL_Unload(&p->sounds[i]);
	}
	
}

static void SoundTest_FadeOutEnd( KLObj* pObj, void* pParam ){

	KLScene_ChangeTo( FirstScene_Init, NULL );
	
}

// メインループ
static void SoundTest_Update( KLObj* pObj, void* pParam ){
	
	SoundTest*	p	= (SoundTest*)pObj;
	char* str = "Back";
	
	app.label.fontsize = 36;
	
	// charが1文字何byteか不定なので描画文字数指定はstrlenではなくcstrlenにする事！strlenはcharの総byte数で文字数ではない
	KLLabel_Draw(&app.label, str, cstrlen(str), 16, 16, 0, (Rgba){255,255,255,255}, KLLABEL_ALIGN_L);
	
	if( KLTouch_IsStart(0, 0, 128, 32) )
	{
		KLScene_FadeOut( SoundTest_FadeOutEnd, pObj, RGBA(0,0,0,0), 0);
	}
	
	str = "wav play";
	KLLabel_Draw(&app.label, str, cstrlen(str), 16, 128, 0, (Rgba){255,255,255,255}, KLLABEL_ALIGN_L);
	
	if( KLTouch_IsStart(0, 128-36, 320, 72) )
	{
		float starttime = 0.0f;
		
		KLAL_Play( &p->sounds[0], starttime, FALSE);
		
	}
	
	
	str = "mp3 play";
	KLLabel_Draw(&app.label, str, cstrlen(str), 16, 160+128, 0, (Rgba){255,255,255,255}, KLLABEL_ALIGN_L);
	
	if( KLTouch_IsStart(0, 160+128-36, 320, 72) )
	{
		float starttime = 0.0f;
		
		KLAL_Play( &p->sounds[1], starttime, FALSE);
		
	}
	
}

void SoundTest_Init( KLObj* pObj, void* pParam ){
	
	// KLibは予め確保したsizeof(KLObj)+シーンワークサイズ(KLConfig.hで調整可)分のヒープを
	// 引数のpObjとして。シーンワークサイズを超えない構造体であれば
	// そのヒープをキャストするだけで自由に扱えます。
	SoundTest* p = (SoundTest*)pObj;
	
	
	// シーン初期化時に行いたい処理
	//==========================================================
	
	// このシーンは黒いフェードインで開始
	KLScene_FadeIn( NULL, pObj, (Rgba){0,0,0,0}, 0);
	
	// 毎メインループで都度この関数が呼ばれて何度も初期化しても困るので
	// 次のメインループでは違う関数(初期化後の処理)をする関数に切り替えます。
	KLObj_SetFunction( pObj, SoundTest_Update );
	
	
	// シーンが切り替わる際、このシーンのUnload処理を行う関数を設定します。
	KLObj_SetUnload( pObj, SoundTest_Unload );
	
	// サウンド読み込み
	char* pName[] = {
		"tm2r_bom27_a.wav",	// Copyright (C) 2012 http://osabisi.sakura.ne.jp/m2/ All Rights Reserved.
		"powerup10.mp3", // Copyright (C) 2012  http://taira-komori.jpn.org/freesound.html All Rights Reserved.
		"bgm_maoudamashii_cyber07.mp3", // Copyright (C) 2012 http://maoudamashii.jokersounds.com/ All Rights Reserved.
	};

	for( int i=0; i<SOUND_TEST_MAX; ++i )
	{
		
		KLAL_Load( &p->sounds[i], pName[i]);
		
	}
	
	// BGMをループ再生
	KLAL_Play(&p->sounds[SOUND_TEST_OGG], 0, TRUE);
	
	// インジケータ解除
	KLIndicator_Stop(TRUE);
	
}


