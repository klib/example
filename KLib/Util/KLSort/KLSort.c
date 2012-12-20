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

#include <stdlib.h>
#include "KLSort.h"
#include "KLMath.h"
//=========================================================
/**
 *	@brief	Signed 32bit変数配列をソートする
 *
 *	@param	pArray	[ in/out ] ソートするSigned 32bit配列のポインタ
 *	@param	nCount	配列のサイズ
 */
//=========================================================
void KLSort_s32( s32* pArray, size_t nCount ){
	
	qsort( pArray, nCount, sizeof(s32), KLSort_cmps32 );

}

//=========================================================
/**
 *	@brief	Unsigned 32bit変数配列をソートする
 *
 *	@param	pArray	[ in/out ] ソートするUnsigned 32bit配列のポインタ
 *	@param	nCount	配列のサイズ
 */
//=========================================================
void KLSort_u32( u32* pArray, size_t nCount ){
	
	qsort( pArray, nCount, sizeof(u32), KLSort_cmpu32 );
	
}
//=========================================================
/**
 *	@brief	Signed 16bit変数配列をソートする
 *
 *	@param	pArray	[ in/out ] ソートするSigned 16bit配列のポインタ
 *	@param	nCount	配列のサイズ
 */
//=========================================================
void KLSort_s16( s16* pArray, size_t nCount ){
	
	qsort( pArray, nCount, sizeof(s16), KLSort_cmps16 );
	
}
//=========================================================
/**
 *	@brief	Unsigned 16bit変数配列をソートする
 *
 *	@param	pArray	[ in/out ] ソートするUnsigned 16bit配列のポインタ
 *	@param	nCount	配列のサイズ
 */
//=========================================================
void KLSort_u16( u16* pArray, size_t nCount ){
	
	qsort( pArray, nCount, sizeof(u16), KLSort_cmpu16 );
	
}
//=========================================================
/**
 *	@brief	Signed 8bit変数配列をソートする
 *
 *	@param	pArray	[ in/out ] ソートするSigned 8bit配列のポインタ
 *	@param	nCount	配列のサイズ
 */
//=========================================================
void KLSort_s8( s8* pArray, size_t nCount ){
	
	qsort( pArray, nCount, sizeof(s8), KLSort_cmps8 );
	
}
//=========================================================
/**
 *	@brief	Unsigned 8bit変数配列をソートする
 *
 *	@param	pArray	[ in/out ] ソートするUnsigned 8bit配列のポインタ
 *	@param	nCount	配列のサイズ
 */
//=========================================================
void KLSort_u8( u8* pArray, size_t nCount ){
	
	qsort( pArray, nCount, sizeof(u8), KLSort_cmpu8 );
	
}

//=========================================================================
/**
 *	@brief	Signed 32bit の変数値を比較
 *
 *	@param	pValueA	比較値A
 *	@param	pValueB	比較値B
 *
 *	@return	A>Bなら1 / A<Bなら-1 / A==Bなら0
 */
//=========================================================================
s32 KLSort_cmps32( const void *pValueA, const void *pValueB ){
	
	s32 a = *(s32*)pValueA;
	s32 b = *(s32*)pValueB;
	
	if( a < b )
	{
		return -1;
	}
	else if( a > b )
	{
		return 1;
	}
	
	return 0;
	
}
//=========================================================================
/**
 *	@brief	Unsigned 32bit の変数値を比較
 *
 *	@param	pValueA	比較値A
 *	@param	pValueB	比較値B
 *
 *	@return	A>Bなら1 / A<Bなら-1 / A==Bなら0
 */
//=========================================================================
s32 KLSort_cmpu32( const void *pValueA, const void *pValueB ){
	
	u32 a = *(u32*)pValueA;
	u32 b = *(u32*)pValueB;
	
	if( a < b )
	{
		return -1;
	}
	else if( a > b )
	{
		return 1;
	}
	
	return 0;
	
}
//=========================================================================
/**
 *	@brief	Signed 16bit の変数値を比較
 *
 *	@param	pValueA	比較値A
 *	@param	pValueB	比較値B
 *
 *	@return	A>Bなら1 / A<Bなら-1 / A==Bなら0
 */
//=========================================================================
s32 KLSort_cmps16( const void *pValueA, const void *pValueB ){
	
	s16 a = *(s16*)pValueA;
	s16 b = *(s16*)pValueB;
	
	if( a < b )
	{
		return -1;
	}
	else if( a > b )
	{
		return 1;
	}
	
	return 0;
	
}
//=========================================================================
/**
 *	@brief	Unsigned 16bit の変数値を比較
 *
 *	@param	pValueA	比較値A
 *	@param	pValueB	比較値B
 *
 *	@return	A>Bなら1 / A<Bなら-1 / A==Bなら0
 */
//=========================================================================
s32 KLSort_cmpu16( const void *pValueA, const void *pValueB ){
	
	u16 a = *(u16*)pValueA;
	u16 b = *(u16*)pValueB;
	
	if( a < b )
	{
		return -1;
	}
	else if( a > b )
	{
		return 1;
	}
	
	return 0;
	
}
//=========================================================================
/**
 *	@brief	Signed 8bit の変数値を比較
 *
 *	@param	pValueA	比較値A
 *	@param	pValueB	比較値B
 *
 *	@return	A>Bなら1 / A<Bなら-1 / A==Bなら0
 */
//=========================================================================
s32 KLSort_cmps8( const void *pValueA, const void *pValueB ){
	
	s8 a = *(s8*)pValueA;
	s8 b = *(s8*)pValueB;
	
	if( a < b )
	{
		return -1;
	}
	else if( a > b )
	{
		return 1;
	}
	
	return 0;
	
}
//=========================================================================
/**
 *	@brief	Unsigned 8bit の変数値を比較
 *
 *	@param	pValueA	比較値A
 *	@param	pValueB	比較値B
 *
 *	@return	A>Bなら1 / A<Bなら-1 / A==Bなら0
 */
//=========================================================================
s32 KLSort_cmpu8( const void *pValueA, const void *pValueB ){
	
	u8 a = *(u8*)pValueA;
	u8 b = *(u8*)pValueB;
	
	if( a < b )
	{
		return -1;
	}
	else if( a > b )
	{
		return 1;
	}
	
	return 0;
	
}


