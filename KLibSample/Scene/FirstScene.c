//
//  FirstScene.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "KLDrawQueue.h"
#include "KLLogo.h"
#include "FirstScene.h"
#include "PrimitiveExample.h"
#include "SpriteExample.h"
#include "FontAndLabelExample.h"
#include "SoundExample.h"
#include "ObjContainerExample.h"
#include "DeviceExample.h"
#include "TouchExample.h"

//===========================================================
/**
 *	@brief	最初のシーンのアンロード
 *
 *	@param	pObj	アンロードされるシーンオブジェクトのポインタ
 *	@param	pParam	パラメータがあればそのポインタ
 */
//===========================================================
static void FirstScene_Unload( KLObj* pObj, void* pParam ){

	FirstScene* p = (FirstScene*)pObj;
	
	// シーン終了時に行いたい処理
	KLSprite_UnloadWithTexture(&p->spLogo);
	
}

//===========================================================
/**
 *	@brief	フェードアウト完了時のコールバック
 *
 *	@param	pObj	シーンオブジェクトのポインタ
 *	@param	pParam	パラメータがあればそのポインタ
 */
//===========================================================
static void FirstScene_FadeOutEnd( KLObj* pObj, void* pParam ){

	FirstScene* p = (FirstScene*)pObj;
	
	// フェードアウトしたのでシーンを切り替える
	KLScene_ChangeTo( p->pNextSceneInitFunc, NULL );
	
}

//===========================================================
/**
 *	@brief	メインループ
 *
 *	@param	pObj	シーンオブジェクトのポインタ
 *	@param	pParam	パラメータがあればそのポインタ
 */
//===========================================================
static void FirstScene_Update( KLObj* pObj, void* pParam ){
	
	// KLibは予め確保したsizeof(KLObj)+シーンワークサイズ(KLConfig.hで調整可)分のヒープを
	// 引数のpObjとして渡します。シーンワークサイズを超えない構造体であれば
	// そのヒープをキャストするだけで１シーン内で自由に扱えます。
	FirstScene*	p		= (FirstScene*)pObj;
	
	s8			touched	= -1;
	char*		str		= "EXAMPLE MENU";
	u8			col		= 0;
	
	KL_DrawRectScreen( RGBA(255,255,255,255) );
	
	KLSprite_SetCenterSimple(&p->spLogo);
	KLSprite_SetXY(&p->spLogo, klib.device.w*0.5f, klib.device.h*0.5f);
	KLSprite_SetRgb( &p->spLogo, 218, 218, 218 );
	KLSprite_Draw(&p->spLogo);
	
	app.label.fontsize = 36;
	
	// charが1文字何byteか不定なので描画文字数指定はstrlenではなくcstrlenにする事！strlenはcharの総byte数で文字数ではない
	KLLabel_Draw(&app.label, str, cstrlen(str), 16, 16, 0, (Rgba){0,0,0,255}, KLLABEL_ALIGN_L);
	
	// MENU
	//---------------------------------------------------
	typedef struct{
		
		char*		pCaption;
		KLObjFunc	func;
		
	}Menu;
	
	Menu menu[] = {
		{"Primitives",			PrimitiveTest_Init },
		{"Sprites",				SpriteTest_Init },
		{"Font&Labels",			FontAndLabel_Init },
		{"Sounds",				SoundTest_Init },
		{"Object Container",	ObjContainer_Init },
		{"Device info",			DeviceTest_Init },
		{"Touch&Scroll",		TouchTest_Init },
	};
	
	app.label.fontsize = 32;
	for( int i=0; i<sizeof(menu)/sizeof(Menu); ++i )
	{
		str = menu[i].pCaption;
		
		col = p->pNextSceneInitFunc==menu[i].func? 255 : 0;
		
		// charが1文字何byteか不定なので描画文字数指定はstrlenではなくcstrlenにする事！strlenはcharの総byte数で文字数ではない
		KLLabel_Draw(&app.label, str, cstrlen(str), 32, 96 + 72*i, 0, (Rgba){0,col,col*0.5f,255}, KLLABEL_ALIGN_L);
		
		// 既にタッチされていないか
		if( touched==-1 )
		{
			// タッチした？
			if( KLTouch_IsStart( 0, 96 + 72*i -16, klib.device.w, app.label.fontsize+32 ) )
			{
				KLLog("Touched:%s\n", menu[i].pCaption);
				touched = i;
			}
		}
		
	}
	
	// タッチされているものがあればシーン切り替え
	if( touched!=-1 )
	{
	
		// フェードアウト開始(使用せずパッと切り替えたいなら直接KLScene_ChangeToしてよい)
		KLScene_FadeOut( FirstScene_FadeOutEnd, pObj, (Rgba){0,0,0,0}, 0);
		p->pNextSceneInitFunc = menu[touched].func;
	
		// サウンド読み込みだけ時間がかかるのでインジケータ表示
		if( p->pNextSceneInitFunc==SoundTest_Init )
		{
			KLIndicator_Start();
		}
		
	}
	
}

//===========================================================
/**
 *	@brief	シーンの初期化
 *
 *	@param	pObj	シーンオブジェクトのポインタ
 *	@param	pParam	パラメータがあればそのポインタ
 */
//===========================================================
void FirstScene_Init( KLObj* pObj, void* pParam ){
	
	FirstScene* p = (FirstScene*)pObj;
	
	// このシーンは黒いフェードインで開始
	KLScene_FadeIn( NULL, pObj, (Rgba){0,0,0,0}, 0);
		
	// メインループ関数を切り替える(シーンはこのまま)
	//----------------------------------------------------------

	// Logo
	KLSprite_Init(&p->spLogo);
	KLSprite_LoadFile(&p->spLogo, "klib.png");
	
	// 毎メインループで都度この関数が呼ばれ、何度も初期化しても困るので
	// 次のメインループでは違う関数(初期化後の処理)を呼んでもらえるように
	// シーンマネージャにメインループ関数に切り替えを依頼します。
	KLObj_SetFunction( pObj, FirstScene_Update );
	
	// このシーンのUnload処理を行う関数を設定します。
	// KLScene_ChangeToするとコールされます。
	KLObj_SetUnload( pObj, FirstScene_Unload );
	
}


