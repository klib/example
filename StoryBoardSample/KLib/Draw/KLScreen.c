//
//  KLScreen.c
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

#include <stdio.h>
#include "KLib.h"
#include "KLGL.h"
#include "KLScreen.h"

/// 設定毎の指定が面倒な時は予めこれにセットしてから関数にはNULLを指定する
KLScreen* kl_screen = NULL;

/// パラメータにNULLを入れた時の代役設定
#define KLScreen_SafeNull(pScreen) \
	if( !pScreen ){ pScreen = &klib.screen; }

//=============================================
/**
 *	@brief	初期化
 */
//=============================================
bl KLScreen_Init(KLScreen* pScreen){
	
	KLScreen_SafeNull(pScreen);
	
	return 1;
}

//=============================================
/**
 *	@brief	初期化
 */
//=============================================
bl KLScreen_InitWH(KLScreen* pScreen, u32 width, u32 height){
	
	KLScreen_SafeNull(pScreen);
	
	if( KLScreen_Init(pScreen) ){
		
		KLScreen_SetWH( pScreen, width, height );
		return 1;
		
	}
	
	return 0;
	
}

//=============================================
/**
 *	@brief	幅・高さの設定
 */
//=============================================
void KLScreen_SetWH( KLScreen* pScreen, int width, int height ){
	
	KLScreen_SafeNull(pScreen);
	
	pScreen->w = width;
	pScreen->h = height;
	
}

//=============================================
/**
 *	@brief	2Dカメラを設定
 */
//=============================================
void KLScreen_SetCamera2D( KLScreen* pScreen ){

	KLScreen_SafeNull(pScreen);
	
	glsMatrixMode(KLMAT_PROJECTION);
	glsViewport(	0, 0, pScreen->w, pScreen->h );
	glsScissor(	0, 0, pScreen->w, pScreen->h );
	glsOrthMatrix( 0, pScreen->w, 0, pScreen->h, -1, 1, 0, 0 );
	
}
