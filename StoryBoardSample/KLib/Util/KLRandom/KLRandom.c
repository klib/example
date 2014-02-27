//
//  KLRandom.c
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

#include "KLrandom.h"
#include <string.h>

// XorShiftを使用しています。
/// 乱数テーブル(アプリ毎に異なる適当な数値を設定する事)
static u32 _kl_rs[4]		= {KLRANDOM_KEY_A,KLRANDOM_KEY_B,KLRANDOM_KEY_C,KLRANDOM_KEY_D};
// ※ハッシュ算出にもこの乱数を使用しているので、
//  アプリリリース後にテーブルやアルゴリズムを変更すると
//  アップデート後にアップデート前のハッシュと不整合が起こる危険性があります。

/// 乱数テーブルバックアップバッファ(変更不要)
static u32 _kl_rs_back[4]	= {0,0,0,0};

//=================================================================
/**
 *	@brief	符号なし64bitの数値で乱数を返す
 */
//=================================================================
u32 KL_rand(){
	
	u32 t		= ( _kl_rs[0]^(_kl_rs[0]<<11) );
	_kl_rs[0]	= _kl_rs[1];
	_kl_rs[1]	= _kl_rs[2];
	_kl_rs[2]	= _kl_rs[3];
	return( _kl_rs[3] = (_kl_rs[3]^(_kl_rs[3]>>19))^(t^(t>>8)) );

}

//=================================================================
/**
 *	@brief	4つの数値でランダムシードを設定する
 */
//=================================================================
inline void KL_srand( u32 x, u32 y, u32 z, u32 w ){
	
	_kl_rs[0] = x; _kl_rs[1] = y;
	_kl_rs[2] = z; _kl_rs[3] = w;

}

//=================================================================
/**
 *	@brief	現在のランダムシードをバックアップする<br/>
 *			※バックアップはアプリの起動している間だけ有効
 *			 2回Saveすると後ろのものが優先して上書きされるので注意
 */
//=================================================================
inline void KL_SaveRandomSeed(){
	
	_kl_rs_back[0] = _kl_rs[0];
	_kl_rs_back[1] = _kl_rs[1];
	_kl_rs_back[2] = _kl_rs[2];
	_kl_rs_back[3] = _kl_rs[3];
	
}

//=================================================================
/**
 *	@brief	バックアップしたランダムシードを復元する<br/>
 *			※バックアップはアプリの起動している間だけ有効
 */
//=================================================================
inline void KL_LoadRandomSeed(){
	
	_kl_rs[0] = _kl_rs_back[0];
	_kl_rs[1] = _kl_rs_back[1];
	_kl_rs[2] = _kl_rs_back[2];
	_kl_rs[3] = _kl_rs_back[3];
	
}

static s32* KLRandom_pSeq = NULL;
static u32	KLRandom_nSeq = 0;

//=================================================================
/**
 *	@brief	指定数から連続した数をランダムで配列に設定する
 *
 *	@param	pAry	出力先配列の先頭ポインタ
 *	@param	nAry	pAryの要素数
 *	@param	start	連番の開始（最小）数
 */
//=================================================================
s32* KLRandom_Sequence( s32* pAry, u32 nAry, s32 start ){

	u16		rest	= nAry;
	s32*	a		= NULL;
	
	if( KLRandom_nSeq<nAry )
	{
		KLRandom_pSeq = (s32*)realloc( KLRandom_pSeq, sizeof(s32)*nAry );
		KLRandom_nSeq = nAry;
	}
	
	a = KLRandom_pSeq;
	
	for( int i=0; i<nAry; ++i )
	{
		a[i] = start+i;
	}
	
	for( int i=0; i<nAry; ++i )
	{
		u32 c = KL_rand()%rest;
		
		pAry[i] = a[c];
		
		if( c!=rest-1 )
		{
			memmove( &a[c], &a[c+1], (rest-c-1)*sizeof(s32) );
		}
		
		--rest;
		
	}
	
	return pAry;
	
}

void KLRandom_Unload(){
	
	sfree(KLRandom_pSeq);
	
}