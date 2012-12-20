//
//  KLib.cpp
//  KLib
//
// Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//
// KLib MIT Lisence
//#####################################################################
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
// 以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェア」）の複製を取得する
// すべての人に対し、ソフトウェアを無制限に扱うことを無償で許可します。
// これには、ソフトウェアの複製を使用、複写、変更、結合、掲載、頒布、サブライセンス、
// および/または販売する権利、およびソフトウェアを提供する相手に同じことを許可する権利も無制限に含まれます。
// 上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に記載するものとします。
// ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もなく提供されます。
// ここでいう保証とは、商品性、特定の目的への適合性、および権利非侵害についての保証も含みますが、
// それに限定されるものではありません。
// 作者または著作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフトウェアに起因または関連し、
// あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損害、その他の義務について
// 何らの責任も負わないものとします。
//
//#####################################################################

#include <math.h>
#include <string.h>
#include "KLib.h"
#include "KLDevice.h"
#include "KLScreen.h"
#include "KLDrawQueue.h"
#include "KLList.h"
#include "KLRandom.h"
#include "KLLabel.h"
#include "KLString.h"
#include "KLUUID.h"
#include "KLAL.h"
#include "stb_image.h"

/// ライブラリオブジェクト
KLib klib;

/// 事前計算cos
static float kl_cos[KL_WDEG_CYCLE];

/// 事前計算sin
static float kl_sin[KL_WDEG_CYCLE];


// P R O T T Y P E
//=============================================
static void KL_InitSinCos();

//=============================================
/**
 *	@brief	ライブラリの初期化
 */
//=============================================
void KL_Init(){

	KLShaderInfo shader = klib.shader;
	
	memset( &klib, 0, sizeof(KLib) );
	
	klib.fps	= 60.0f;
	klib.shader	= shader;
	
	stbi_convert_iphone_png_to_rgb(1);
	KLList_InitForKLib();
	KLDevice_Init();
	KLDevice_GetOrientationWhenBoot();
	KLDevice_InitScreenSize( KLDevice_IsPortrait() );
	KLPath_Init();
	KL_InitSinCos();
	KLGL_Init(klib.shader.program.handler);
	KLScreen_InitWH( NULL, KL_GetDeviceW(0), KL_GetDeviceH(0) );
	//KLScreen_SetCamera2D( NULL );
	KLScene_Init();
	KLDrawQueue_Init(0);
	KLIndicator_Init();
	KLLabel_InitSystem();
	KLAL_InitSystem();
	
	KLLog("KLib loaded.\n");
	
}

//=============================================
/**
 *	@brief	ライブラリの終了処理
 */
//=============================================
void KL_Unload(){
	
	if( klib.func.callbackAppTerminate )
	{
		klib.func.callbackAppTerminate( klib.func.pContextTerminate );
	}
	
	KLAL_UnloadSystem();
	KLLabel_UnloadSystem();
	KLScene_Unload();
	KLString_UnloadSystem();
	KLDrawQueue_Unload();
	KLRandom_Unload();
	KLGameCenter_Unload();
	
	if( klib.device.autolockdisabled )
	{
		KLDevice_EnableAutoLock();
	}
	
	KLLog("KLib unloaded.");

}

//=============================================
/**
 *	@brief	sin cosの事前計算
 */
//=============================================
static void KL_InitSinCos(){
	
	u16 hdeg = KL_WDEG_CYCLE*0.5;
	
	for( int i=0; i<KL_WDEG_CYCLE; ++i ){
		
		kl_cos[i] = cos(PI * i / hdeg);
		kl_sin[i] = sin(PI * i / hdeg);
		
	}
	
}

//=============================================
/**
 *	@brief	事前計算済みのcosを取得
 *
 *	@param	widedeg	360°を0x10000で表した数値
 */
//=============================================
inline float KL_Cos(wdeg widedeg){
	
	return kl_cos[widedeg];
	
}

//=============================================
/**
 *	@brief	事前計算済みのsinを取得
 *
 *	@param	widedeg	360°を0x10000で表した数値
 */
//=============================================
inline float KL_Sin(wdeg widedeg){
	
	return kl_sin[widedeg];
	
}

//=============================================
/**
 *	@brief	dirをX方向に反転したKLPowerを返す
 *
 *	@param	pow	計算元になるKLPower
 *
 *	@return	計算後のKLPower(引数のpowは変化しない)
 */
//=============================================
KLPower KLPower_ReflectDirX( KLPower pow ){

	// 下弦
	if( pow.dir>=0 && pow.dir<=KL_WDEG_CYCLE*0.5f )
	{
		
		pow.dir = KL_WDEG_CYCLE*0.5f - pow.dir;
	
	}
	// 上弦
	else
	{
		
		pow.dir = KL_WDEG_CYCLE*0.75 - pow.dir + KL_WDEG_CYCLE*0.75;
		
	}

	return pow;
	
}

//=============================================
/**
 *	@brief	dirをY方向に反転したKLPowerを返す
 *
 *	@param	pow	計算元になるKLPower
 *
 *	@return	計算後のKLPower(引数のpowは変化しない)
 */
//=============================================
KLPower KLPower_ReflectDirY( KLPower pow ){
	
	// 左弦
	if( pow.dir>=KL_WDEG_CYCLE*0.25f && pow.dir<=KL_WDEG_CYCLE*0.75f )
	{
		
		pow.dir = KL_WDEG_CYCLE*0.75f - pow.dir + KL_WDEG_CYCLE*0.25f;
		
	}
	// 右弦
	else
	{
		
		pow.dir = KL_WDEG_CYCLE - pow.dir;
		
	}
	
	return pow;
	
}

//==========================================================
/**
 *	@brief	アプリが終了する(KLibアンロード)時のイベントコールバックを設定する
 *
 *	@param	callbackAppTerminate	アプリが終了するときのイベントコールバック
 *	@param	pContext				コンテキストポインタ
 */
//==========================================================
void KL_SetUnloadCallback( void (*callbackAppTerminate)(void* pContext), void* pContext ){

	KLFunc*	p		= &klib.func;
	
	p->callbackAppTerminate = callbackAppTerminate;
	p->pContextTerminate	= pContext;
	
}

//==========================================================
/**
 *	@brief	アプリがバックグラウンドに移るときのイベントコールバックを設定する<br/>
 *			名前が似ている「KLScene_***」もあるが、KL_***のこちらはアプリ内すべてで有効<br/>
 *			あるシーンだけに適応させたい場合はKLScene_***の方を使う
 *
 *	@param	callbackGotoBackground	アプリがバックグラウンドに移るときのイベントコールバック
 *	@param	pContext				コンテキストポインタ
 */
//==========================================================
void KL_SetBackgroundCallback( void (*callbackGotoBackground)(void* pContext), void* pContext ){
	
	KLFunc*	p		= &klib.func;
	
	p->callbackGotoBackground = callbackGotoBackground;
	p->pContext = pContext;
	
}

//==========================================================
/**
 *	@brief	アプリがフォアグラウンドに移るときのイベントコールバックを設定する<br/>
 *			名前が似ている「KLScene_***」もあるが、KL_***のこちらはアプリ内すべてで有効<br/>
 *			あるシーンだけに適応させたい場合はKLScene_***の方を使う
 *
 *	@param	callbackEnterForeground	アプリがフォアグラウンドに移るときのイベントコールバック
 *	@param	pContext				コンテキストポインタ
 */
//==========================================================
void KL_SetForegroundCallback( void (*callbackEnterForeground)(void* pContext), void* pContext ){
	
	KLFunc*	p		= &klib.func;
	
	p->callbackEnterForeground = callbackEnterForeground;
	p->pContextFore = pContext;
	
}

