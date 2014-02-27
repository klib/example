//
//  Logo.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "KLDrawQueue.h"
#include "Game.h"
#include "Logo.h"
#include "EmptyScene.h"

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
	
	// シーン終了時に、このシーンでしか使わないスプライトをテクスチャごと破棄する
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
	
	// もし、Objective-c 周りの API や UI が不要なら、
	// タイトル画面のシーンなど、最初の画面のシーンのコードを作成後、
	// その初期化関数を指定すればよい
	KLScene_ChangeTo( EmptyScene_Init, NULL );
	
	// KLib から Storyboard を連携したい場合は以下を使用して移動先を指定
	// KLib だけで Objective-c 周りの API や UI が不要なら、上記同様
	// Scene の切り替えだけで作成していけばよいので、以下は不要
	KL_GotoSegue("toSampleList");
	
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
	
	// 引数の pObj は、シーン作成時に KLib が予め確保したすぐに使えるヒープのポインタです。
	// サイズは、sizeof(KLObj) + シーンワークサイズ( KLConfig.h で調整可)分なので、
	// そのサイズを超えない構造体を定義して、それにキャストするだけで１シーン内で自由に扱えます。
	// ※ 確保されているだけで、初期化されてはいませんのでシーン初期化関数内で
	// 　キャスト後の内容を自分で初期化してやる必要があります。
	Logo*	p		= (Logo*)pObj;
	
	p->wait++;
	
	KL_DrawRectScreen(RGBA(0,0,0,255));
	
	KLSprite_SetCenterSimple( &p->spLogo );
	KLSprite_SetXY( &p->spLogo, klib.device.w*0.5f, klib.device.h*0.5f );
	KLSprite_Draw( &p->spLogo );
	
	// 一定時間ロゴ表示か、タッチしたらフェードアウトして次のシーンへ
	if( KLTouch_IsStart( 0, 0, klib.device.w, klib.device.h ) || (!p->isfade && p->wait>klib.fps*3.0f) )
	{
		
		// フェードアウト開始(使用せずパッと切り替えたいなら直接KLScene_ChangeToしてよい)
		KLScene_FadeOut( Logo_FadeOutEnd, pObj, (Rgba){0,0,0,0}, 0);
		
		p->isfade = 1;
		
	}
	
}

// 結合文字なら進むインデクス数、結合文字やサロゲートペアでなければ0
int IsCombinedChar ( wchar* pwc, int i, size_t len ) {

	// 囲み文字チェック
	if ( pwc[i]>=0x30 && pwc[i]<=0x39 )
	{
	
		if ( i+1<len && pwc[i+1]==020E3 )
		{
			return 2;
		}
		
	}
	// 国旗系ダブルサロゲートペア
	else if ( pwc[i]==0xD83C )
	{
		
		if ( i+1<len && pwc[i+1]>=0xDDE6 && pwc[i+1]<=0xDDFF )
		{
			// 2文字目も同様の国旗用サロゲートペア？
			if ( i+2<len && pwc[i]==0xD83C )
			{
				
				if ( i+3<len && pwc[i+3]>=0xDDE6 && pwc[i+3]<=0xDDFF )
				{
					
					return 4;
					
				}
				// 最初のサロゲートペアのみで結合文字ではないはず(１・２文字目のサロゲートペアで一文字として2を返す)
				else
				{
					return 2;
				}
			
			}
			// 最初のサロゲートペアのみで結合文字ではないはず
			else
			{
				return 2;
			}
		}
		// 0xD83C単独文字はないと思うが結合文字ではない
		else
		{
			
		}
		
	}
	
	
	return 0;
	
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
	
	// 引数の pObj は、シーン作成時に KLib が予め確保したすぐに使えるヒープのポインタです。
	// サイズは、sizeof(KLObj) + シーンワークサイズ( KLConfig.h で調整可)分なので、
	// そのサイズを超えない構造体を定義して、それにキャストするだけで１シーン内で自由に扱えます。
	// ※ 確保されているだけで、初期化されてはいませんのでシーン初期化関数内で
	// 　キャスト後の内容を自分で初期化してやる必要があります。
	Logo* p = (Logo*)pObj;
	
	// このシーンは黒いフェードインで開始
	KLScene_FadeIn( NULL, pObj, (Rgba){0,0,0,0}, 0);
	
	
	// 全シーン共通で使用するグローバルオブジェクト
	// (今回はグローバル変数appとして定義)の初期化
	// KLibView 以外をまたぐ場合は都度テクスチャを読み直す必要が出てくるので
	// このサンプルでは、都度初期化の必要があるが、1ビューでやりくりする場合は
	// ここで一度だけ初期化するような方法をとった方が効率が良い場合がある
	//----------------------------------------------------------
	APP_Init (TRUE);
	
	// このシーン限定のスプライトをロード
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


