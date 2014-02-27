//
//  DeviceTest.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "DeviceExample.h"
#include "KLDrawQueue.h"
#include "Game.h"

// 終了処理
static void DeviceTest_Unload( KLObj* pObj, void* pParam ){

	DeviceTest*	p	= (DeviceTest*)pObj;

	// シーン終了時に行いたい処理
	KLLabel_Unload(&p->label);
	
}

// メインループ
static void DeviceTest_Update( KLObj* pObj, void* pParam ){
	
	DeviceTest*	p	= (DeviceTest*)pObj;
	char info[1024];
	
	p->label.fontsize = 32;
	
	sprintf( info, "Device.w:%u\nDevice.h:%u\nView.w:%u\nView.h:%u\niOS version:%f\nLanguage:%s\nUIID:%s\nis iPad:%u\nis iPhone:%u\nis iPodTouch:%u\non Simulator:%u\nSeries:%u",
					klib.device.w, klib.device.h,
					klib.view.w, klib.view.h,
					klib.device.iosVer,
					klib.device.language,
					klib.device.uiid,
					klib.device.iPad?1:0,
					klib.device.iPhone?1:0,
					klib.device.iPodTouch?1:0,
					klib.device.simulator?1:0,
					klib.device.series
					);
	KLLabel_Draw(&p->label, info, cstrlen(info), 16, 96, 0, (Rgba){255,255,255,255}, KLLABEL_ALIGN_L);

}

void DeviceTest_Init( KLObj* pObj, void* pParam ){
	
	// KLibは予め確保したsizeof(KLObj)+シーンワークサイズ(KLConfig.hで調整可)分のヒープを
	// 引数のpObjとして。シーンワークサイズを超えない構造体であれば
	// そのヒープをキャストするだけで自由に扱えます。
	DeviceTest* p = (DeviceTest*) pObj;
	
	// ラベルに使うフォントの初期化
	KLFont_Load(&p->font, KLFont_name[KLFontType_HiraKakuProN_W3], 0);
	
	
	// 文字表示用にラベルを初期化（絵文字はまだ非対応）
	KLLabel_Init(&p->label);
	
		
	// 予め使用する文字をテクスチャにマッピングしておき
	// 描画時にそれらを組み合わせて文字列として表示する
	// なので、ここでテクスチャに含まれない文字は描画できない
	KLLabel_Setup(&p->label,
					 KLSTRSET_ALL, // プリセット以外も追加したければスペースの後ろに定義可
					 &p->font);
	
	
	
	// 文字を変えてスプライトを再作成しない限りはもうフォントは使わないので破棄
	KLFont_Unload(&p->font);
	
	
	
	
	// シーン初期化時に行いたい処理
	//==========================================================
	
	// このシーンは黒いフェードインで開始
	KLScene_FadeIn( NULL, pObj, (Rgba){0,0,0,0}, 0);
	
	// 毎メインループで都度この関数が呼ばれて何度も初期化しても困るので
	// 次のメインループでは違う関数(初期化後の処理)をする関数に切り替えます。
	KLObj_SetFunction( pObj, DeviceTest_Update );
	
	
	// シーンが切り替わる際、このシーンのUnload処理を行う関数を設定します。
	KLObj_SetUnload( pObj, DeviceTest_Unload );
	
}


