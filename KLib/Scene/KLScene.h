//
//  KLibScene.h
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

#ifndef KLib_KLScene_h
#define KLib_KLScene_h

#include "c_extern.h"
#include "KLObj.h"

/// シーン最大ワークサイズ(シーンサイズがこのサイズを超えるとメモリ破壊の可能性があるので要注意)
#define KLSCENE_WORKSIZE (1024*150)


/// シーン切り替えスタック数
#define KLSCENE_STACK_NUM (2)

typedef enum{

	KLSCENE_EFF_NONE,	///< None
	KLSCENE_EFF_FADE,	///< FadeIn FadeOut
	
}KLSceneEffectType;

/// KLSceneEffect
typedef struct{

	u8			type;	///< タイプ
	Rgba		rgba;	///< 色情報が必要な場合の色
	float		rate;	///< 変化レート
	KLObjFunc	callback;
	void*		pContext;
	
}KLSceneEffect;

/// KLScene
typedef struct{

	KLObj			objcore;	///< KLibObjとして扱うための継承的な事
	
	KLContainer		stack;		///< シーンスタック
	KLObj*			pCurrent;	///< 現在のシーン
	
	KLSceneEffect	inEffect;	///< シーンに入る時の効果
	KLSceneEffect	outEffect;	///< シーンを出る時の効果
	
	s8				inout;		///< シーンIN(1)かOUT(-1)か
	
	void (*callbackGotoBackground)(void* pContext);
	void (*callbackEnterForeground)(void* pContext);
	void* pContext, *pContextFore;
	
}KLScene;

void KLScene_Init();
void KLScene_Unload();
void KLScene_SetBackgroundCallback( void (*callbackGotoBackground)(void* pContext), void* pContext );
void KLScene_SetForegroundCallback( void (*callbackEnterForeground)(void* pContext), void* pContext );

void KLScene_Update( void* pParam );

u32 KLScene_IsOverWorkSize( size_t szObj );

void KLScene_ChangeTo( KLObjFunc pObjFunc, void* pParam );

void KLScene_FadeIn( KLObjFunc pFadeEndCallback, void* pObj, Rgba rgba, float rate );
void KLScene_FadeOut( KLObjFunc pFadeEndCallback, void* pObj, Rgba rgba, float rate );

#include "c_extern_close.h"
#endif
