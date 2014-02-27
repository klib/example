//
//  SoundTest.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "SoundExample.h"
#include "Game.h"
#include "EmptyScene.h"

// 終了処理
static void SoundTest_Unload( KLObj* pObj, void* pParam ){

	SoundTest*	p	= (SoundTest*)pObj;

	// シーン終了時に行いたい処理
	for( int i=0; i<SOUND_TEST_MAX; ++i)
	{
		KLAL_Unload(&p->sounds[i]);
	}
	
	// シーン終了時に行いたい処理
	KLLabel_Unload(&p->label);
	
}

// メインループ
static void SoundTest_Update( KLObj* pObj, void* pParam ){
	
	SoundTest*	p	= (SoundTest*)pObj;
	char* str = NULL;
	
	p->label.fontsize = 36;
	
	
	str = "SE(wav) play";
	KLLabel_Draw(&p->label, str, cstrlen(str), 16, 128, 0, (Rgba){255,255,255,255}, KLLABEL_ALIGN_L);
	
	if( KLTouch_IsStart(0, 128-36, 320, 72) )
	{
		float starttime = 0.0f;
		
		KLAL_Play( &p->sounds[0], starttime, FALSE);
		
	}
	
	
	str = "SE(mp3) play";
	KLLabel_Draw(&p->label, str, cstrlen(str), 16, 160+128, 0, (Rgba){255,255,255,255}, KLLABEL_ALIGN_L);
	
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
	
	
	// ラベルに使うフォントの初期化
	KLFont_Load(&p->font, KLFont_name[KLFontType_HiraKakuProN_W6], 0);
	
	
	// 文字表示用にラベルを初期化（絵文字はまだ非対応）
	KLLabel_Init(&p->label);
	
	
	// 予め使用する文字をテクスチャにマッピングしておき
	// 描画時にそれらを組み合わせて文字列として表示する
	// なので、ここでテクスチャに含まれない文字は描画できない
	KLLabel_Setup(&p->label,
					 KLSTRSET_ALL "使用漢字", // プリセット以外も追加したければスペースの後ろに定義可
					 &p->font);
	
	
	
	// 文字を変えてスプライトを再作成しない限りはもうフォントは使わないので破棄
	KLFont_Unload(&p->font);

	
}


