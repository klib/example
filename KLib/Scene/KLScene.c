//
//  KLibScene.c
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

#include "KLScene.h"
#include "KLDrawQueue.h"
#include "KLib.h"

//==========================================================
/**
 *	@brief	シーンの初期化
 */
//==========================================================
void KLScene_Init(){

	// 指定ワークサイズとスタック数で事前確保
	// ちなみにKLSCENE_WORKSIZEの初期値はだいたいこれくらいあれば足りるだろうという
	// 根拠のない適当な値なので、シーンオブジェクトのサイズがこれを超えるとメモリ破壊が起こる
	KLContainer_Init(	&klib.scene.stack,
					 	sizeof(KLObj)+KLSCENE_WORKSIZE, KLSCENE_STACK_NUM, NULL );
	
}

//==========================================================
/**
 *	@brief	シーンの終了処理
 */
//==========================================================
void KLScene_Unload(){
	
	KLContainer_Unload( &klib.scene.stack, NULL );
	
}

//==========================================================
/**
 *	@brief	シーン変更を反映
 */
//==========================================================
void KLScene_ChangeExecute( KLObjFunc pObjFunc, void* pParam ){
	
	KLScene*	p		= &klib.scene;
	
	if( p->pCurrent )
	{
		KLObj_Free( p->pCurrent );
	}
	
	p->callbackGotoBackground = NULL;
	p->pContext = NULL;
	
	p->pCurrent = KLContainer_Add( &p->stack, pObjFunc, pParam );
	//KLLog("Scene:%p\n", p->pCurrent);
	
}

//==========================================================
/**
 *	@brief	シーンを即座に変更する
 *
 *	@param	pObjFunc	シーン初期化関数へのポインタ
 *	@param	pParam		渡したいパラメータがある場合はそのポインタ
 */
//==========================================================
void KLScene_ChangeTo( KLObjFunc pObjFunc, void* pParam ){
	
	KLScene_ChangeExecute( pObjFunc, pParam );

}

//==========================================================
/**
 *	@brief	シーンをフェードアウトする<br/>
 *			フェード中はタッチ操作を無視するようになる
 *
 *	@param	pFadeEndCallback	フェードアウト終了後のコールバック（ChangeToのように次のシーン関数ではないので注意）
 *	@param	pObj				コールバック時に渡す現在のシーンオブジェクトポインタ
 *	@param	rgba				フェードする色を指定
 *	@param	rate				フェード速度(１フレームあたりのアルファ増加量)を設定する(最大255 0でKLibが設定)
 */
//==========================================================
void KLScene_FadeOut( KLObjFunc pFadeEndCallback, void* pObj, Rgba rgba, float rate ){

	KLScene* p = &klib.scene;
	
	p->inout = -1;
	p->outEffect.type = KLSCENE_EFF_FADE;
	p->outEffect.rgba = (Rgba){rgba.r, rgba.g, rgba.b, 0};
	p->outEffect.rate = rate;
	
	p->outEffect.callback	= pFadeEndCallback;
	p->outEffect.pContext	= pObj;
	
	if( rate==0.0 )
	{
		p->outEffect.rate = 255/15.0f;
	}

}

//==========================================================
/**
 *	@brief	シーンをフェードインする<br/>
 *			フェード中はタッチ操作を無視するようになる
 *
 *	@param	pFadeEndCallback	フェードイン終了後のコールバック（ChangeToのように次のシーン関数ではないので注意）
 *	@param	pObj				コールバック時に渡す現在のシーンオブジェクトポインタ
 *	@param	rgba				フェードする色を指定
 *	@param	rate				フェード速度(１フレームあたりのアルファ減少量)を設定する(最大255 0でKLibが設定)
 */
//==========================================================
void KLScene_FadeIn( KLObjFunc pFadeEndCallback, void* pObj, Rgba rgba, float rate ){
	
	KLScene* p = &klib.scene;
	
	p->inout = 1;
	p->inEffect.type = KLSCENE_EFF_FADE;
	p->inEffect.rgba = (Rgba){rgba.r, rgba.g, rgba.b, 255};
	p->inEffect.rate = rate;
	
	p->inEffect.callback	= pFadeEndCallback;
	p->inEffect.pContext	= pObj;
	
	if( rate==0.0 )
	{
		p->inEffect.rate = 255/15.0f;
	}
	
}

//==========================================================
/**
 *	@brief	シーンを実行する
 *
 *	@param	pParam	渡したいパラメータがある場合はそのポインタ
 */
//==========================================================
void KLScene_Update( void* pParam ){
	
	KLScene* p = &klib.scene;
	
	// フェード中はタッチ操作無効
	if( p->inout )
	{
		KLTouch_Reset();
	}
	
	// フェード更新
	if( p->inout<0 )
	{
		if( p->outEffect.type==KLSCENE_EFF_FADE )
		{
			p->outEffect.rgba.a += p->outEffect.rate;
			
			if( p->outEffect.rgba.a>=255.0 )
			{
				p->outEffect.rgba.a = 255;
				p->inout = 0;
				if( p->outEffect.callback )
				{
					p->outEffect.callback( p->outEffect.pContext, NULL );
				}
			}
		}
	}
	else if( p->inout>0 )
	{
		if( p->inEffect.type==KLSCENE_EFF_FADE )
		{
			p->inEffect.rgba.a -= p->inEffect.rate;
			
			if( p->inEffect.rgba.a<=0 )
			{
				p->inEffect.rgba.a = 0;
				p->inout = 0;
				if( p->inEffect.callback )
				{
					p->inEffect.callback( p->inEffect.pContext, NULL );
				}
			}
		}
	}
	
	// シーン関数呼び出し
	KLContainer_Execute( &klib.scene.stack, pParam );
	
	// フェード描画
	if( p->inout<0 )
	{
		KL_DrawRectScreen( p->outEffect.rgba );
	}
	else if( p->inout>0 )
	{
		KL_DrawRectScreen( p->inEffect.rgba );
	}

		
}

//==========================================================
/**
 *	@brief	アプリがバックグラウンドに移るときのイベントコールバックを設定する<br/>
 *			名前が似ている「KL_***」もあるが、KLScene_***のこちらは現在のシーンのみ有効<br/>
 *			全シーンに適応させたい場合はKL_***の方を使う
 *
 *	@param	callbackGotoBackground	アプリがバックグラウンドに移るときのイベントコールバック
 *	@param	pContext				コンテキストポインタ
 */
//==========================================================
void KLScene_SetBackgroundCallback( void (*callbackGotoBackground)(void* pContext), void* pContext ){

	KLScene*	p		= &klib.scene;
	
	p->callbackGotoBackground = callbackGotoBackground;
	p->pContext = pContext;

}

//==========================================================
/**
 *	@brief	アプリがフォアグラウンドに移るときのイベントコールバックを設定する<br/>
 *			名前が似ている「KL_***」もあるが、KLScene_***のこちらは現在のシーンのみ有効<br/>
 *			全シーンに適応させたい場合はKL_***の方を使う
 *
 *	@param	callbackGotoBackground	アプリがフォアグラウンドに移るときのイベントコールバック
 *	@param	pContext				コンテキストポインタ
 */
//==========================================================
void KLScene_SetForegroundCallback( void (*callbackEnterForeground)(void* pContext), void* pContext ){
	
	KLScene*	p		= &klib.scene;
	
	p->callbackEnterForeground = callbackEnterForeground;
	p->pContextFore = pContext;
	
}

//==========================================================
/**
 *	@brief	シーンワークサイズに収まっているかを確認する
 *
 *	@param	szObj	計測するシーンオブジェクトのサイズ
 *
 *	@return	収まっていればFALSE  / 収まっていなければ不足サイズを返す
 */
//==========================================================
u32 KLScene_IsOverWorkSize( size_t szObj ){
	
	if( KLSCENE_WORKSIZE > (szObj-sizeof(KLObj)) )
	{
		return 0;
	}
	
	return szObj-sizeof(KLObj)-KLSCENE_WORKSIZE;
	
}

