//
//  MapChipTest.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "Game.h"
#include "MapChipExample.h"
#include "KLGL.h"
#include "KLDrawQueue.h"

// 終了処理
static void MapChipTest_Unload( KLObj* pObj, void* pParam ){

	MapChipTest* p = (MapChipTest*)pObj;
	
	// シーン終了時に行いたい処理
	// シーンが持つスプライトはこのシーンでしか使わないのでテクスチャも破棄する
	KLSprite_UnloadWithTexture(&p->sprite);
	KLSprite_UnloadWithTexture(&p->spLogo);
	
	KLLabel_Unload( &p->label );
	
}

// メインループ
static void MapChipTest_Update( KLObj* pObj, void* pParam ){
	
	MapChipTest*	p	= (MapChipTest*)pObj;
	int x,y,tx,ty;
	
	// 地面のような抜きもアルファも一切必要ないものはブレンドを切った方がFPSがだいぶ稼げる
	glsBlend(FALSE);
	
	for( y=0; y<klib.view.h/32+1; y++ )
	{
		for( x=0; x<klib.view.w/32+1; ++x )
		{
			KLSprite_SetUVFit(&p->sprite, 0, 0, MAPCHIPW, MAPCHIPH);
			KLSprite_SetXY(&p->sprite, x*MAPCHIPW, y*MAPCHIPH);
			KLSprite_Draw(&p->sprite);
		}
	}
	
	// 木のように抜きが必要なものにはブレンドを加える
	glsBlend(TRUE);
	
	// 今回は決めうち
	//--------------------------------------------------------------------
	u16 chipid[] = {
		
		0,	0,	50,	51,	0,	0,	0,	0,	41,	26,	44,	45,	46,	0,	3,	0,	52,	53,// nx:18 1
		0,	0,	0,	0,	0,	0,	0,	0,	57,	41,	52,	53,	54,	0,	0,	16,	60,	61,//2
		40,	41,	0,	0,	0,	0,	24,	25,	0,	57,	60,	61,	62,	0,	0,	3,	0,	0,//3
		0,	57,	0,	0,	0,	16,	32,	33,	0,	0,	0,	0,	11,	0,	0,	0,	0,	0,//4
		15,	0,	0,	0,	0,	0,	16,	4,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,//5
		23,	0,	0,	2,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	5,//6
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	8,	0,	0,	0,//7
		43,	0,	0,	0,	0,	0,	3,	0,	0,	0,	0,	0,	0,	4,	0,	0,	0,	0,//8
		51,	0,	0,	0,	0,	0,	0,	0,	0,	17,	18,	0,	0,	0,	0,	0,	0,	0,//9
		0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	65,	66,	67,	68,	69,	70,//10
		0,	0,	65,	66,	67,	68,	69,	0,	0,	0,	0,	6,	73,	74,	75,	76,	77,	78,//11
		0,	72,	73,	74,	75,	76,	77,	78,	79,	0,	0,	80,	81,	82,	83,	84,	85,	86,//12
		0,	80,	81,	82,	83,	84,	85,	86,	87,	0,	0,	88,	89,	90,	91,	92,	93,	94,//13
		1,	88,	89,	90,	91,	92,	93,	94,	95,	0,	0,	96,	97,	98,	99,100,101,102,//14
		0,	96,	97,	98,	99,	100,101,102,103,0,	0,	104,105,106,107,108,109,110,//15
		0,	104,105,106,107,108,109,110,111,0,	0,	112,113,114,115,116,117,118,//16
		0,	112,113,114,115,116,117,118,119,0,	0,	0,	0,	122,123,124,125,  0,//17
		0,	0,	3,	122,123,124,125,6,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,//18
	
	};
	
	int nx = 18;
	
	for( y=0; y<klib.view.h/MAPCHIPW+1; ++y )
	{
		for( x=0; x<klib.view.w/MAPCHIPW+1; ++x )
		{
			if( x<nx && y<18 )
			{
				int c = chipid[y*nx+x];
				tx = c%8;
				ty = c/8;
			}
			else
			{
				tx = 0;
				ty = 0;
			}
			
			if(tx+ty)
			{
				// テクスチャ座標指定後、そのサイズに合わせる
				KLSprite_SetUVFit(&p->sprite, MAPCHIPW*tx, MAPCHIPH*ty, MAPCHIPW*(tx+1), MAPCHIPH*(ty+1));
				
				// 2倍にして表示
				KLSprite_SetWH(&p->sprite, p->sprite.w*2, p->sprite.h*2);
				KLSprite_SetXY(&p->sprite, x*MAPCHIPW*2, y*MAPCHIPH*2);
				KLSprite_Draw(&p->sprite);
			}
		}
	}
	
	p->label.fontsize = 36;
	
	// ビュー内のタッチでクリッピングサンプル
	u64 fingerid	= KLTouch_IsMove(0, 0, klib.view.w, klib.view.h);
	bl	isStart		= !fingerid;
	
	// タッチ開始か
	if ( isStart )
	{
		fingerid = KLTouch_IsTouching (0, 0, klib.view.w, klib.view.h);
		
		if ( fingerid )
		{
			isStart = TRUE;
		}
	}
	
	if ( fingerid )
	{
		KLFinger*	f	= KLTouch_GetFinger( fingerid );
		p->fx	= isStart ? f->startx : f->movex;
		p->fy	= isStart ? f->starty : f->movey;
		
		p->maskAlpha += 12;
		
		if ( p->maskAlpha>255 )
		{
			p->maskAlpha = 255;
		}
		
	}
	else
	{
		
		p->maskAlpha -= 12;
		
		if ( p->maskAlpha<0 )
		{
		
			p->maskAlpha = 0;
			
		}
		
	}
	
	if ( p->maskAlpha )
	{
		// クリッピングエリアに使用するIDをバッファに通達
		KL_PrepareClipping (1);
		
		// クリッピング処理に関するエリアを描画(バッファに描画するので目に見える形では描画されない)
		KL_DrawPolygon( p->fx, p->fy, 480, 0, 360, RGBA(0xFF,0xFF,0xFF,255));
		
		// スプライトでも型取りできる
		KLSprite_SetXY ( &p->spLogo, p->fx - p->spLogo.w * 0.5f, p->fy + 240 );
		KLSprite_Draw( &p->spLogo );
		
		// 以降は上で描画したエリアじゃない所を描画する
		KL_StartClippingDraw( 1, GL_NOTEQUAL );
		
		//KL_DrawRect( klib.view.w*0.5f-200, klib.view.h*0.5f-200, 360, 360, RGBA(0,0,0,255) );
		KL_DrawRectScreen(RGBA(0,0,0,p->maskAlpha));
		
		// クリッピングここまで、以降は普通に描画
		KL_EndClippingDraw();
		
	}
	
	char fpsstr[16];
	sprintf( fpsstr, "FPS:%u", KLFps_Update() );
	KLLabel_Draw(&p->label, fpsstr, cstrlen(fpsstr), klib.view.w-16, 16, 0, RGBA(255,255,255,255), KLLABEL_ALIGN_R);
	
	
}

void MapChipTest_Init( KLObj* pObj, void* pParam ){
	
	// KLibは予め確保したsizeof(KLObj)+シーンワークサイズ(KLConfig.hで調整可)分のヒープを
	// 引数のpObjとして。シーンワークサイズを超えない構造体であれば
	// そのヒープをキャストするだけで自由に扱えます。
	MapChipTest* p = (MapChipTest*)pObj;
	
	
	// シーン初期化時に行いたい処理
	//==========================================================
	
	// このシーンは黒いフェードインで開始
	KLScene_FadeIn( NULL, pObj, (Rgba){0,0,0,0}, 0);
	
	// スプライトを初期化＆読み込み
	KLSprite_LoadFile(&p->sprite, "mori640x480.png"); // Created by http://piposozai.blog76.fc2.com/blog-entry-38.html
		
	// 毎メインループで都度この関数が呼ばれて何度も初期化しても困るので
	// 次のメインループでは違う関数(初期化後の処理)をする関数に切り替えます。
	KLObj_SetFunction( pObj, MapChipTest_Update );
	
	
	// シーンが切り替わる際、このシーンのUnload処理を行う関数を設定します。
	KLObj_SetUnload( pObj, MapChipTest_Unload );
	
	
	
	// FPSを計測
	KLFps_Init();
	
	
	
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
	
	
	
	// マスクサンプルに使うスプライトをロード
	KLSprite_Init(&p->spLogo);
	KLSprite_LoadFile(&p->spLogo, "klib.png");
	
	// マスクの黒い部分のフェード演出に使用するアルファ
	p->maskAlpha = 0;
	
}


