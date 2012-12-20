//
//  DeviceTest.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "FirstScene.h"
#include "PrimitiveExample.h"
#include "KLDrawQueue.h"

// 終了処理
static void DeviceTest_Unload( KLObj* pObj, void* pParam ){

	// シーン終了時に行いたい処理
	
}

static void DeviceTest_FadeOutEnd( KLObj* pObj, void* pParam ){

	KLScene_ChangeTo( FirstScene_Init, NULL );
	
}

// メインループ
static void DeviceTest_Update( KLObj* pObj, void* pParam ){
	
	//DeviceTest*	p	= (DeviceTest*)pObj;
	char* str = "Back";
	
	app.label.fontsize = 36;
	
	// charが1文字何byteか不定なので描画文字数指定はstrlenではなくcstrlenにする事！strlenはcharの総byte数で文字数ではない
	KLLabel_Draw(&app.label, str, cstrlen(str), 16, 16, 0, (Rgba){255,255,255,255}, KLLABEL_ALIGN_L);
	
	if( KLTouch_IsStart(0, 0, 128, 32) )
	{
		KLScene_FadeOut( DeviceTest_FadeOutEnd, pObj, RGBA(0,0,0,0), 0);
	}
	
	char info[1024];
	
	sprintf( info, "Device.w:%u\nDevice.h:%u\niOS version:%f\nLanguage:%s\nUIID:%s\nis iPad:%u\nis iPhone:%u\nis iPodTouch:%u\non Simulator:%u\nSeries:%u",
					klib.device.w, klib.device.h,
					klib.device.iosVer,
					klib.device.language,
					klib.device.uiid,
					klib.device.iPad?1:0,
					klib.device.iPhone?1:0,
					klib.device.iPodTouch?1:0,
					klib.device.simulator?1:0,
					klib.device.series
					);
	KLLabel_Draw(&app.label, info, cstrlen(info), 16, 96, 0, (Rgba){255,255,255,255}, KLLABEL_ALIGN_L);

}

void DeviceTest_Init( KLObj* pObj, void* pParam ){
	
	// KLibは予め確保したsizeof(KLObj)+シーンワークサイズ(KLConfig.hで調整可)分のヒープを
	// 引数のpObjとして。シーンワークサイズを超えない構造体であれば
	// そのヒープをキャストするだけで自由に扱えます。
	//DeviceTest* p = (DeviceTest*)pObj;
	
	
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


