//
//  KLDrawQueue.h
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

#ifndef KLib_KLDrawQueue_h
#define KLib_KLDrawQueue_h

#include "c_extern.h"
#include "KLibDefine.h"
#include "KLGL.h"
#include "KLSprite.h"

typedef enum{
	
	KLPRIM_UNDEFINED,
	KLPRIM_SQUARE,
	KLPRIM_LINE,
	KLPRIM_CIRCLE,
	
}KLPrimitiveType;

typedef struct{

	GLfloat		x,y,z;
	GLubyte		r,g,b,a;
	GLubyte		rm,gm,bm,am;
	GLushort	u,v;
	GLushort	i;
	
	// 各種型を変えて構造体のサイズが変わったら
	// 必要に応じてu8 padding[n]を定義して4byte境界に揃える事
	// nは構造体サイズが4の倍数になるよう要調整
	
}KLVertex;

/// 描画キュー
typedef struct{
	
	u32				nMax;			///< キュー最大数
	KLPrimitiveType	type;			///< 最後に処理したプリミティブタイプ
	GLuint			tid;			///< 最後に処理したテクスチャID
	GLuint			iboSquare;		///< スプライト＆矩形用IBO
	GLuint			iboLine;		///< ライン用インデクス
	GLuint			iboCircle;		///< サークル用インデクス
	GLuint			blend[2];		///< ブレンドタイプ
	GLushort*		pUv;			///< UV( ライン系はプリミティブ区切り数 )
	GLfloat*		pVertex;		///< 座標
	GLushort*		pIndexes;		///< スプライト＆矩形用インデクス
	GLushort*		pIndexesLine;	///< ライン用インデクス
	GLushort*		pIndexesCircle;	///< サークル用インデクス
	GLubyte*		pColorRate;		///< カラーレート(各ピクセルに乗算する値 デフォルトは255)
	GLubyte*		pColor;			///< カラー加算値(各ピクセルに加算する値 デフォルトは0)
	
	GLfloat			wLine;			///< ライン幅
	
	KLVertex*		pv;				///< 頂点バッファ
	
	u16				nVertexPrim;	///< プリミティブ一個あたりの頂点数
	u16				nIndexPrim;		///< プリミティブ一個あたりのインデクス数
	u32				offset;			///< 次の追加先オフセット
	bl				isForceUpdate;	///< 次回描画で古いキューを強制描画
		
}KLDrawQueue;


bl		KLDrawQueue_Init(u32 nMax);
void	KLDrawQueue_Unload();
void	KLDrawQueue_DrawSprite( KLSprite* pSprite );
void	KLDrawQueue_Clear();
void	KLDrawQueue_Flush();
void	KL_DrawRect( float x, float y, float w, float h, Rgba rgba );
void	KL_DrawRectRot( float x, float y, float w, float h, Rgba rgba, float cx, float cy, wdeg rot );
void	KL_DrawRectScreen( Rgba rgba );
void	KL_DrawLine( KLPos3d* pPosAry, Rgba commonRgba, u32 nAry, u32 linew );
void	KL_DrawLineMultiColor( KLPos3d* pPosAry, Rgba* pRgbaAry, u32 nAry, u32 linew );
void	KL_DrawPolygon( float cx, float cy, float r, u16 nTerm, u16 rot, Rgba rgba );
	//void	KL_DrawArc( float cx, float cy, float r, wdeg startwdeg, wdeg endwdeg, Rgba rgba );
void	KL_DrawGradation( float x, float y, float w, float h, Rgba topL, Rgba bottomL, Rgba topR, Rgba bottomR );
void	KL_DrawGradationX( float x, float y, float w, float h, Rgba rgba1, Rgba rgba2 );
void	KL_DrawGradationY( float x, float y, float w, float h, Rgba rgba1, Rgba rgba2 );

#define DrawRect	KL_DrawRect
#define DrawRectRot	KL_DrawRectRot
#define DrawCircle	KL_DrawArc
#define DrawScreen	KL_DrawRectScreen
#define DrawFlush	KLDrawQueue_Flush
#define DrawClear	KLDrawQueue_Clear
#define DrawSprite	KLDrawQueue_DrawSprite

#include "c_extern_close.h"
#endif
