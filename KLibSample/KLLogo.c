//
//  Logo.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "KLDrawQueue.h"
#include "KLLogo.h"
#include "FirstScene.h"

App app;

//===========================================================
/**
 *	@brief	アプリ終了時のコールバック
 *
 *	@param	pContext	コンテキストのポインタ
 */
//===========================================================
static void APP_Unload(void* pContext){
	
	KLLabel_Unload(&app.label);
	
}

//===========================================================
/**
 *	@brief	最初のシーンのアンロード
 *
 *	@param	pObj	アンロードされるシーンオブジェクトのポインタ
 *	@param	pParam	パラメータがあればそのポインタ
 */
//===========================================================
static void Logo_Unload( KLObj* pObj, void* pParam ){

	Logo* p = (Logo*)pObj;
	
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
static void Logo_FadeOutEnd( KLObj* pObj, void* pParam ){

	//Logo* p = (Logo*)pObj;
	
	// フェードアウトしたのでシーンを切り替える
	KLScene_ChangeTo( FirstScene_Init, NULL );
	
}

//===========================================================
/**
 *	@brief	メインループ
 *
 *	@param	pObj	シーンオブジェクトのポインタ
 *	@param	pParam	パラメータがあればそのポインタ
 */
//===========================================================
static void Logo_Update( KLObj* pObj, void* pParam ){
	
	// KLibは予め確保したsizeof(KLObj)+シーンワークサイズ(KLConfig.hで調整可)分のヒープを
	// 引数のpObjとして渡します。シーンワークサイズを超えない構造体であれば
	// そのヒープをキャストするだけで１シーン内で自由に扱えます。
	Logo*	p		= (Logo*)pObj;
	
	p->wait++;
	
	KL_DrawRectScreen(RGBA(0,0,0,255));
	
	KLSprite_SetCenterSimple( &p->spLogo );
	KLSprite_SetXY( &p->spLogo, klib.device.w*0.5f, klib.device.h*0.5f );
	KLSprite_Draw( &p->spLogo );
	
	// タッチされているものがあればシーン切り替え
	if( KLTouch_IsStart( 0, 0, klib.device.w, klib.device.h ) || (!p->isfade && p->wait>klib.fps*3.0f) )
	{
		
		// フェードアウト開始(使用せずパッと切り替えたいなら直接KLScene_ChangeToしてよい)
		KLScene_FadeOut( Logo_FadeOutEnd, pObj, (Rgba){0,0,0,0}, 0);
		
		p->isfade = 1;
		
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
void Logo_Init( KLObj* pObj, void* pParam ){
	
	Logo* p = (Logo*)pObj;
	
	// このシーンは黒いフェードインで開始
	KLScene_FadeIn( NULL, pObj, (Rgba){0,0,0,0}, 0);
	
	
	// 全シーン共通で使用するグローバルオブジェクト
	// (今回はグローバル変数appとして定義)の初期化
	//----------------------------------------------------------
	if( !app.loaded )
	{
		// ラベルに使うフォントの初期化
		KLFont_Load(&app.font, KLFont_name[KLFontType_HiraKakuProN_W6], 0);
		
		
		
		// 文字表示用にラベルを初期化（絵文字はまだ非対応）
		KLLabel_Init(&app.label);
		
		
		
		// 予め使用する文字をテクスチャにマッピングしておき
		// 描画時にそれらを組み合わせて文字列として表示する
		// なので、ここでテクスチャに含まれない文字は描画できない
		KLLabel_ToSprite(&app.label,
						 KLSTRSET_ALL "使用漢字", // プリセット以外も追加したければスペースの後ろに定義可
						 &app.font);
		
		
		
		// 文字を変えてスプライトを再作成しない限りはもうフォントは使わないので破棄
		KLFont_Unload(&app.font);
		
		
		
		// アプリ終了時に他のアンロードがしたいので終了時をコールバックで教えてもらう
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
		
		// このシーンに戻ってくるたびにグローバルオブジェクトを初期化しない
		app.loaded = 1;
	}
	
	KLSprite_Init(&p->spLogo);
	KLSprite_LoadFile(&p->spLogo, "klib.png");
	
		
	// メインループ関数を切り替える(シーンはこのまま)
	//----------------------------------------------------------

	// 毎メインループで都度この関数が呼ばれ、何度も初期化しても困るので
	// 次のメインループでは違う関数(初期化後の処理)を呼んでもらえるように
	// シーンマネージャにメインループ関数に切り替えを依頼します。
	KLObj_SetFunction( pObj, Logo_Update );
	
	// このシーンのUnload処理を行う関数を設定します。
	// KLScene_ChangeToするとコールされます。
	KLObj_SetUnload( pObj, Logo_Unload );
	
}


