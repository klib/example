//
//  KLSprite.c
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

#include "Klib.h"
#include "KLSprite.h"
#include "KLDrawQueue.h"
#include "stb_image.h"

//=========================================================================
/**
 *	@brief	描画キューにスプライトを積む
 *
 *	@param	pSprite	対象スプライト
 */
//=========================================================================
void KLSprite_Draw(KLSprite* pSprite){

	KLDrawQueue_DrawSprite(pSprite);

}

//=========================================================================
/**
 *	@brief	スプライトの初期化
 *
 *	@param	pSprite	対象スプライト
 */
//=========================================================================
void KLSprite_Init( KLSprite* pSprite ){

	klmemclear(pSprite, KLSprite);
	pSprite->texfilter	= GL_NEAREST;
	pSprite->tid		= KLDrawQueue_GetMinimumTexId();
	KLSprite_SetRgba(pSprite, 255, 255, 255, 255);
	KLSprite_SetRgbaAdd(pSprite, 0, 0, 0, 0 );
	
}

//=========================================================================
/**
 *	@brief	スプライトのテクスチャに指定ファイルを読み込んでバインドする
 *
 *	@param	pSprite	対象スプライト
 *	@param	path	ロードする画像のパス(アプリホーム以下)
 *
 *	@return	正常に処理した場合はTRUE 失敗時はFALSE
 */
//=========================================================================
bl KLSprite_LoadFile(KLSprite* pSprite, const path8 path ){

	int 	w, h, n;
	char	fixpath[256];
	bin8*	pData = NULL;
	bl		ret = FALSE;
	
	KLSprite_Init(pSprite);
	
	sprintf( fixpath, "%s/%s", klib.path.appRoot, path );
	
	pData = (bin8*)stbi_load(fixpath, &w, &h, &n, 4); // request RGBA
	
	if(!pData)
	{
		KLLog("[KLSprite] !!! Could not load %s\n", fixpath);
		return FALSE;
	}
	
	ret = KLSprite_LoadFromPixels( pSprite, w, h, pData );
	
	stbi_image_free(pData);
	
	return ret;
	
}

//=========================================================================
/**
 *	@brief	スプライトのテクスチャに指定画像バイナリをバインドする
 *
 *	@param	pSprite		対象スプライト
 *	@param	pImgData	読み込み済みの画像データ
 *	@param	szImg		画像データサイズ
 *
 *	@return	正常に処理した場合はTRUE 失敗時はFALSE
 */
//=========================================================================
bl KLSprite_LoadFromImageData (KLSprite* pSprite, bin8* pImgData, const size_t szImg ){

	bl		ret = FALSE;
	int 	w, h, n;
	
	bin8* pData = (bin8*)stbi_load_from_memory( (stbi_uc const *)pImgData, szImg, &w, &h, &n, 4);
	
	if(!pData)
	{
		KLLog("[KLSprite] !!! Could not load image.(KLSprite_LoadFromImageData)\n");
		return FALSE;
	}
	
	ret = KLSprite_LoadFromPixels( pSprite, w, h, pData );
	
	stbi_image_free(pData);
	
	return ret;
	
}

//=========================================================================
/**
 *	@brief	スプライトのテクスチャに指定ピクセルデータをバインドする
 *
 *	@param	pSprite	対象スプライト
 *	@param	w		指定データの幅
 *	@param	h		指定データの高さ
  *	@param	pData	読み込み済みのピクセルデータ		
 *
 *	@return	正常に処理した場合はTRUE 失敗時はFALSE
 */
//=========================================================================
bl KLSprite_LoadFromPixels (KLSprite* pSprite, const s32 w, const s32 h, const bin8* pData ){
	
	if (pData != NULL)
	{
		pSprite->w_src	= w;
		pSprite->h_src	= h;
		
		glGenTextures(1, &pSprite->tid);
		KLGL_CheckError();
		glsBindTexture(pSprite->tid);
		KLGL_CheckError();
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		KLGL_CheckError();
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
		KLGL_CheckError();
		
		// Mipmapフィルタ以外を指定するならglTexImage2D以前に設定しないと警告が出る上にパフォーマンスダウンする模様
		// ただしMipmapを設定するとテクスチャサイズは2の累乗に制限されるので注意
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,pSprite->texfilter);
		KLGL_CheckError();
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,pSprite->texfilter);
		KLGL_CheckError();
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);
		KLGL_CheckError();
		
		KLGL_AddTextureUsage(w*h,GL_RGBA,GL_UNSIGNED_BYTE);
		
		KLSprite_SetUVFit(pSprite, 0, 0, w, h);
		
		return TRUE;
	}
	
	return FALSE;
	
}

//===========================================================
/**
 *	@brief	スプライトの複製を行う
 *
 *	@param	pSpriteDest	コピー先
 *	@param	pSpriteSrc	コピー元
 *
 *	テクスチャを使いまわしつつスプライトを分けたい時に使用します。<br/>
 *	アンロードの際、UnloadWithTextureすると複製先・元で共有しているテクスチャが削除され<br/>
 *	使用できなくなるので注意
 */
//===========================================================
inline void KLSprite_Duplicate( KLSprite* pSpriteDest, KLSprite* pSpriteSrc ){
	
	*pSpriteDest = *pSpriteSrc;
	
}

//===========================================================
/**
 *	@brief	ピクセル値をテクスチャX座標の0.0〜1.0に変換(float)
 *
 *	@param	変換したいピクセルの数値
 *
 *	@return	0.0〜1.0のテクスチャ座標
 */
//===========================================================
inline GLfloat KLSprite_PixelToU( KLSprite* pSprite, const s32 pixel ){
	
	return pixel ? (pixel / (GLfloat)pSprite->w_src) : 0.0f;

}

//===========================================================
/**
 *	@brief	ピクセル値をテクスチャY座標の0.0〜1.0に変換(float)
 *
 *	@param	変換したいピクセルの数値
 *
 *	@return	0.0〜1.0のテクスチャ座標
 */
//===========================================================
inline GLfloat KLSprite_PixelToV( KLSprite* pSprite, const s32 pixel ){
	
	return pixel ? (pixel / (GLfloat)pSprite->h_src) : 0.0f;

}

//===========================================================
/**
 *	@brief	スプライトのUVを設定する
 *
 *	@param	pSprite		スプライトのポインタ
 *	@param	xTopL		左上のX座標(左から何ピクセルか)
 *	@param	yTopL		左上のY座標(上から何ピクセルか)
 *	@param	xBottomR	右下のX座標(左から何ピクセルか)
 *	@param	yBottomR	右下のY座標(上から何ピクセルか)
 */
//===========================================================
void KLSprite_SetUV( KLSprite* pSprite, const s32 xTopL, const s32 yTopL, const s32 xBottomR, const s32 yBottomR ){
	
	// OpenGLの原点は左下なので反転させて上下を合わせる
	if( pSprite->isReverseUv )
	{
		pSprite->uv[0] = KLSprite_PixelToU(pSprite, xTopL);
		pSprite->uv[3] = 1.0f-KLSprite_PixelToV(pSprite, yTopL);
		pSprite->uv[2] = KLSprite_PixelToU(pSprite, xTopL);
		pSprite->uv[1] = 1.0f-KLSprite_PixelToV(pSprite, yBottomR);
		pSprite->uv[4] = KLSprite_PixelToU(pSprite, xBottomR);
		pSprite->uv[7] = 1.0f-KLSprite_PixelToV(pSprite, yTopL);
		pSprite->uv[6] = KLSprite_PixelToU(pSprite, xBottomR);
		pSprite->uv[5] = 1.0f-KLSprite_PixelToV(pSprite, yBottomR);	
	}
	else
	{
		pSprite->uv[0] = KLSprite_PixelToU(pSprite, xTopL);
		pSprite->uv[3] = KLSprite_PixelToV(pSprite, yBottomR);
		pSprite->uv[2] = KLSprite_PixelToU(pSprite, xTopL);
		pSprite->uv[1] = KLSprite_PixelToV(pSprite, yTopL);
		pSprite->uv[4] = KLSprite_PixelToU(pSprite, xBottomR);
		pSprite->uv[7] = KLSprite_PixelToV(pSprite, yBottomR);
		pSprite->uv[6] = KLSprite_PixelToU(pSprite, xBottomR);
		pSprite->uv[5] = KLSprite_PixelToV(pSprite, yTopL);
	}

}

//===========================================================
/**
 *	@brief	スプライトのUVを設定した後、そのサイズに変更する
 *
 *	@param	pSprite		スプライトのポインタ
 *	@param	xTopL		左上のX座標(左から何ピクセルか)
 *	@param	yTopL		左上のY座標(上から何ピクセルか)
 *	@param	xBottomR	右下のX座標(左から何ピクセルか)
 *	@param	yBottomR	右下のY座標(上から何ピクセルか)
 */
//===========================================================
void KLSprite_SetUVFit( KLSprite* pSprite, const s32 xTopL, const s32 yTopL, const s32 xBottomR, const s32 yBottomR ){
	
	KLSprite_SetUV( pSprite, xTopL, yTopL, xBottomR, yBottomR );
	KLSprite_SetWH( pSprite,
					(xBottomR-xTopL)<0?-(xBottomR-xTopL):(xBottomR-xTopL),
				    (yBottomR-yTopL)<0?-(yBottomR-yTopL):(yBottomR-yTopL) );
	
}


//===========================================================
/**
 *	@brief	スプライトのアンロード
 *
 *	@param	pSprite	スプライトのポインタ
 */
//===========================================================
void KLSprite_Unload( KLSprite* pSprite ){
	
	if( pSprite->tid != KLGL_UNBINDED_TEXID )
	{
		glsBindTexturef(KLGL_UNBINDED_TEXID);
	}
	
}


//===========================================================
/**
 *	@brief	スプライトのアンロード(バインド中のテクスチャもglDeleteTexturesする)
 *
 *	@param	pSprite	スプライトのポインタ
 */
//===========================================================
void KLSprite_UnloadWithTexture( KLSprite* pSprite ){
	
	if( pSprite->tid != KLGL_UNBINDED_TEXID )
	{
		KLGL_SubTextureUsage( pSprite->w_src*pSprite->h_src, GL_RGBA, GL_UNSIGNED_BYTE);
		glsBindTexturef( pSprite->tid );
		glDeleteTextures( 1, &pSprite->tid );
		glsBindTexturef(KLGL_UNBINDED_TEXID);
	}
	
}


//===========================================================
/**
 *	@brief	スプライトのXYを設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	x		X座標
 *	@param	y		Y座標
 */
//===========================================================
inline void KLSprite_SetXY( KLSprite* pSprite, const s32 x, const s32 y ){
	
	pSprite->x = x;
	pSprite->y = y;
	
}

//===========================================================
/**
 *	@brief	スプライトのWidth heightを設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	w		幅
 *	@param	h		高さ
 */
//===========================================================
inline void KLSprite_SetWH( KLSprite* pSprite, const s32 w, const s32 h ){
	
	pSprite->w = w;
	pSprite->h = h;
	
}

//===========================================================
/**
 *	@brief	スプライトのWidth heightを画面サイズに設定
 *
 *	@param	pSprite	スプライトのポインタ
 */
//===========================================================
inline void KLSprite_SetScreenWH( KLSprite* pSprite ){
	
	KLSprite_SetWH( pSprite, klib.device.w, klib.device.h );
	
}

//===========================================================
/**
 *	@brief	スプライトのテクスチャIDを設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	texid	テクスチャID(設定しないなら0)
 */
//===========================================================
inline void KLSprite_SetTexId( KLSprite* pSprite, const GLuint texid ){

	pSprite->tid = texid;

}

//===========================================================
/**
 *	@brief	スプライトのカラーレートを設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	r		赤レート値(0〜255)
 *	@param	g		緑レート値(0〜255)
 *	@param	b		青レート値(0〜255)
 */
//===========================================================
void KLSprite_SetRgb( KLSprite* pSprite, const u8 r, const u8 g, const u8 b ){
	
	pSprite->rgbaRate[0].r =
	pSprite->rgbaRate[1].r =
	pSprite->rgbaRate[2].r =
	pSprite->rgbaRate[3].r = r;
	
	pSprite->rgbaRate[0].g =
	pSprite->rgbaRate[1].g =
	pSprite->rgbaRate[2].g =
	pSprite->rgbaRate[3].g = g;
	
	pSprite->rgbaRate[0].b =
	pSprite->rgbaRate[1].b =
	pSprite->rgbaRate[2].b =
	pSprite->rgbaRate[3].b = b;
	
}

//===========================================================
/**
 *	@brief	スプライトのカラーレートとアルファレートを設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	r		赤レート値(0〜255)
 *	@param	g		緑レート値(0〜255)
 *	@param	b		青レート値(0〜255)
 *	@param	a		アルファレート値(0〜255)
 */
//===========================================================
void KLSprite_SetRgba( KLSprite* pSprite, const u8 r, const u8 g, const u8 b, const u8 a ){
	
	pSprite->rgbaRate[0].r =
	pSprite->rgbaRate[1].r =
	pSprite->rgbaRate[2].r =
	pSprite->rgbaRate[3].r = r;
	
	pSprite->rgbaRate[0].g =
	pSprite->rgbaRate[1].g =
	pSprite->rgbaRate[2].g =
	pSprite->rgbaRate[3].g = g;
	
	pSprite->rgbaRate[0].b =
	pSprite->rgbaRate[1].b =
	pSprite->rgbaRate[2].b =
	pSprite->rgbaRate[3].b = b;
	
	pSprite->rgbaRate[0].a =
	pSprite->rgbaRate[1].a =
	pSprite->rgbaRate[2].a =
	pSprite->rgbaRate[3].a = a;
	
}

//===========================================================
/**
 *	@brief	スプライトのアルファレートを設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	a		アルファレート値(0〜255)
 */
//===========================================================
inline void KLSprite_SetAlpha( KLSprite* pSprite, const u8 a ){

	pSprite->rgbaRate[0].a =
	pSprite->rgbaRate[1].a =
	pSprite->rgbaRate[2].a =
	pSprite->rgbaRate[3].a = a;

}

//===========================================================
/**
 *	@brief	スプライトのカラーとアルファをどれくらい反映するかのレートを設定<br/>
 *			255で設定した色をそのまま反映 / 0で反映しない
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	r		赤の反映レート(0〜255)
 *	@param	g		緑の反映レート(0〜255)
 *	@param	b		青の反映レート(0〜255)
 *	@param	a		アルファの反映レート(0〜255)
 */
//===========================================================
void KLSprite_SetChannelRate( KLSprite* pSprite, const u8 r, const u8 g, const u8 b, const u8 a ){
	
	pSprite->rgbaRate[0].r =
	pSprite->rgbaRate[1].r =
	pSprite->rgbaRate[2].r =
	pSprite->rgbaRate[3].r = r;
	
	pSprite->rgbaRate[0].g =
	pSprite->rgbaRate[1].g =
	pSprite->rgbaRate[2].g =
	pSprite->rgbaRate[3].g = g;
	
	pSprite->rgbaRate[0].b =
	pSprite->rgbaRate[1].b =
	pSprite->rgbaRate[2].b =
	pSprite->rgbaRate[3].b = b;
	
	pSprite->rgbaRate[0].a =
	pSprite->rgbaRate[1].a =
	pSprite->rgbaRate[2].a =
	pSprite->rgbaRate[3].a = a;
	
}

//===========================================================
/**
 *	@brief	スプライト頂点RGBAレートを左上、左下、右上、右下の順で設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	topL	左上のRGBAレート
 *	@param	bottomL	左下のRGBAレート
 *	@param	topR	右上のRGBAレート
 *	@param	bottomR	右下のRGBAレート
 */
//===========================================================
void KLSprite_SetGrad( KLSprite* pSprite, const Rgba topL, const Rgba bottomL, const Rgba topR, const Rgba bottomR ){
	
	pSprite->rgba[0] = topL;
	pSprite->rgba[1] = bottomL;
	pSprite->rgba[2] = topR;
	pSprite->rgba[3] = bottomR;

	pSprite->rgbaRate[0] = RGBA(0,0,0,0);
	pSprite->rgbaRate[1] = RGBA(0,0,0,0);
	pSprite->rgbaRate[2] = RGBA(0,0,0,0);
	pSprite->rgbaRate[3] = RGBA(0,0,0,0);
	
}

//===========================================================
/**
 *	@brief	スプライトの上下頂点のRGBAレートを設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	top		上のRGBAレート
 *	@param	bottom	下のRGBAレート
 */
//===========================================================
void KLSprite_SetGradY( KLSprite* pSprite, const Rgba top, const Rgba bottom ){

	pSprite->rgba[0] = top;
	pSprite->rgba[1] = bottom;
	pSprite->rgba[2] = top;
	pSprite->rgba[3] = bottom;
	
	pSprite->rgbaRate[0] = RGBA(0,0,0,0);
	pSprite->rgbaRate[1] = RGBA(0,0,0,0);
	pSprite->rgbaRate[2] = RGBA(0,0,0,0);
	pSprite->rgbaRate[3] = RGBA(0,0,0,0);
	
}

//===========================================================
/**
 *	@brief	スプライトの左右頂点のRGBAレートを設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	left	左のRGBAレート
 *	@param	right	右のRGBAレート
 */
//===========================================================
void KLSprite_SetGradX( KLSprite* pSprite, const Rgba left, const Rgba right ){
	
	pSprite->rgba[0] = left;
	pSprite->rgba[1] = left;
	pSprite->rgba[2] = right;
	pSprite->rgba[3] = right;

	pSprite->rgbaRate[0] = RGBA(0,0,0,0);
	pSprite->rgbaRate[1] = RGBA(0,0,0,0);
	pSprite->rgbaRate[2] = RGBA(0,0,0,0);
	pSprite->rgbaRate[3] = RGBA(0,0,0,0);
	
}

//===========================================================
/**
 *	@brief	スプライトの加算カラーを設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	r		赤の加算値(0〜255)
 *	@param	g		緑の加算値(0〜255)
 *	@param	b		青の加算値(0〜255)
 */
//===========================================================
void KLSprite_SetRgbAdd( KLSprite* pSprite, const u8 r, const u8 g, const u8 b ){
	
	pSprite->rgba[0].r =
	pSprite->rgba[1].r =
	pSprite->rgba[2].r =
	pSprite->rgba[3].r = r;
	
	pSprite->rgba[0].g =
	pSprite->rgba[1].g =
	pSprite->rgba[2].g =
	pSprite->rgba[3].g = g;
	
	pSprite->rgba[0].b =
	pSprite->rgba[1].b =
	pSprite->rgba[2].b =
	pSprite->rgba[3].b = b;
	
}

//===========================================================
/**
 *	@brief	スプライトの加算カラーと加算アルファを設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	r		赤の加算値(0〜255)
 *	@param	g		緑の加算値(0〜255)
 *	@param	b		青の加算値(0〜255)
 *	@param	a		アルファの加算値(0〜255)
 */
//===========================================================
void KLSprite_SetRgbaAdd( KLSprite* pSprite, const u8 r, const u8 g, const u8 b, const u8 a ){
	
	pSprite->rgba[0].r =
	pSprite->rgba[1].r =
	pSprite->rgba[2].r =
	pSprite->rgba[3].r = r;
	
	pSprite->rgba[0].g =
	pSprite->rgba[1].g =
	pSprite->rgba[2].g =
	pSprite->rgba[3].g = g;
	
	pSprite->rgba[0].b =
	pSprite->rgba[1].b =
	pSprite->rgba[2].b =
	pSprite->rgba[3].b = b;
	
	pSprite->rgba[0].a =
	pSprite->rgba[1].a =
	pSprite->rgba[2].a =
	pSprite->rgba[3].a = a;
	
}

//===========================================================
/**
 *	@brief	スプライトの加算アルファを設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	a		加算アルファ値(0〜255)
 */
//===========================================================
inline void KLSprite_SetAlphaAdd( KLSprite* pSprite, const u8 a ){
	
	pSprite->rgba[0].a =
	pSprite->rgba[1].a =
	pSprite->rgba[2].a =
	pSprite->rgba[3].a = a;
	
}

//===========================================================
/**
 *	@brief	スプライト頂点のRGBA加算値を左上、左下、右上、右下の順で設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	topL	左上のRGBA加算値
 *	@param	bottomL	左下のRGBA加算値
 *	@param	topR	右上のRGBA加算値
 *	@param	bottomR	右下のRGBA加算値
 */
//===========================================================
void KLSprite_SetGradAdd( KLSprite* pSprite, const Rgba topL, const Rgba bottomL, const Rgba topR, const Rgba bottomR ){
	
	pSprite->rgba[0] = topL;
	pSprite->rgba[1] = bottomL;
	pSprite->rgba[2] = topR;
	pSprite->rgba[3] = bottomR;
	
}

//===========================================================
/**
 *	@brief	スプライトの左右頂点のRGBA加算値を設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	left	左のRGBA加算値
 *	@param	right	右のRGBA加算値
 */
//===========================================================
void KLSprite_SetGradXAdd( KLSprite* pSprite, const Rgba left, const Rgba right ){
	
	pSprite->rgbaRate[0] = left;
	pSprite->rgbaRate[1] = left;
	pSprite->rgbaRate[2] = right;
	pSprite->rgbaRate[3] = right;
	
}

//===========================================================
/**
 *	@brief	スプライトの上下頂点のRGBA加算値を設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	top		上のRGBA加算値
 *	@param	bottom	下のRGBA加算値
 */
//===========================================================
void KLSprite_SetGradYAdd( KLSprite* pSprite, const Rgba top, const Rgba bottom ){
	
	pSprite->rgba[0] = top;
	pSprite->rgba[1] = bottom;
	pSprite->rgba[2] = top;
	pSprite->rgba[3] = bottom;
	
}

//===========================================================
/**
 *	@brief	スプライトの中心点を設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	cx		中心点X座標
 *	@param	cy		中心点Y座標
 */
//===========================================================
inline void KLSprite_SetCenter( KLSprite* pSprite, const s32 cx,const s32 cy ){
	
	pSprite->cx = cx;
	pSprite->cy = cy;
	
}

//===========================================================
/**
 *	@brief	スプライトの中心点を現在のサイズの半分の位置に設定
 *
 *	@param	pSprite	スプライトのポインタ
 */
//===========================================================
inline void KLSprite_SetCenterSimple( KLSprite* pSprite ){
	
	pSprite->cx = pSprite->w*0.5f;
	pSprite->cy = pSprite->h*0.5f;
	
}

//===========================================================
/**
 *	@brief	スプライトの回転角度を設定
 *
 *	@param	pSprite	スプライトのポインタ
 *	@param	rot		wdegでの角度
 */
//===========================================================
inline void KLSprite_SetRot( KLSprite* pSprite, const wdeg rot ){

	pSprite->rz = rot;

}


