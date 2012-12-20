//
//  FontAndLabel.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "FirstScene.h"
#include "FontAndLabelExample.h"


// 終了処理
static void FontAndLabel_Unload( KLObj* pObj, void* pParam ){

	FontAndLabel* p = (FontAndLabel*)pObj;
	
	for( int i=0; i<LABEL_MAX; ++i )
	{
		KLLabel_Unload( &p->label[i] );
	}
	
}

static void FontAndLabel_FadeOutEnd( KLObj* pObj, void* pParam ){

	KLScene_ChangeTo( FirstScene_Init, NULL );
	
}

// メインループ
static void FontAndLabel_Update( KLObj* pObj, void* pParam ){
	
	FontAndLabel*	p	= (FontAndLabel*)pObj;
	char* str = "Back";
	int i = 0;
	
	app.label.fontsize = 36;
	KLLabel_Draw(&app.label, str, strlen(str), 16, 16, 0, (Rgba){255,255,255,255}, KLLABEL_ALIGN_L);
	
	if( KLTouch_IsStart(0, 0, 128, 32) )
	{
		KLScene_FadeOut( FontAndLabel_FadeOutEnd, pObj, RGBA(0,0,0,0), 0);
	}
	
	
	const char* pFontName [] = {
		
		KLFont_name[KLFontType_ArialMT],
		KLFont_name[KLFontType_Courier],
		KLFont_name[KLFontType_HiraKakuProN_W3],
		KLFont_name[KLFontType_Georgia],
		KLFont_name[KLFontType_HiraKakuProN_W6],
		
	};

	for( i=0; i<LABEL_MAX-1; ++i )
	{
		
		p->label[i].fontsize = 32;
	
		// charが1文字何byteか不定なので描画文字数指定はstrlenではなくcstrlenにする事！strlenはcharの総byte数で文字数ではない
		KLLabel_Draw(&p->label[i], (char*)pFontName[i], cstrlen(pFontName[i]), 16, 96+64*i, 0, RGBA(255,255,255,255), KLLABEL_ALIGN_L);
		
	}
	
	str = "左寄せ";
	p->label[i].fontsize = 32;
	
	// charが1文字何byteか不定なので描画文字数指定はstrlenではなくcstrlenにする事！strlenはcharの総byte数で文字数ではない
	KLLabel_Draw(&p->label[i], str, cstrlen(str), 16, 96+64*i, 0, RGBA(255,255,255,255), KLLABEL_ALIGN_L);

	str = "中央寄せ";
	KLLabel_Draw(&p->label[i], str, cstrlen(str), klib.device.w*0.5f, 96+64*(i+1), 0, RGBA(255,255,255,255), KLLABEL_ALIGN_CENTER);
	
	str = "右寄せ";
	KLLabel_Draw(&p->label[i], str, cstrlen(str), klib.device.w-16, 96+64*(i+2), 0, RGBA(255,255,255,255), KLLABEL_ALIGN_R);
	
	str = "このように1文字ずつ表示する事もできます。途中改行も\nこの通り。改行コードor半角バックスラッシュ+nでも可能です。\\n描画前にフォントサイズを変更する事で文字列全体のサイズも変えることができます。";
	
	if( p->wait++ % 12 ==0 )
	{
		
		p->wait = 0;
		p->clen++;
		
		if( p->clen > cstrlen(str)*2 )
		{
			p->clen = 0;
		}
		
	}
	
	if( !p->scaledir )
	{
		p->fontsize += 1;
		
		if( p->fontsize>200 )
		{
			p->fontsize = 100;
			p->scaledir = 1;
		}
	}
	else
	{
		p->fontsize -= 1;
		
		if( p->fontsize<-100 )
		{
			p->fontsize = -100;
			p->scaledir = 0;
		}
	}
	
	p->label[i].fontsize = p->fontsize>100?100:p->fontsize<14?14:p->fontsize;
	
	KLLabel_Draw(&p->label[i], str, p->clen > cstrlen(str)?cstrlen(str):p->clen, klib.device.w*0.5f, 96+64*(i+4), 0, RGBA(255,255,255,255), KLLABEL_ALIGN_CENTER);
	
	
}

void FontAndLabel_Init( KLObj* pObj, void* pParam ){
	
	// KLibは予め確保したsizeof(KLObj)+シーンワークサイズ(KLConfig.hで調整可)分のヒープを
	// 引数のpObjとして。シーンワークサイズを超えない構造体であれば
	// そのヒープをキャストするだけで自由に扱えます。
	FontAndLabel* p = (FontAndLabel*)pObj;
	
	
	// シーン初期化時に行いたい処理
	//==========================================================
	
	// このシーンは黒いフェードインで開始
	KLScene_FadeIn( NULL, pObj, (Rgba){0,0,0,0}, 0);

	
	
	// フォントとラベルの初期化
	const char* pFontName [] = {
		
		KLFont_name[KLFontType_ArialMT],
		KLFont_name[KLFontType_Courier],
		KLFont_name[KLFontType_HiraKakuProN_W3],
		KLFont_name[KLFontType_Georgia],
		KLFont_name[KLFontType_HiraKakuProN_W6],
		
	};

	for( int i=0; i<LABEL_MAX; ++i )
	{
		
		// ラベル作成後はフォント不要
		if( i )
		{
			KLFont_Unload(&p->font);
		}
		
		KLFont_Load( &p->font, pFontName[i], FALSE );
		KLLabel_Init( &p->label[i] );
		
		// 最後のフォントだけ別例に使うのでフォント名以外のセリフをセットする
		if( i==LABEL_MAX-1 )
		{
			/// サンプル文字列（スプライトにマッピングする文字）この並びのままマッピングされるわけではないので順序は関係なし
			char* pStringExample = "右寄せ左寄せ中央寄せこのように1文字ずつ表示する事もできます。途中改行も\nこの通り。改行コードor半角バックスラッシュ+nでも可能です。\\n描画前にフォントサイズを変更する事で文字列全体のサイズも変えることができます。";
			KLLabel_ToSprite( &p->label[i], pStringExample, &p->font );
		}
		// フォント名の文字をスプライトマッピング
		else
		{
			KLLabel_ToSprite( &p->label[i], (char*)pFontName[i], &p->font );
		}
		
	}
	KLFont_Unload(&p->font);
	
	// 毎メインループで都度この関数が呼ばれて何度も初期化しても困るので
	// 次のメインループでは違う関数(初期化後の処理)をする関数に切り替えます。
	KLObj_SetFunction( pObj, FontAndLabel_Update );
	
	
	// シーンが切り替わる際、このシーンのUnload処理を行う関数を設定します。
	KLObj_SetUnload( pObj, FontAndLabel_Unload );
	
}


