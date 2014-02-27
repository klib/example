//
//  HttpExample.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "KLDrawQueue.h"
#include "Game.h"
#include "HttpExample.h"
#include "EmptyScene.h"

#define GET_URL "http://k.yimg.jp/images/top/sp2/cmn/logo-ns_d-130528.png"
#define POST_URL "https://ssl.gstatic.com/accounts/ui/logo_2x.png"

//===========================================================
/**
 *	@brief	最初のシーンのアンロード
 *
 *	@param	pObj	アンロードされるシーンオブジェクトのポインタ
 *	@param	pParam	パラメータがあればそのポインタ
 */
//===========================================================
static void HttpExample_Unload( KLObj* pObj, void* pParam ){

	HttpExample* p = (HttpExample*)pObj;
	
	if ( p->pCurrentReq )
	{
		
		KLHttp_Cancel (p->pCurrentReq);
		
	}
	
	KLHttp_Unload(&p->getReq);
	KLHttp_Unload(&p->postReq);
	
	KLIndicator_Stop(TRUE);
	
}

void HttpExample_GetCallback ( bin8* pData, KLHttpEvent event, int statusCode, size_t sz, void* pContext ) {

	HttpExample* p = (HttpExample*) pContext;
	
	if ( pData && statusCode==KLHTTP_OK )
	{
		
		KLSprite_UnloadWithTexture( &p->sprite );
		KLSprite_LoadFromImageData( &p->sprite, pData, sz);
		
	}
	
	KLIndicator_Stop(TRUE);
	p->pCurrentReq = NULL;
	
	KLLog ("GET METHOD returned.\nStatusCode:%d\nSize:%ld\n", statusCode, sz);
	
}

void HttpExample_PostCallback ( bin8* pData, KLHttpEvent event, int statusCode, size_t sz, void* pContext ) {
	
	HttpExample* p = (HttpExample*) pContext;
	
	if ( pData && statusCode==KLHTTP_OK )
	{
	
		KLSprite_UnloadWithTexture( &p->sprite );
		KLSprite_LoadFromImageData( &p->sprite, pData, sz);
	
	}
	
	KLIndicator_Stop(TRUE);
	p->pCurrentReq = NULL;
	
	KLLog ("POST METHOD returned.\nStatusCode:%d\nSize:%ld\n", statusCode, sz);

}

static void HttpExample_Alert () {

	KLAlert_Open("接続エラー", "端末が通信できる状態にありません。Wifiや電波の状態をご確認ください。", "O K", NULL);
	
}

static void HttpExample_Connect (HttpExample* p, bl isPost) {
	
	KLIndicator_Start();
	
	if (isPost)
	{
	
		KLHttp_InitPost( p->pCurrentReq, POST_URL, NULL, 10.0f );
		
	}
	else
	{
	
		KLHttp_InitGet( p->pCurrentReq, GET_URL, 10.0f );
		
	}
	
	KLHttp_Start( p->pCurrentReq, isPost ? HttpExample_PostCallback : HttpExample_GetCallback, p );
	
}

//===========================================================
/**
 *	@brief	メインループ
 *
 *	@param	pObj	シーンオブジェクトのポインタ
 *	@param	pParam	パラメータがあればそのポインタ
 */
//===========================================================
static void HttpExample_Update( KLObj* pObj, void* pParam ){
	
	// 引数の pObj は、シーン作成時に KLib が予め確保したすぐに使えるヒープのポインタです。
	// サイズは、sizeof(KLObj) + シーンワークサイズ( KLConfig.h で調整可)分なので、
	// そのサイズを超えない構造体を定義して、それにキャストするだけで１シーン内で自由に扱えます。
	// ※ 確保されているだけで、初期化されてはいませんのでシーン初期化関数内で
	// 　キャスト後の内容を自分で初期化してやる必要があります。
	HttpExample*	p		= (HttpExample*)pObj;
	
	int marginx = 48;
	int btnw	= 240;
	bl	pass	= FALSE;
	
	KL_DrawRectScreen(RGBA(0,0,0,255));
	
	// GET
	KL_DrawRect( marginx, klib.view.h*0.75f, btnw, 64, RGBA(0x66,0xCC,0x99,0xFF) );
	KLLabel_Draw( &p->label, "GET", 3, marginx+btnw*0.5f, klib.view.h*0.75f+12, 0, RGBA(0xFF,0xFF,0xFF,0xFF), KLLABEL_ALIGN_CENTER);
	
	if ( !p->pCurrentReq && KLTouch_IsEnd(marginx, klib.view.h*0.75f, btnw, 64) )
	{
		
		if ( KLNetwork_IsConnecting() )
		{
			
			p->pCurrentReq = &p->getReq;
			HttpExample_Connect(p,FALSE);
			
		}
		else
		{
			
			HttpExample_Alert();
			pass = TRUE;
			
		}
		
	}
	
	// POST
	KL_DrawRect( klib.view.w-marginx-btnw, klib.view.h*0.75f, btnw, 64, RGBA(0x66,0xCC,0x99,0xFF) );
	KLLabel_Draw( &p->label, "POST", 4, klib.view.w-marginx-btnw*0.5f, klib.view.h*0.75f+12, 0, RGBA(0xFF,0xFF,0xFF,0xFF), KLLABEL_ALIGN_CENTER);
	
	if ( !pass && !p->pCurrentReq && KLTouch_IsStart(klib.view.w-marginx-btnw, klib.view.h*0.75f, btnw, 64) )
	{
		
		if ( KLNetwork_IsConnecting() )
		{
		
			p->pCurrentReq = &p->postReq;
			HttpExample_Connect(p,TRUE);
			
		}
		else
		{
			
			HttpExample_Alert();
			
		}
		
	}
	
	p->rot += 0x40;
	
	KLSprite_SetRot( &p->sprite,  p->rot);
	KLSprite_SetCenterSimple( &p->sprite ); // 中央を回転軸に
	KLSprite_SetXY( &p->sprite, klib.view.w*0.5f, klib.view.h*0.5f );
	KLSprite_Draw( &p->sprite );
	
}

//===========================================================
/**
 *	@brief	シーンの初期化
 *
 *	@param	pObj	シーンオブジェクトのポインタ
 *	@param	pParam	パラメータがあればそのポインタ
 */
//===========================================================
void HttpExample_Init( KLObj* pObj, void* pParam ){
	
	// 引数の pObj は、シーン作成時に KLib が予め確保したすぐに使えるヒープのポインタです。
	// サイズは、sizeof(KLObj) + シーンワークサイズ( KLConfig.h で調整可)分なので、
	// そのサイズを超えない構造体を定義して、それにキャストするだけで１シーン内で自由に扱えます。
	// ※ 確保されているだけで、初期化されてはいませんのでシーン初期化関数内で
	// 　キャスト後の内容を自分で初期化してやる必要があります。
	HttpExample* p = (HttpExample*)pObj;
	
	// このシーンは黒いフェードインで開始
	KLScene_FadeIn( NULL, pObj, (Rgba){0,0,0,0}, 0);
	
	p->pCurrentReq = NULL;
	
	
	// この画像に読み込み後のデータを反映
	KLSprite_Init(&p->sprite);
	
	
	// ラベルに使うフォントの初期化
	KLFont_Load(&p->font, KLFont_name[KLFontType_HiraKakuProN_W6], 0);
	
	
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
	
	
	
	// メインループ関数を切り替える(シーンはこのまま)
	//----------------------------------------------------------

	// 毎メインループで都度この関数が呼ばれ、何度も初期化しても困るので
	// 次のメインループでは違う関数(初期化後の処理)を呼んでもらえるように
	// シーンマネージャにメインループ関数に切り替えを依頼します。
	KLObj_SetFunction( pObj, HttpExample_Update );
	
	// このシーンのUnload処理を行う関数を設定します。
	// KLScene_ChangeToするとコールされます。
	KLObj_SetUnload( pObj, HttpExample_Unload );
	
}


