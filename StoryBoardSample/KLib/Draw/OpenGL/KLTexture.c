//
//  KLTexture.c
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

#include "KLTexture.h"
#include "KLSprite.h"

//================================================================
/**
 *	@brief	スプライトにテクスチャUVテーブルを設定する
 *
 *	@param	pSprite		設定先のスプライトポインタ
 *	@param	pUvTable	KLRect配列の先頭ポインタ
 */
//================================================================
inline void KLTexture_Init( KLSprite* pSprite, KLRect* pUvTable ){
	
	pSprite->pUvTable = pUvTable;
	
}

//================================================================
/**
 *	@brief	スプライトにテクスチャUVテーブルからUVを設定する（サイズはそのまま）
 *
 *	@param	pSprite	対象のスプライトポインタ
 *	@param	index	設定済みKLRect配列の何番目のUVを使用するか
 */
//================================================================
inline void KLTexture_SetBounds( KLSprite* pSprite, u16 index ){
	
	KLSprite_SetUV( pSprite,
				   	pSprite->pUvTable[index].x,
				   	pSprite->pUvTable[index].y,
				   	pSprite->pUvTable[index].w,
				   	pSprite->pUvTable[index].h );
	
}

//================================================================
/**
 *	@brief	スプライトにテクスチャUVテーブルからUVを設定しサイズも合わせる
 *
 *	@param	pSprite	対象のスプライトポインタ
 *	@param	index	設定済みKLRect配列の何番目のUVを使用するか
 */
//================================================================
inline void KLTexture_SetBoundsFit( KLSprite* pSprite, u16 index ){
	
	KLSprite_SetUVFit(	pSprite,
					  	pSprite->pUvTable[index].x,
					  	pSprite->pUvTable[index].y,
					  	pSprite->pUvTable[index].w,
					  	pSprite->pUvTable[index].h );
	
}

//================================================================
/**
 *	@brief	スプライトにテクスチャUVテーブルから幅を取得する
 *
 *	@param	pSprite	対象のスプライトポインタ
 *	@param	index	設定済みKLRect配列の何番目のUVを使用するか
 */
//================================================================
inline s32 KLTexture_GetW( KLSprite* pSprite, u16 index ){
	
	return pSprite->pUvTable[index].w - pSprite->pUvTable[index].x;
	
}

//================================================================
/**
 *	@brief	スプライトにテクスチャUVテーブルから高さを取得する
 *
 *	@param	pSprite	対象のスプライトポインタ
 *	@param	index	設定済みKLRect配列の何番目のUVを使用するか
 */
//================================================================
inline s32 KLTexture_GetH( KLSprite* pSprite, u16 index ){
	
	return pSprite->pUvTable[index].h - pSprite->pUvTable[index].y;
	
}
