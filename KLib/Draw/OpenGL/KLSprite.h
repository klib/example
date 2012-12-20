//
//  KLSprite.h
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

#ifndef KLib_KLSprite_h
#define KLib_KLSprite_h

#include "c_extern.h"
#include "KLibDefine.h"
#include "KLGL.h"
#include "KLMatrix.h"
#include "KLTexture.h"

/// プリミティブ頂点数
#define KLDRAWQUEUE_PRIM_VERTEXNUM (6)

/// プリミティブインデクス数
#define KLDRAWQUEUE_PRIM_INDEXNUM  (KLDRAWQUEUE_PRIM_VERTEXNUM)

/// スプライトオブジェクト
typedef struct{

	
	GLuint 		tid;			///< テクスチャID
	
	GLuint		texfilter;		///< テクスチャフィルタタイプ
	
	GLfloat		x,y,z;			///< XYZ座標
	GLfloat		cx,cy,cz;		///< 回転軸座標(デフォルトは中央)
	
	Rgba		rgbaRate[4];	///< スプライトカラー(各頂点乗算用) 0に近いほどその要素は反映されなくなる
	Rgba		rgba[4];		///< スプライトカラー(各頂点加算用)
	GLfloat		uv[KL_AXIS_SIZE_XYZ*KLDRAWQUEUE_PRIM_INDEXNUM];///< UV座標
	GLubyte*	pData;			///< ピクセルデータ
	KLRect*		pUvTable;		///< テクスチャ情報テーブル
	
	wdeg		rx,ry,rz;		///< 各軸の回転(wdeg) ※現在はZのみ有効
	s32			w,h;			///< 現在の幅と高さ
	s32			w_src;			///< 初期化時の幅
	s32			h_src;			///< 初期化時の高さ
	
	bl			isGradation:1;	///< グラデーション使用有無
	bl			isReverseUv:1;	///< UV上下を反転して設定するか
	
}KLSprite;

void	KLSprite_Init(			KLSprite* pSprite );
void	KLSprite_Unload(		KLSprite* pSprite );
void	KLSprite_UnloadWithTexture( KLSprite* pSprite );
void	KLSprite_Duplicate(		KLSprite* pSpriteDest, KLSprite* pSpriteSrc );
void	KLSprite_Draw(			KLSprite* pSprite );
bl		KLSprite_LoadFile(		KLSprite* pSprite, const path8 path );
bl		KLSprite_LoadFromPixels(KLSprite* pSprite, const s32 w, const s32 h, const bin8* pData );
void	KLSprite_SetTexId(		KLSprite* pSprite, const GLuint texid );
GLfloat KLSprite_PixelToU(		KLSprite* pSprite, const s32 pixel );
GLfloat KLSprite_PixelToV(		KLSprite* pSprite, const s32 pixel );
void	KLSprite_SetXY(			KLSprite* pSprite, const s32 x, const s32 y );
void	KLSprite_SetWH(			KLSprite* pSprite, const s32 w, const s32 h );
void	KLSprite_SetScreenWH(	KLSprite* pSprite );
void	KLSprite_SetCenter(		KLSprite* pSprite, const s32 cx,const s32 cy );
void	KLSprite_SetCenterSimple( KLSprite* pSprite );
void	KLSprite_SetRot(		KLSprite* pSprite, const wdeg rot );
void	KLSprite_SetUV(			KLSprite* pSprite, const s32 xTopL, const s32 yTopL, const s32 xBottomR, const s32 yBottomR );
void	KLSprite_SetUVFit(		KLSprite* pSprite, const s32 xTopL, const s32 yTopL, const s32 xBottomR, const s32 yBottomR );
void	KLSprite_SetRgb(		KLSprite* pSprite, const u8 r, const u8 g, const u8 b );
void	KLSprite_SetRgba(		KLSprite* pSprite, const u8 r, const u8 g, const u8 b, const u8 a );
void	KLSprite_SetAlpha(		KLSprite* pSprite, const u8 a );
void	KLSprite_SetRgbAdd(		KLSprite* pSprite, const u8 r, const u8 g, const u8 b );
void	KLSprite_SetRgbaAdd(	KLSprite* pSprite, const u8 r, const u8 g, const u8 b, const u8 a );
void	KLSprite_SetAlphaAdd(	KLSprite* pSprite, const u8 a );
void	KLSprite_SetChannelRate(KLSprite* pSprite, const u8 r, const u8 g, const u8 b, const u8 a );
void	KLSprite_SetGrad(		KLSprite* pSprite, const Rgba topL, const Rgba bottomL, const Rgba topR, const Rgba bottomR );
void	KLSprite_SetGradX(		KLSprite* pSprite, const Rgba left, const Rgba right );
void	KLSprite_SetGradY(		KLSprite* pSprite, const Rgba top, const Rgba bottom );
void	KLSprite_SetGradAdd(	KLSprite* pSprite, const Rgba topL, const Rgba bottomL, const Rgba topR, const Rgba bottomR );
void	KLSprite_SetGradXAdd(	KLSprite* pSprite, const Rgba left, const Rgba right );
void	KLSprite_SetGradYAdd(	KLSprite* pSprite, const Rgba top, const Rgba bottom );

void	KLTexture_Init( KLSprite* pSprite, KLRect* pUvTable );
void	KLTexture_SetBounds( KLSprite* pSprite, u16 index );
void	KLTexture_SetBoundsFit( KLSprite* pSprite, u16 index );
s32		KLTexture_GetW( KLSprite* pSprite, u16 index );
s32		KLTexture_GetH( KLSprite* pSprite, u16 index );


#include "c_extern_close.h"
#endif
