//
//  MapChipTest.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "FirstScene.h"
#include "MapChipExample.h"
#include "KLGL.h"


// 終了処理
static void MapChipTest_Unload( KLObj* pObj, void* pParam ){

	MapChipTest* p = (MapChipTest*)pObj;
	
	// シーン終了時に行いたい処理
	// シーンが持つスプライトはこのシーンでしか使わないのでテクスチャも破棄する
	KLSprite_UnloadWithTexture(&p->sprite);
	
}

static void MapChipTest_FadeOutEnd( KLObj* pObj, void* pParam ){

	KLScene_ChangeTo( FirstScene_Init, NULL );
	
}

// メインループ
static void MapChipTest_Update( KLObj* pObj, void* pParam ){
	
	MapChipTest*	p	= (MapChipTest*)pObj;
	char* str = NULL;
	int x,y,tx,ty;
	
	// 地面のような抜きもアルファも一切必要ないものはブレンドを切った方がFPSがだいぶ稼げる
	glsBlend(FALSE);
	
	for( y=0; y<klib.device.h/32+1; y++ )
	{
		for( x=0; x<klib.device.w/32+1; ++x )
		{
			KLSprite_SetUVFit(&p->sprite, 0, 0, MAPCHIPW, MAPCHIPH);
			KLSprite_SetXY(&p->sprite, x*MAPCHIPW, y*MAPCHIPH);
			KLSprite_Draw(&p->sprite);
		}
	}
	
	// 木など抜きがあるものはブレンドが必要
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
	
	for( y=0; y<klib.device.h/MAPCHIPW+1; ++y )
	{
		for( x=0; x<klib.device.w/MAPCHIPW+1; ++x )
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
	
	
	str = "Back";
	
	app.label.fontsize = 36;
	KLLabel_Draw(&app.label, str, cstrlen(str), 16, 16, 0, (Rgba){255,255,255,255}, KLLABEL_ALIGN_L);
	
	if( KLTouch_IsStart(0, 0, 128, 32) )
	{
		KLScene_FadeOut( MapChipTest_FadeOutEnd, pObj, RGBA(0,0,0,0), 0);
	}
	
	char fpsstr[16];
	sprintf( fpsstr, "FPS:%u", KLFps_Update() );
	KLLabel_Draw(&app.label, fpsstr, cstrlen(fpsstr), klib.device.w-16, 16, 0, RGBA(255,255,255,255), KLLABEL_ALIGN_R);
	
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
	
}


