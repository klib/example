//
//  FileEncWithKbd.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "KLFile.h"
#include "KLMath.h"
#include "KLDrawQueue.h"
#include "Game.h"
#include "FileEncWithKbdExample.h"
#include "EmptyScene.h"

#define FILEENCWITHKBD_PLACEHOLDER "保存するファイルの中身を入力"
#define FILEENCWITHKBD_STR KLSTRSET_ALL FILEENCWITHKBD_PLACEHOLDER "ここをタップ" "暗号化したファイルの書き込みが完了しました！" "書き込み読み込み復元"

#define FILEENCWITHKBD_FILEPATH "klib_example.txt"

static void FileEncWithKbd_CallbackKbdClosed (char* pInputStr, void* pContext) {

	FileEncWithKbd* p = (FileEncWithKbd*) pContext;
	size_t sz = 0;
	
	// ラベルをマッピングし直す
	char* pMap = strmerge( pInputStr, FILEENCWITHKBD_STR );
	KLLabel_Setup( &p->label, pMap, &p->font );
	sfree(pMap);
	
	sfree(p->pStr);
	
	sz = strlen(pInputStr)+1;
	p->pStr = Malloc ( sz );
	memset ( p->pStr, 0, sz );
	memcpy ( p->pStr, pInputStr, sz-1 );
	
}

static void FileEncWithKbd_CallbackKbdReturned (char* pInputStr, void* pContext) {
	
	
	
}

//===========================================================
/**
 *	@brief	最初のシーンのアンロード
 *
 *	@param	pObj	アンロードされるシーンオブジェクトのポインタ
 *	@param	pParam	パラメータがあればそのポインタ
 */
//===========================================================
static void FileEncWithKbd_Unload( KLObj* pObj, void* pParam ){

	FileEncWithKbd* p = (FileEncWithKbd*)pObj;
	
	sfree(p->pStr);
	
	KLFont_Unload(&p->font);

}

//===========================================================
/**
 *	@brief	メインループ
 *
 *	@param	pObj	シーンオブジェクトのポインタ
 *	@param	pParam	パラメータがあればそのポインタ
 */
//===========================================================
static void FileEncWithKbd_Update( KLObj* pObj, void* pParam ){
	
	// 引数の pObj は、シーン作成時に KLib が予め確保したすぐに使えるヒープのポインタです。
	// サイズは、sizeof(KLObj) + シーンワークサイズ( KLConfig.h で調整可)分なので、
	// そのサイズを超えない構造体を定義して、それにキャストするだけで１シーン内で自由に扱えます。
	// ※ 確保されているだけで、初期化されてはいませんのでシーン初期化関数内で
	// 　キャスト後の内容を自分で初期化してやる必要があります。
	FileEncWithKbd*	p		= (FileEncWithKbd*)pObj;
	
	int marginx = klib.view.w*0.15f;
	int btnw	= klib.view.w-marginx*2;
	bl	pass	= FALSE;
	size_t sz   = 0;
	
	KL_DrawRectScreen(RGBA(0,0,0,255));
	
	KL_DrawRect( marginx, klib.view.h*0.25f, btnw, 64, RGBA(0xFF,0xFF,0xFF,0xFF) );

	p->label.fontsize = 20;
	KLLabel_Draw( &p->label, FILEENCWITHKBD_PLACEHOLDER, cstrlen(FILEENCWITHKBD_PLACEHOLDER), marginx+btnw*0.5f, klib.view.h*0.25f-28, 0, RGBA(0x99,0x99,0x99,0xFF), KLLABEL_ALIGN_CENTER);
	
	p->label.fontsize = 32;
	
	// タグを入力しても無視してそのまま表示
	p->label.escapeTag = TRUE;
	
	KLLabel_Draw( &p->label, p->pStr?p->pStr:"ここをタップ", cstrlen(p->pStr?p->pStr:"ここをタップ"), marginx+btnw*0.5f, klib.view.h*0.25f+12, btnw, p->pStr?RGBA(0x00,0x00,0x00,0xFF):RGBA(0x99,0x99,0x99,0xFF), KLLABEL_ALIGN_CENTER);
	
	// 元に戻す
	p->label.escapeTag = FALSE;
	
	if (  KLTouch_IsEnd(marginx, klib.view.h*0.25f, btnw, 64) )
	{
		
		KLKeyboard_Open ( NULL, FileEncWithKbd_CallbackKbdClosed, FileEncWithKbd_CallbackKbdReturned, p );
		
	}
	
	// 暗号化
	btnw = 128;
	KL_DrawRect( marginx, klib.view.h*0.55f, btnw, 64, RGBA(0x00,0x99,0x00,0xFF) );
	KLLabel_Draw( &p->label, "暗号化", cstrlen("暗号化"), marginx+btnw*0.5f, klib.view.h*0.55f+12, 0, RGBA(0x00,0x00,0x00,0xFF), KLLABEL_ALIGN_CENTER);
	
	if ( KLTouch_IsEnd(marginx, klib.view.h*0.55f, btnw, 64) )
	{
		if ( !p->pStr )
		{
			
			KLAlert_Open("", "ファイルに書き込む文字列を入力してから暗号化しましょう。", "O K", NULL);
			
		}
		else
		{
			
			// 暗号化されると、保存先のファイルを開いて見ても入力した文字列ではなくなっている
			size_t len = strlen(p->pStr);
			char* pFileValue = NULL;
			char* pStr = Malloc( len+1 );
			memset ( pStr, 0, len+1 );
			memcpy ( pStr, p->pStr, len );
			
			// データを4つのキーで簡易暗号化する
			// 同じキーを使わないと復元できず失敗となる
			// この処理がなければ暗号化せずハッシュ＋生のデータで保存する
			KLMath_Encrypt ( pStr, len, 123, 456, 789, 1011 );
			
			if ( KLFile_WriteDocumentsWithHash( FILEENCWITHKBD_FILEPATH, pStr, len) )
			{
				char msg[1024];
				
				sprintf( msg, "FileName：%s\nファイルを開き、適当に書き換えても整合エラーとなり読み込みできないようになっています。ファイル保存時の簡易保護にどうぞ。", FILEENCWITHKBD_FILEPATH );
				
				KLAlert_Open("暗号化ファイルの書込完了", msg, "O K", NULL);
			
			}
			else
			{
			
				KLAlert_Open("", "書き込みに失敗しました。", "O K", NULL);
			
			}
			
			sfree(pStr);
			
		}
		
	}
	
	// 復元
	btnw = 128;
	KL_DrawRect( klib.view.w-marginx-btnw, klib.view.h*0.55f, btnw, 64, RGBA(0x00,0x99,0x00,0xFF) );
	KLLabel_Draw( &p->label, "復元", cstrlen("復元"), klib.view.w-marginx-btnw*0.5f, klib.view.h*0.55f+12, 0, RGBA(0x00,0x00,0x00,0xFF), KLLABEL_ALIGN_CENTER);
	
	if ( KLTouch_IsEnd(klib.view.w-marginx-btnw, klib.view.h*0.55f, btnw, 64) )
	{
		
		char* pFileValue = NULL;
		
		// 保存した暗号化ファイルの内容を適当に書き換えてここにくるとハッシュが合わずエラーになる
		if ( KLFile_ReadDocumentsWithHash( FILEENCWITHKBD_FILEPATH, &sz, &pFileValue ) )
		{
			char* pStr = Malloc( sz+1 );
			memset ( pStr, 0, sz+1 );
			memcpy ( pStr, pFileValue, sz );
			
			// 暗号化した時のキーで復元
			KLMath_Decrypt ( pStr, sz, 123, 456, 789, 1011 );
			
			KLAlert_Open ( "ファイル内容", pStr, "O K", NULL);
			
			// アラート以外で使いまわす箇所もないのでここで解放
			sfree (pFileValue);
			sfree (pStr);
			
		}
		else
		{
		
			KLAlert_Open("", "暗号化したファイルの復元に失敗しました。", "O K", NULL);
		
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
void FileEncWithKbd_Init( KLObj* pObj, void* pParam ){
	
	// 引数の pObj は、シーン作成時に KLib が予め確保したすぐに使えるヒープのポインタです。
	// サイズは、sizeof(KLObj) + シーンワークサイズ( KLConfig.h で調整可)分なので、
	// そのサイズを超えない構造体を定義して、それにキャストするだけで１シーン内で自由に扱えます。
	// ※ 確保されているだけで、初期化されてはいませんのでシーン初期化関数内で
	// 　キャスト後の内容を自分で初期化してやる必要があります。
	FileEncWithKbd* p = (FileEncWithKbd*)pObj;
	
	// このシーンは黒いフェードインで開始
	KLScene_FadeIn( NULL, pObj, (Rgba){0,0,0,0}, 0);
	
	// ラベルに使うフォントの初期化
	KLFont_Load(&p->font, KLFont_name[KLFontType_HiraKakuProN_W6], 0);
	
	
	// 文字表示用にラベルを初期化（絵文字はまだ非対応）
	KLLabel_Init(&p->label);
	
	
	// 予め使用する文字をテクスチャにマッピングしておき
	// 描画時にそれらを組み合わせて文字列として表示する
	// なので、ここでテクスチャに含まれない文字は描画できない
	KLLabel_Setup(&p->label,
					 FILEENCWITHKBD_STR, // プリセット以外も追加したければスペースの後ろに定義可
					 &p->font);
	
	
	
	// メインループ関数を切り替える(シーンはこのまま)
	//----------------------------------------------------------

	// 毎メインループで都度この関数が呼ばれ、何度も初期化しても困るので
	// 次のメインループでは違う関数(初期化後の処理)を呼んでもらえるように
	// シーンマネージャにメインループ関数に切り替えを依頼します。
	KLObj_SetFunction( pObj, FileEncWithKbd_Update );
	
	// このシーンのUnload処理を行う関数を設定します。
	// KLScene_ChangeToするとコールされます。
	KLObj_SetUnload( pObj, FileEncWithKbd_Unload );
	
}


