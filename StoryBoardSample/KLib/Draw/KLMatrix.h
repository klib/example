//
//  KLMatrix.h
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

#ifndef __KLib__KLMatrix__
#define __KLib__KLMatrix__

#include <OpenGLES/ES2/gl.h>

#if __OBJC__
	#include <OpenGLES/EAGL.h>
#endif

#include "c_extern.h"

// O B J E C T
//====================================

	//====================================
	/**
	 *	@brief	XYZベクトルオブジェクト
	 */
	//====================================
	typedef union{
		
		struct{ GLfloat x,y,z; };
		GLfloat	_[3];
		GLfloat	*p;
		
	}KLVec3;


	//====================================
	/**
	 *	@brief	4x4マトリクスオブジェクト
	 */
	//====================================
	typedef struct{

		GLfloat	p[4*4];
		
	}KLMat4;


// M A C R O
//====================================

	/// 360度数をラジアンに変換
	#define KL_DegToRadian(deg) (deg * PI / 180.0f)

	/// 360度数をワイド度数(65536度数)に変換
	#define KL_DegToWdeg(deg) (deg * KL_WDEG_CYCLE / 360)



// P R O T T Y P E
//====================================

	float KL_Cos(u16 angle);
	float KL_Sin(u16 angle);

	bl KLMat4_Cmp( KLMat4* pMatA, KLMat4* pMatB );
	bl KLMat4_IsSame( KLMat4* pMatA, KLMat4* pMatB );
	KLMat4 KLMat4_Inverse( KLMat4* pMat );
	KLMat4 KLMat4_GetOrthoMatrix(float left, float right, float bottom, float top, float near, float far);
	KLMat4 KLMat4_GetRotZ( wdeg widedeg );
	KLMat4 KLMat4_GetRotY( wdeg widedeg );
	KLMat4 KLMat4_GetRotX( wdeg widedeg );
	KLMat4 KLMat4_Scaling( KLMat4* pMatA, GLfloat scale );
	KLMat4 KLMat4_Add( KLMat4* pMatA, KLMat4* pMatB );
	KLMat4 KLMat4_Sub( KLMat4* pMatA, KLMat4* pMatB );
	KLMat4 KLMat4_Mul( KLMat4* pMatA, KLMat4* pMatB );
	void KLMat4_Copy( KLMat4* pDest, KLMat4* pSrc );
	KLMat4 KLMat4_GetIdentity();
	void KLMat4_Identity( KLMat4* pMat );
	KLMat4 KLMat4_GetZero();
	void KLMat4_Zero( KLMat4* pMat );
	KLMat4 KLMat4_Move( KLMat4 mat, float x, float y, float z );
	KLMat4 KLMat4_GetMove( float x, float y, float z );


#include "c_extern_close.h"
#endif /* defined(__KLib__KLMatrix__) */
