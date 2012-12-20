//
//  KLMatrix.c
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
#include "KLibDefine.h"
#include "KLMatrix.h"

//===========================================================
/**
 *	@brief	指定した行列をゼロ行列にする
 *	
 *	@param	pMat	処理する行列のポインタ
 */
//===========================================================
void KLMat4_Zero( KLMat4* pMat ){
	
	pMat->p[ 0] = pMat->p[ 1] = pMat->p[ 2] = pMat->p[ 3] =
	pMat->p[ 4] = pMat->p[ 5] = pMat->p[ 6] = pMat->p[ 7] =
	pMat->p[ 8] = pMat->p[ 9] = pMat->p[10] = pMat->p[11] =
	pMat->p[12] = pMat->p[13] = pMat->p[14] = pMat->p[15] = 0;
	
}

//===========================================================
/**
 *	@brief	ゼロ行列を取得する
 *
 *	@return	作成した行列
 */
//===========================================================
KLMat4 KLMat4_GetZero(){
	
	KLMat4 mat;
	KLMat4_Zero(&mat);
	return mat;
	
}

//===========================================================
/**
 *	@brief	行列を比較する
 *
 *	@param	pMatA	比較行列A
 *	@param	pMatB	比較行列B
 *
 *	@return A>Bなら1 ・ 同じなら0 ・ A<Bなら-1を返す
 */
//===========================================================
bl KLMat4_Cmp( KLMat4* pMatA, KLMat4* pMatB ){

	return memcmp( pMatA, pMatB, sizeof(KLMat4) ) != 0;
	
}

//===========================================================
/**
 *	@brief	行列が同じか比較する
 *
 *	@param	pMatA	比較行列A
 *	@param	pMatB	比較行列B
 *
 *	@return 同じならTRUE ・ 異なるならFALSE
 */
//===========================================================
bl KLMat4_IsSame( KLMat4* pMatA, KLMat4* pMatB ){
	
	return KLMat4_Cmp( pMatA, pMatB )==0;
}

//===========================================================
/**
 *	@brief	単位行列化する
 *
 *	@param	pMat	処理する行列のポインタ
 */
//===========================================================
void KLMat4_Identity( KLMat4* pMat ){
	
					pMat->p[ 1] =	pMat->p[ 2] =	pMat->p[ 3] =
	pMat->p[ 4] =					pMat->p[ 6] =	pMat->p[ 7] =
	pMat->p[ 8] =	pMat->p[ 9] =					pMat->p[11] =
	pMat->p[12] =	pMat->p[13] =	pMat->p[14] =					0;

	//-----------------------------------------
	
	pMat->p[ 0] =
					pMat->p[ 5] =
									pMat->p[10] =
													pMat->p[15] =	1;

}

//===========================================================
/**
 *	@brief	単位行列を取得する
 *
 *	@return	作成した行列
 */
//===========================================================
KLMat4 KLMat4_GetIdentity(){
	
	KLMat4 mat;
	KLMat4_Identity(&mat);
	return mat;
	
}

//===========================================================
/**
 *	@brief	行列をコピーする
 *
 *	@param	pDest	コピー先のポインタ
 *	@param	pSrc	コピー元のポインタ
 */
//===========================================================
void KLMat4_Copy( KLMat4* pDest, KLMat4* pSrc ){
	
	pDest[0] = pSrc[0];
	pDest[1] = pSrc[1];
	pDest[2] = pSrc[2];
	pDest[3] = pSrc[3];
	
	pDest[4] = pSrc[4];
	pDest[5] = pSrc[5];
	pDest[6] = pSrc[6];
	pDest[7] = pSrc[7];
	
	pDest[8] = pSrc[8];
	pDest[9] = pSrc[9];
	pDest[10] = pSrc[10];
	pDest[11] = pSrc[11];
		
	pDest[12] = pSrc[12];
	pDest[13] = pSrc[13];
	pDest[14] = pSrc[14];
	pDest[15] = pSrc[15];
	
}

//===========================================================
/**
 *	@brief	行列を加算した結果の行列を返す
 *
 *	@param	pMatA	行列1
 *	@param	pMatB	行列2
 *
 *	@return	計算結果の行列
 */
//===========================================================
KLMat4 KLMat4_Add( KLMat4* pMatA, KLMat4* pMatB ){
	
	KLMat4 ret;
	
	ret.p[0]	= pMatA->p[0]+pMatB->p[0];
	ret.p[1]	= pMatA->p[1]+pMatB->p[1];
	ret.p[2]	= pMatA->p[2]+pMatB->p[2];
	ret.p[3]	= pMatA->p[3]+pMatB->p[3];
	
	ret.p[4]	= pMatA->p[4]+pMatB->p[4];
	ret.p[5]	= pMatA->p[5]+pMatB->p[5];
	ret.p[6]	= pMatA->p[6]+pMatB->p[6];
	ret.p[7]	= pMatA->p[7]+pMatB->p[7];
	
	ret.p[ 8]	= pMatA->p[ 8]+pMatB->p[ 8];
	ret.p[ 9]	= pMatA->p[ 9]+pMatB->p[ 9];
	ret.p[10]	= pMatA->p[10]+pMatB->p[10];
	ret.p[11]	= pMatA->p[11]+pMatB->p[11];
	
	ret.p[12]	= pMatA->p[12]+pMatB->p[12];
	ret.p[13]	= pMatA->p[13]+pMatB->p[13];
	ret.p[14]	= pMatA->p[14]+pMatB->p[14];
	ret.p[15]	= pMatA->p[15]+pMatB->p[15];
	
	return ret;
	
}

//===========================================================
/**
 *	@brief	行列を減算した結果の行列を返す
 *
 *	@param	pMatA	行列1
 *	@param	pMatB	行列2
 *
 *	@return	計算結果の行列
 */
//===========================================================
KLMat4 KLMat4_Sub( KLMat4* pMatA, KLMat4* pMatB ){
	
	KLMat4 ret;
	
	ret.p[0]	= pMatA->p[0]-pMatB->p[0];
	ret.p[1]	= pMatA->p[1]-pMatB->p[1];
	ret.p[2]	= pMatA->p[2]-pMatB->p[2];
	ret.p[3]	= pMatA->p[3]-pMatB->p[3];
	
	ret.p[4]	= pMatA->p[4]-pMatB->p[4];
	ret.p[5]	= pMatA->p[5]-pMatB->p[5];
	ret.p[6]	= pMatA->p[6]-pMatB->p[6];
	ret.p[7]	= pMatA->p[7]-pMatB->p[7];
	
	ret.p[ 8]	= pMatA->p[ 8]-pMatB->p[ 8];
	ret.p[ 9]	= pMatA->p[ 9]-pMatB->p[ 9];
	ret.p[10]	= pMatA->p[10]-pMatB->p[10];
	ret.p[11]	= pMatA->p[11]-pMatB->p[11];
	
	ret.p[12]	= pMatA->p[12]-pMatB->p[12];
	ret.p[13]	= pMatA->p[13]-pMatB->p[13];
	ret.p[14]	= pMatA->p[14]-pMatB->p[14];
	ret.p[15]	= pMatA->p[15]-pMatB->p[15];
	
	return ret;
	
}

//===========================================================
/**
 *	@brief	行列を乗算した結果の行列を返す
 *
 *	@param	pMatA	行列1
 *	@param	pMatB	行列2
 *
 *	@return	計算結果の行列
 */
//===========================================================
KLMat4 KLMat4_Mul( KLMat4* pMatA, KLMat4* pMatB ){
	
	KLMat4 ret;
	
	ret.p[0]	=	pMatA->p[0] * pMatB->p[ 0] +
					pMatA->p[1] * pMatB->p[ 4] +
					pMatA->p[2] * pMatB->p[ 8] +
					pMatA->p[3] * pMatB->p[12];
	
	ret.p[1]	=	pMatA->p[0] * pMatB->p[ 1] +
					pMatA->p[1] * pMatB->p[ 5] +
					pMatA->p[2] * pMatB->p[ 9] +
					pMatA->p[3] * pMatB->p[13];
	
	ret.p[2]	=	pMatA->p[0] * pMatB->p[ 2] +
					pMatA->p[1] * pMatB->p[ 6] +
					pMatA->p[2] * pMatB->p[10] +
					pMatA->p[3] * pMatB->p[14];
	
	ret.p[3]	=	pMatA->p[0] * pMatB->p[ 3] +
					pMatA->p[1] * pMatB->p[ 7] +
					pMatA->p[2] * pMatB->p[11] +
					pMatA->p[3] * pMatB->p[15];
			
	
	ret.p[4]	=	pMatA->p[4] * pMatB->p[ 0] +
					pMatA->p[5] * pMatB->p[ 4] +
					pMatA->p[6] * pMatB->p[ 8] +
					pMatA->p[7] * pMatB->p[12];
	
	ret.p[5]	=	pMatA->p[4] * pMatB->p[ 1] +
					pMatA->p[5] * pMatB->p[ 5] +
					pMatA->p[6] * pMatB->p[ 9] +
					pMatA->p[7] * pMatB->p[13];
	
	ret.p[6]	=	pMatA->p[4] * pMatB->p[ 2] +
					pMatA->p[5] * pMatB->p[ 6] +
					pMatA->p[6] * pMatB->p[10] +
					pMatA->p[7] * pMatB->p[14];
					
	ret.p[7]	=	pMatA->p[4] * pMatB->p[ 3] +
					pMatA->p[5] * pMatB->p[ 7] +
					pMatA->p[6] * pMatB->p[11] +
					pMatA->p[7] * pMatB->p[15];
	
	
	ret.p[8]	=	pMatA->p[8] * pMatB->p[ 0] +
					pMatA->p[9] * pMatB->p[ 4] +
					pMatA->p[10] * pMatB->p[ 8] +
					pMatA->p[11] * pMatB->p[12];
				
	ret.p[9]	=	pMatA->p[8] * pMatB->p[ 1] +
					pMatA->p[9] * pMatB->p[ 5] +
					pMatA->p[10] * pMatB->p[ 9] +
					pMatA->p[11] * pMatB->p[13];
					
	ret.p[10]	=	pMatA->p[8] * pMatB->p[ 2] +
					pMatA->p[9] * pMatB->p[ 6] +
					pMatA->p[10] * pMatB->p[10] +
					pMatA->p[11] * pMatB->p[14];
					
	ret.p[11]	=	pMatA->p[8] * pMatB->p[ 3] +
					pMatA->p[9] * pMatB->p[ 7] +
					pMatA->p[10] * pMatB->p[11] +
					pMatA->p[11] * pMatB->p[15];
	
	
	ret.p[12]	=	pMatA->p[12] * pMatB->p[ 0] +
					pMatA->p[13] * pMatB->p[ 4] +
					pMatA->p[14] * pMatB->p[ 8] +
					pMatA->p[15] * pMatB->p[12];
					
	ret.p[13]	=	pMatA->p[12] * pMatB->p[ 1] +
					pMatA->p[13] * pMatB->p[ 5] +
					pMatA->p[14] * pMatB->p[ 9] +
					pMatA->p[15] * pMatB->p[13];
					
	ret.p[14]	=	pMatA->p[12] * pMatB->p[ 2] +
					pMatA->p[13] * pMatB->p[ 6] +
					pMatA->p[14] * pMatB->p[10] +
					pMatA->p[15] * pMatB->p[14];
					
	ret.p[15]	=	pMatA->p[12] * pMatB->p[ 3] +
					pMatA->p[13] * pMatB->p[ 7] +
					pMatA->p[14] * pMatB->p[11] +
					pMatA->p[15] * pMatB->p[15];
					
	return ret;
	
}

//===========================================================
/**
 *	@brief	行列の全要素に均一の係数をかける
 *
 *	@param	pMatA	行列
 *	@param	scale	係数
 *
 *	@return	計算結果の行列
 */
//===========================================================
KLMat4 KLMat4_Scaling( KLMat4* pMatA, GLfloat scale ){
	
	KLMat4		ret = *pMatA;
	
	ret.p[ 0] *= scale; 	ret.p[ 1] *= scale; 	ret.p[ 2] *= scale; 	ret.p[ 3] *= scale;
	ret.p[ 4] *= scale; 	ret.p[ 5] *= scale; 	ret.p[ 6] *= scale; 	ret.p[ 7] *= scale;
	ret.p[ 8] *= scale; 	ret.p[ 9] *= scale; 	ret.p[10] *= scale; 	ret.p[11] *= scale;
	ret.p[12] *= scale; 	ret.p[13] *= scale; 	ret.p[14] *= scale; 	ret.p[15] *= scale;
	
	return ret;
	
}

//===========================================================
/**
 *	@brief	X回転行列を返す
 *
 *	@param	widedeg	一周を KL_WDEG_CYCLE 度とした回転角度単位
 *
 *	@return	計算結果の行列
 */
//===========================================================
KLMat4 KLMat4_GetRotX( wdeg widedeg ){
	
	KLMat4 ret;
	
	ret.p[ 0] = 1;					ret.p[ 1] = 0;					ret.p[ 2] = 0;					ret.p[ 3] = 0;
	ret.p[ 4] = 0;					ret.p[ 5] = KL_Cos(widedeg);	ret.p[ 6] = -KL_Sin(widedeg);	ret.p[ 7] = 0;
	ret.p[ 8] = 0;					ret.p[ 9] = KL_Sin(widedeg);	ret.p[10] = KL_Cos(widedeg);	ret.p[11] = 0;
	ret.p[12] = 0;					ret.p[13] = 0;					ret.p[14] = 0;					ret.p[15] = 1;
	
	return ret;
	
}

//===========================================================
/**
 *	@brief	Y回転行列を返す
 *
 *	@param	widedeg	一周を KL_WDEG_CYCLE 度とした回転角度単位
 *
 *	@return	計算結果の行列
 */
//===========================================================
KLMat4 KLMat4_GetRotY( wdeg widedeg ){

	KLMat4 ret;
	
	ret.p[ 0] = KL_Cos(widedeg);	ret.p[ 1] = 0;					ret.p[ 2] = KL_Sin(widedeg);	ret.p[ 3] = 0;
	ret.p[ 4] = 0;					ret.p[ 5] = 1;					ret.p[ 6] = 0;					ret.p[ 7] = 0;
	ret.p[ 8] = -KL_Sin(widedeg);	ret.p[ 9] = 0;					ret.p[10] = KL_Cos(widedeg);	ret.p[11] = 0;
	ret.p[12] = 0;					ret.p[13] = 0;					ret.p[14] = 0;					ret.p[15] = 1;
	
	return ret;
	
}

//===========================================================
/**
 *	@brief	Z回転行列を返す
 *
 *	@param	widedeg	一周を KL_WDEG_CYCLE 度とした回転角度単位
 *
 *	@return	計算結果の行列
 */
//===========================================================
KLMat4 KLMat4_GetRotZ( wdeg widedeg ){
	
	KLMat4 ret;
	
	ret.p[ 0] = KL_Cos(widedeg);	ret.p[ 1] = -KL_Sin(widedeg);	ret.p[ 2] = 0;					ret.p[ 3] = 0;
	ret.p[ 4] = KL_Sin(widedeg);	ret.p[ 5] = KL_Cos(widedeg);	ret.p[ 6] = 0;					ret.p[ 7] = 0;
	ret.p[ 8] = 0;					ret.p[ 9] = 0;					ret.p[10] = 1;					ret.p[11] = 0;
	ret.p[12] = 0;					ret.p[13] = 0;					ret.p[14] = 0;					ret.p[15] = 1;

	return ret;
	
}

//===========================================================
/**
 *	@brief	正射影行列を設定する
 *
 *	@param	left	左クリップ
 *	@param	left	右クリップ
 *	@param	left	下クリップ
 *	@param	left	上クリップ
 *	@param	left	ニアクリップ
 *	@param	left	ファークリップ
 *
 *	@return	計算結果の行列
 */
//===========================================================
KLMat4 KLMat4_GetOrthoMatrix(float left, float right, float bottom, float top, float near, float far){
	
	KLMat4 ret;
	KLMat4_Identity(&ret);
	
	float dx = right - left;
	float dy = top - bottom;
	float dz = far - near;
	
	float tx = dx ? -(right + left) / dx : 0;
	float ty = dy ? -(top + bottom) / dy : 0;
	float tz = dz ? -(far + near) / dz : 0;

	ret.p[0] = 2.0f / dx;														//ret.p[3] = tx;
							ret.p[5] = 2.0f / dy;								//ret.p[7] = ty;
													ret.p[10] = 2.0f / dz;		//ret.p[11] = tz;
	ret.p[12]= tx;			ret.p[13] = ty;			ret.p[14] = tz;				ret.p[15] = 1.0f;
//																				ret.p[15] = 1.0f;

	return ret;
}

//===========================================================
/**
 *	@brief	逆行列を取得する
 *
 *	@param	pMat	元となる行列
 *
 *	@return	計算結果の行列
 */
//===========================================================
KLMat4 KLMat4_Inverse( KLMat4* pMat ){

	KLMat4 mat = *pMat;
	
	GLfloat rate =	pMat->p[0]*pMat->p[5]*pMat->p[10]*pMat->p[15]	+
					pMat->p[0]*pMat->p[6]*pMat->p[11]*pMat->p[13]	+
					pMat->p[0]*pMat->p[7]*pMat->p[9]*pMat->p[14]	+
					pMat->p[1]*pMat->p[4]*pMat->p[11]*pMat->p[14]	+
					pMat->p[1]*pMat->p[6]*pMat->p[8]*pMat->p[15]	+
					pMat->p[1]*pMat->p[7]*pMat->p[10]*pMat->p[12]	+
					pMat->p[2]*pMat->p[4]*pMat->p[9]*pMat->p[15]	+
					pMat->p[2]*pMat->p[5]*pMat->p[11]*pMat->p[12]	+
					pMat->p[2]*pMat->p[7]*pMat->p[8]*pMat->p[13]	+
					pMat->p[3]*pMat->p[4]*pMat->p[10]*pMat->p[13]	+
					pMat->p[3]*pMat->p[5]*pMat->p[8]*pMat->p[14]	+
					pMat->p[3]*pMat->p[6]*pMat->p[9]*pMat->p[12]	-
					pMat->p[0]*pMat->p[5]*pMat->p[11]*pMat->p[14]	-
					pMat->p[0]*pMat->p[6]*pMat->p[9]*pMat->p[15]	-
					pMat->p[0]*pMat->p[7]*pMat->p[10]*pMat->p[13]	-
					pMat->p[1]*pMat->p[4]*pMat->p[10]*pMat->p[15]	-
					pMat->p[1]*pMat->p[6]*pMat->p[11]*pMat->p[12]	-
					pMat->p[1]*pMat->p[7]*pMat->p[8]*pMat->p[14]	-
					pMat->p[2]*pMat->p[4]*pMat->p[11]*pMat->p[13]	-
					pMat->p[2]*pMat->p[5]*pMat->p[8]*pMat->p[15]	-
					pMat->p[2]*pMat->p[7]*pMat->p[9]*pMat->p[12]	-
					pMat->p[3]*pMat->p[4]*pMat->p[9]*pMat->p[14]	-
					pMat->p[3]*pMat->p[5]*pMat->p[10]*pMat->p[12]	-
					pMat->p[3]*pMat->p[6]*pMat->p[8]*pMat->p[13];
	
	
	mat.p[0] = (
				pMat->p[5]*pMat->p[10]*pMat->p[15] +
				pMat->p[6]*pMat->p[11]*pMat->p[13] +
				pMat->p[7]*pMat->p[9]*pMat->p[14] -
				pMat->p[5]*pMat->p[11]*pMat->p[14] -
				pMat->p[6]*pMat->p[9]*pMat->p[15] -
				pMat->p[7]*pMat->p[10]*pMat->p[13]
	)/rate;
	
	mat.p[1] = (
				pMat->p[1]*pMat->p[11]*pMat->p[14] +
				pMat->p[2]*pMat->p[9]*pMat->p[15] +
				pMat->p[3]*pMat->p[10]*pMat->p[13] -
				pMat->p[1]*pMat->p[10]*pMat->p[15] -
				pMat->p[2]*pMat->p[11]*pMat->p[13] -
				pMat->p[3]*pMat->p[9]*pMat->p[14]				
	)/rate;
	
	mat.p[2] = (
				pMat->p[1]*pMat->p[6]*pMat->p[15] +
				pMat->p[2]*pMat->p[7]*pMat->p[13] +
				pMat->p[3]*pMat->p[5]*pMat->p[14] -
				pMat->p[1]*pMat->p[7]*pMat->p[14] -
				pMat->p[2]*pMat->p[5]*pMat->p[15] -
				pMat->p[3]*pMat->p[6]*pMat->p[13]				
	)/rate;
	
	mat.p[3] = (
				pMat->p[1]*pMat->p[7]*pMat->p[10] +
				pMat->p[2]*pMat->p[5]*pMat->p[11] +
				pMat->p[3]*pMat->p[6]*pMat->p[9] -
				pMat->p[1]*pMat->p[6]*pMat->p[11] -
				pMat->p[2]*pMat->p[7]*pMat->p[9] -
				pMat->p[3]*pMat->p[5]*pMat->p[10]
	)/rate;
	
	
	mat.p[4] = (
				pMat->p[4]*pMat->p[11]*pMat->p[14] +
				pMat->p[6]*pMat->p[8]*pMat->p[15] +
				pMat->p[7]*pMat->p[10]*pMat->p[12] -
				pMat->p[4]*pMat->p[10]*pMat->p[15] -
				pMat->p[6]*pMat->p[11]*pMat->p[12] -
				pMat->p[7]*pMat->p[8]*pMat->p[14]
	)/rate;
	
	mat.p[5] = (
				pMat->p[0]*pMat->p[10]*pMat->p[15] +
				pMat->p[2]*pMat->p[11]*pMat->p[12] +
				pMat->p[3]*pMat->p[8]*pMat->p[14] -
				pMat->p[0]*pMat->p[11]*pMat->p[14] -
				pMat->p[2]*pMat->p[8]*pMat->p[15] -
				pMat->p[3]*pMat->p[10]*pMat->p[12]
	)/rate;
	
	mat.p[6] = (
				pMat->p[0]*pMat->p[7]*pMat->p[14] +
				pMat->p[2]*pMat->p[4]*pMat->p[15] +
				pMat->p[3]*pMat->p[6]*pMat->p[12] -
				pMat->p[0]*pMat->p[6]*pMat->p[15] -
				pMat->p[2]*pMat->p[7]*pMat->p[12] -
				pMat->p[3]*pMat->p[4]*pMat->p[14]
	)/rate;
	
	mat.p[7] = (
				pMat->p[0]*pMat->p[6]*pMat->p[11] +
				pMat->p[2]*pMat->p[7]*pMat->p[8] +
				pMat->p[3]*pMat->p[4]*pMat->p[10] -
				pMat->p[0]*pMat->p[7]*pMat->p[10] -
				pMat->p[2]*pMat->p[4]*pMat->p[11] -
				pMat->p[3]*pMat->p[6]*pMat->p[8]
	)/rate;
	
	mat.p[8] = (
				pMat->p[4]*pMat->p[9]*pMat->p[15] +
				pMat->p[5]*pMat->p[11]*pMat->p[12] +
				pMat->p[7]*pMat->p[8]*pMat->p[13] -
				pMat->p[4]*pMat->p[11]*pMat->p[13] -
				pMat->p[5]*pMat->p[8]*pMat->p[15] -
				pMat->p[7]*pMat->p[9]*pMat->p[12]
	)/rate;
	
	mat.p[9] = (
				pMat->p[0]*pMat->p[11]*pMat->p[13] +
				pMat->p[1]*pMat->p[8]*pMat->p[15] +
				pMat->p[3]*pMat->p[9]*pMat->p[12] -
				pMat->p[0]*pMat->p[9]*pMat->p[15] -
				pMat->p[1]*pMat->p[11]*pMat->p[12] -
				pMat->p[3]*pMat->p[8]*pMat->p[13]
	)/rate;
	
	mat.p[10]= (
				pMat->p[0]*pMat->p[5]*pMat->p[15] +
				pMat->p[1]*pMat->p[7]*pMat->p[12] +
				pMat->p[3]*pMat->p[4]*pMat->p[13] -
				pMat->p[0]*pMat->p[7]*pMat->p[13] -
				pMat->p[1]*pMat->p[4]*pMat->p[15] -
				pMat->p[3]*pMat->p[5]*pMat->p[12]
	)/rate;
	
	mat.p[11]= (
				pMat->p[0]*pMat->p[7]*pMat->p[9] +
				pMat->p[1]*pMat->p[4]*pMat->p[11] +
				pMat->p[3]*pMat->p[5]*pMat->p[8] -
				pMat->p[0]*pMat->p[5]*pMat->p[11] -
				pMat->p[1]*pMat->p[7]*pMat->p[8] -
				pMat->p[3]*pMat->p[4]*pMat->p[9]
	)/rate;
	
	mat.p[12]= (
				pMat->p[4]*pMat->p[10]*pMat->p[13] +
				pMat->p[5]*pMat->p[8]*pMat->p[14] +
				pMat->p[6]*pMat->p[9]*pMat->p[12] -
				pMat->p[4]*pMat->p[9]*pMat->p[14] -
				pMat->p[5]*pMat->p[10]*pMat->p[12] -
				pMat->p[6]*pMat->p[8]*pMat->p[13]
	)/rate;
	
	mat.p[13]= (
				pMat->p[0]*pMat->p[9]*pMat->p[14] +
				pMat->p[1]*pMat->p[10]*pMat->p[12] +
				pMat->p[2]*pMat->p[8]*pMat->p[13] -
				pMat->p[0]*pMat->p[10]*pMat->p[13] -
				pMat->p[1]*pMat->p[8]*pMat->p[14] -
				pMat->p[2]*pMat->p[9]*pMat->p[12]
	)/rate;
	
	mat.p[14]= (
				pMat->p[0]*pMat->p[6]*pMat->p[13] +
				pMat->p[1]*pMat->p[4]*pMat->p[14] +
				pMat->p[2]*pMat->p[5]*pMat->p[12] -
				pMat->p[0]*pMat->p[5]*pMat->p[14] -
				pMat->p[1]*pMat->p[6]*pMat->p[12] -
				pMat->p[2]*pMat->p[4]*pMat->p[13]
	)/rate;
	
	mat.p[15]= (
				pMat->p[0]*pMat->p[5]*pMat->p[10] +
				pMat->p[1]*pMat->p[6]*pMat->p[8] +
				pMat->p[2]*pMat->p[4]*pMat->p[9] -
				pMat->p[0]*pMat->p[6]*pMat->p[9] -
				pMat->p[1]*pMat->p[4]*pMat->p[10] -
				pMat->p[2]*pMat->p[5]*pMat->p[8]
	)/rate;
	
	return mat;
	
}


//===========================================================
/**
 *	@brief	行列に移動値を反映する。（パラメータで与えた行列自身は変更されない）
 *
 *	@param	pMat	計算する行列
 *	@param
 *
 *	@return	計算結果の行列
 */
//==========================================================
KLMat4 KLMat4_Move( KLMat4 mat, float x, float y, float z ){
	
	mat.p[12] += x;
	mat.p[13] += y;
	mat.p[14] += z;
	
	return mat;
	
}


//===========================================================
/**
 *	@brief	移動行列を取得する
 *
 *	@return	計算結果の行列
 */
//==========================================================
KLMat4 KLMat4_GetMove( float x, float y, float z ){
	
	KLMat4 mat = KLMat4_GetIdentity();
	
	mat.p[12] = x;
	mat.p[13] = y;
	mat.p[14] = z;
	
	return mat;
	
}

