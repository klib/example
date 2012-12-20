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

#include "KLGL.h"
#include "KLSort.h"
#include "KLLabel.h"
#include "KLString.h"
#include "KLMath.h"
#include "KLFont.h"
#include "KLDrawQueue.h"
#include "KLGL.h"
#include <string.h>
#include <stdlib.h>
#include <CoreFoundation/CFString.h>
#include <CoreGraphics/CGContext.h>
#include <CoreGraphics/CGFont.h>
#include <CoreGraphics/CGBitmapContext.h>
#include <CoreText/CTFont.h>

/// CFString コンバート長
#define CFS_CONV_LEN (256)
#define KLLABEL_DEFAULT_GLYPH_WORK_SIZE (1024)
#define KLLABEL_DEFAULT_BBOX_WORK_SIZE (KLLABEL_DEFAULT_GLYPH_WORK_SIZE)
#define KLLABEL_DEFAULT_ADV_WORK_SIZE (KLLABEL_DEFAULT_GLYPH_WORK_SIZE)
#define KLLABEL_DEFAULT_STR_WORK_SIZE (4*1000+1)
#define KLLABEL_DEFAULT_STR32_WORK_SIZE (1000+1)
#define KLLABEL_DEFAULT_EMOJISTR_WORK_SIZE (KLLABEL_DEFAULT_STR_WORK_SIZE)
#define KLLABEL_DEFAULT_EMOJISTR32_WORK_SIZE (KLLABEL_DEFAULT_STR32_WORK_SIZE)
#define KLLABEL_DEFAULT_EMOJIGLYPH_WORK_SIZE (KLLABEL_DEFAULT_STR32_WORK_SIZE)

static CGGlyph*	KLLabel_pGlyphs	= NULL;
static size_t	KLLabel_szGlyph	= 0;

static CGRect*	KLLabel_pRects	= NULL;
static size_t	KLLabel_szRects	= 0;

static bl 		KLLabel_useCustomLineHnextOnly = FALSE;
static u32		KLLabel_customLineH = 24;

static bl KLLabel_lastIsNeedWrap = FALSE;

// todo: バッファ系をまとめる

/// 文字サイズ情報バッファサイズ
static int* KLLabel_pAdv = NULL;

/// 文字サイズ情報バッファサイズ
static size_t KLLabel_szAdv = 0;

/// 矩形バッファ
static CGRect* KLLabel_pBBox = NULL;

/// 矩形バッファサイズ
static size_t KLLabel_szBBox = 0;

/// ToSprite用charバッファ
static char* KLLabel_pStr8 = NULL;

/// ToSprite用charバッファサイズ
static size_t KLLabel_nStr8 = 0;

/// ToSprite用u32バッファ
static u32* KLLabel_pStr32 = NULL;

/// ToSprite用u32バッファサイズ
static size_t KLLabel_nStr32 = 0;

/// ToSprite絵文字用charバッファ
static char* KLLabel_pEmojiStr8 = NULL;

/// ToSprite絵文字用charバッファサイズ
static size_t KLLabel_nEmojiStr8 = 0;

/// ToSprite絵文字用u32バッファ
static u32* KLLabel_pEmojiStr32 = NULL;

/// ToSprite絵文字用u32バッファサイズ
static size_t KLLabel_nEmojiStr32 = 0;

/// ToSprite絵文字用u32文字数
static size_t KLLabel_countEmojiStr32 = 0;

/// 絵文字用矩形バッファ
static CGGlyph* KLLabel_pEmojiGlyph = NULL;

/// 絵文字用矩形バッファサイズ
static size_t KLLabel_szEmojiGlyph = 0;

/// KLLabel共用カラースペース
static CGColorSpaceRef KLLabel_colorSpace = NULL;

inline u16 KLLabel_GetSpaceW( KLLabel* p ){ return p->spacew; };
inline void KLLabel_SetSpaceW( KLLabel* p, u32 spacew ){ p->spacew = spacew; };
inline u16 KLLabel_GetW( KLLabel* p ){ return p->w; };
inline u16 KLLabel_GetH( KLLabel* p ){ return p->h; };
inline void KLLabel_SetColorBG( KLLabel* p, float r, float g, float b, float a ){ p->color.bg.r=r; p->color.bg.g=g; p->color.bg.b=b; p->color.bg.a=a; };
inline void KLLabel_SetColorFill( KLLabel* p, float r, float g, float b, float a ){ p->color.fill.r=r; p->color.fill.g=g; p->color.fill.b=b; p->color.fill.a=a; };
inline void KLLabel_SetColorStroke( KLLabel* p, float r, float g, float b, float a ){ p->color.stroke.r=r; p->color.stroke.g=g; p->color.stroke.b=b; p->color.stroke.a=a; };

static bl KLLabel_IsNotForceWrap = FALSE;
static bl KLLabel_ignoreinfo = FALSE;

static CTFontRef KLLabel_ctref = NULL;

static bl KLLabel_ReallocAdvBuffer( size_t sz ){
	
	if( sz==0 || sz>KLLabel_szAdv )
	{
		KLLabel_szAdv	= sz==0 ? KLLABEL_DEFAULT_ADV_WORK_SIZE : sz;
		
		KLLog("[ KLLabel ] Realloc adv.(%zubytes)\n", KLLabel_szAdv);
		
		KLLabel_pAdv	= (int*)realloc( KLLabel_pAdv, sizeof(int)*KLLabel_szAdv );
	}
	return KLLabel_pAdv!=NULL;
	
}

static bl KLLabel_ReallocGlyphBuffer( size_t sz ){
	
	if( sz==0 || sz>KLLabel_szGlyph )
	{
		KLLabel_szGlyph = sz==0 ? KLLABEL_DEFAULT_GLYPH_WORK_SIZE : sz;
		
		KLLog("[ KLLabel ] Realloc glyph.(%zubytes)\n", KLLabel_szGlyph);
		
		KLLabel_pGlyphs = (CGGlyph*)realloc( KLLabel_pGlyphs, sizeof(CGGlyph)*KLLabel_szGlyph );
	}
	return KLLabel_pGlyphs!=NULL;
	
}

static bl KLLabel_ReallocBBoxBuffer( size_t sz ){
	
	if( sz==0 || sz>KLLabel_szBBox )
	{
		KLLabel_szBBox	= sz==0 ? KLLABEL_DEFAULT_BBOX_WORK_SIZE : sz;
		
		KLLog("[ KLLabel ] Realloc bbox.(%zubytes)\n", KLLabel_szBBox);
		
		KLLabel_pBBox	= (CGRect*)realloc( KLLabel_pBBox, sizeof(CGRect)*KLLabel_szBBox );
	}
	
	return KLLabel_pBBox!=NULL;
	
}


static bl KLLabel_ReallocRectBuffer( size_t sz ){
	
	if( sz==0 || sz>KLLabel_szRects )
	{
		KLLabel_szRects= sz==0 ? KLLABEL_DEFAULT_GLYPH_WORK_SIZE : sz;
		
		KLLog("[ KLLabel ] Realloc rect.(%zubytes)\n", KLLabel_szRects);
		
		KLLabel_pRects = (CGRect*)realloc( KLLabel_pRects, sizeof(CGRect)*KLLabel_szRects );
	}
	
	return KLLabel_pRects!=NULL;
	
}

static bl KLLabel_ReallocStrBuffer( size_t sz ){
	
	if( sz==0 || sz>KLLabel_nStr8 )
	{
		KLLabel_nStr8 = sz==0 ? KLLABEL_DEFAULT_STR_WORK_SIZE : sz;
		
		KLLog("[ KLLabel ] Realloc str8.(%zubytes)\n", KLLabel_nStr8);
		
		KLLabel_pStr8 = (char*)realloc( KLLabel_pStr8, KLLabel_nStr8 );
	}
	
	return KLLabel_pStr8!=NULL;
	
}

static bl KLLabel_ReallocStr32Buffer( size_t sz ){

	if( sz==0 || sz>KLLabel_nStr32 )
	{
		KLLabel_nStr32 = sz==0 ? KLLABEL_DEFAULT_STR32_WORK_SIZE : sz;
		
		KLLog("[ KLLabel ] Realloc str32.(%zubytes)\n", KLLabel_nStr32);
		
		KLLabel_pStr32 = (u32*)realloc( KLLabel_pStr32, sizeof(u32)*KLLabel_nStr32 );
		if( KLLabel_pStr32 )
		{
			memset( KLLabel_pStr32, 0, sizeof(u32)*KLLabel_nStr32 );
		}
	}
	
	return KLLabel_pStr32!=NULL;

}

static bl KLLabel_ReallocEmojiStr( size_t sz ){
	
	if( sz==0 || sz>KLLabel_nEmojiStr8 )
	{
		KLLabel_nEmojiStr8 = sz==0 ? KLLABEL_DEFAULT_EMOJISTR_WORK_SIZE : sz;
		
		KLLog("[ KLLabel ] Realloc emoji str8.(%zubytes)\n", KLLabel_nEmojiStr8);
		
		KLLabel_pEmojiStr8 = (char*)realloc( KLLabel_pEmojiStr8, KLLabel_nEmojiStr8 );
	}
	
	return KLLabel_pEmojiStr8!=NULL;
	
}

static bl KLLabel_ReallocEmojiStr32( size_t sz ){
	
	if( sz==0 || sz>KLLabel_nEmojiStr32 )
	{
		KLLabel_nEmojiStr32 = sz==0 ? KLLABEL_DEFAULT_EMOJISTR32_WORK_SIZE : sz;
		
		KLLog("[ KLLabel ] Realloc emoji str32.(%zubytes)\n", KLLabel_nEmojiStr32);
		
		KLLabel_pEmojiStr32 = (u32*)realloc( KLLabel_pEmojiStr32, sizeof(u32)*KLLabel_nEmojiStr32 );
		if( KLLabel_pEmojiStr32 )
		{
			memset( KLLabel_pEmojiStr32, 0, sizeof(u32)*KLLabel_nEmojiStr32 );
		}
		
	}
	
	return KLLabel_pEmojiStr32!=NULL;
	
}

static bl KLLabel_ReallocEmojiGlyph( size_t sz ){
	
	if( sz==0 || sz>KLLabel_szEmojiGlyph )
	{
		
		KLLabel_szEmojiGlyph = sz==0 ? KLLABEL_DEFAULT_EMOJIGLYPH_WORK_SIZE : sz;
		
		KLLog("[ KLLabel ] Realloc emoji glyph.(%zubytes)\n", KLLabel_szEmojiGlyph);
		
		KLLabel_pEmojiGlyph = (CGGlyph*)realloc( KLLabel_pEmojiGlyph, sizeof(CGGlyph)*KLLabel_szEmojiGlyph );
		if( KLLabel_pEmojiGlyph )
		{
			memset( KLLabel_pEmojiGlyph, 0, sizeof(CGGlyph)*KLLabel_szEmojiGlyph );
		}
		
	}
	
	return KLLabel_pEmojiGlyph!=NULL;
	
}

static void KLLabel_FreeAdvBuffer(){
	
	sfree(KLLabel_pAdv);
	KLLabel_szAdv = 0;
	
}

static void KLLabel_FreeGlyphBuffer(){
	
	sfree(KLLabel_pGlyphs);
	KLLabel_szGlyph = 0;
	
}

static void KLLabel_FreeEmojiGlyphBuffer(){
	
	sfree(KLLabel_pEmojiGlyph);
	KLLabel_szEmojiGlyph = 0;
	
}

static void KLLabel_FreeBBoxBuffer(){
	
	sfree(KLLabel_pBBox);
	KLLabel_szBBox = 0;
	
}

static void KLLabel_FreeRectBuffer(){
	
	sfree(KLLabel_pRects);
	KLLabel_szRects = 0;
	
}

static void KLLabel_FreeStrBuffer(){
	
	sfree(KLLabel_pStr8);
	KLLabel_nStr8 = 0;
	
}

static void KLLabel_FreeStr32Buffer(){
	
	sfree(KLLabel_pStr32);
	KLLabel_nStr32 = 0;
	
}

static void KLLabel_FreeEmojiStrBuffer(){
	
	sfree(KLLabel_pEmojiStr8);
	KLLabel_nEmojiStr8 = 0;
	
}

static void KLLabel_FreeEmojiStr32Buffer(){
	
	sfree(KLLabel_pEmojiStr32);
	KLLabel_nEmojiStr32 = 0;

}

// まだ実験途中で非対応 絵文字フォントをCGGlyphで表示する術はObjC経由しかない?
// This is not availabel. test code.
void KLLabel_InitEmoji( char* pEmojiStr ){
	
	u32		fontsize	= 32;
	size_t	len			= 0;
	
	KLLog("[ KLLabel ] Emoji loaded:%s", pEmojiStr);

	// 文字数(strlenは文字数でなくバッファサイズなので注意)
	len = cstrlen(pEmojiStr);
	
	// 必要に応じてchar→u32変換バッファを再確保
	KLLabel_ReallocEmojiStr32( len+1 );
	
	// 4byteで1文字に変換し絵文字を判別しやすくする
	KLLabel_pEmojiStr32 = chartou32( pEmojiStr, KLLabel_pEmojiStr32 );
	
	// 重複文字をなくす
	KLString_TrimSameChars32( KLLabel_pEmojiStr32, &len );
	
	// 後のバイナリサーチのために文字コードを昇順にソートする
	KLSort_u32( KLLabel_pEmojiStr32, len );	// ソート時に終端NULLは不要
	KLLabel_countEmojiStr32 = len;
	
	// グリフ取得のためにcharに戻す
	KLLabel_ReallocEmojiStr( len*4+1 );
	KLLabel_pEmojiStr8 = u32tochar( KLLabel_pEmojiStr32, KLLabel_pEmojiStr8, strlen32(KLLabel_pEmojiStr32) );
	KLLabel_ReallocEmojiGlyph( len );
	
	// 文字列からグリフの取得
	KLLabel_ctref = CTFontCreateWithName(
										 char2cfs(KLFont_name[KLFontType_AppleColorEmoji],NULL),
										 fontsize, NULL);
	
	// グリフを取得
	CTFontGetGlyphsForCharacters(
								 KLLabel_ctref,
								 char2wchar(KLLabel_pEmojiStr8,0,0,0), KLLabel_pEmojiGlyph, len );
	
	// toSprite時に絵文字コードだったらこちらから参照してCGContextに書けばいけるかとおもいきや
	// うまく描画できなかったので別途方法を探して検討する
	
#if 0
	KLLog("[ KLLabel ] Emoji loaded:%s", KLLabel_pEmojiStr8);

	u32 n = strlen32(KLLabel_pEmojiStr32);
	for(int i=0;i<n;++i)
	{
		KLLog("Glyph[%u] %u\n", i, KLLabel_pGlyphs[i]);
	}
#endif
}


void KLLabel_IgnoreDrawInfo(){
	
	KLLabel_ignoreinfo = TRUE;

}

void KLLabel_UseDrawInfo(){
	
	KLLabel_ignoreinfo = FALSE;

}
#if 0
static void KLLabel_SetupUV( KLLabel* p, u32 texw, u32 texh, s32 x, s32 y, s32 w, s32 h, u32 i ){
	
	p->ppUv[i][0] = GetUVf( texw, x );
	p->ppUv[i][1] = GetUVf( texh, y );
	p->ppUv[i][2] = GetUVf( texw, x+w );
	p->ppUv[i][3] = GetUVf( texh, y );
	p->ppUv[i][4] = GetUVf( texw, x );
	p->ppUv[i][5] = GetUVf( texh, y+h );
	p->ppUv[i][6] = GetUVf( texw, x+w );
	p->ppUv[i][7] = GetUVf( texh, y+h );
	
}

static void KLLabel_SetupUVforText( KLLabel* p, u32 texw, u32 texh, s32 x, s32 y, s32 w, s32 h, u32 i ){
	
	p->ppUv[i][0] = GetUVf( texw, x );
	p->ppUv[i][1] = GetUVf( texh, y+h );
	p->ppUv[i][2] = GetUVf( texw, x+w );
	p->ppUv[i][3] = GetUVf( texh, y+h );
	p->ppUv[i][4] = GetUVf( texw, x );
	p->ppUv[i][5] = GetUVf( texh, y+h+h );
	p->ppUv[i][6] = GetUVf( texw, x+w );
	p->ppUv[i][7] = GetUVf( texh, y+h+h );
	
}
#endif

//=========================================================================
/**
 *	@brief	昇順ソート済み配列から指定インデクスを探す
 *
 *	@param	p			ラベルのポインタ
 *	@param	searchGlyph	検索するWchar値
 *
 *	@return	発見したインデクス / 存在しない数値の場合はMAX値を返す
 */
//=========================================================================
u32 KLLabel_SearchIndex( KLLabel* p, wchar searchWchar ){
	
	int min		= 0;
	int max		= p->len;
	int idx		= 0;
	u32 n		= max;
	
	while( n && max>min )
	{
		idx	= (max-min)*0.5f+min;
		if( p->pMap[idx] > searchWchar )
		{
			max = idx;
		}
		else if( p->pMap[idx] < searchWchar )
		{
			min = idx;
			
		}
		else
		{
			return idx;
		}
		
		n >>= 1;
		
	}
	
	// Not found
	return p->len;
	
}

//=========================================================================
/**
 *	@brief	昇順ソート済み配列から指定インデクスを探す(u32文字列用)
 *
 *	@param	p			ラベルのポインタ
 *	@param	searchGlyph	検索するWchar値
 *
 *	@return	発見したインデクス / 存在しない数値の場合はMAX値を返す
 */
//=========================================================================
u32 KLLabel_SearchIndex32( KLLabel* p, u32 searchu32 ){
	
	int min		= 0;
	int max		= p->len;
	int idx		= 0;
	u32 n		= max;
	
	while( n && max>min )
	{
		idx	= (max-min)*0.5f+min;
		if( p->pMap32[idx] > searchu32 )
		{
			max = idx;
		}
		else if( p->pMap32[idx] < searchu32 )
		{
			min = idx;
			
		}
		else
		{
			return idx;
		}
		
		n >>= 1;
		
	}
	
	// Not found
	return p->len;
	
}

#define KLLABEL_DEFAULT_INFO_NUM (64)
#define KLTEXT_DEFAULT_MAXLEN (256)

void KLLabel_ResetInfo( KLLabel* p ){
	
	p->nInfo = 0;

}

bl KLLabel_AddInfo( KLLabel* p, u32 mode, u32 offset, u32 len, Rgba rgba ){
	
	if( p->nInfo<p->nInfoMax )
	{
		KLLabelDrawInfo* pInfo = &p->pInfo[p->nInfo++];
		
		pInfo->mode		= mode;
		pInfo->offset	= offset;
		pInfo->length	= len;
		pInfo->rgba		= rgba;
		
		KLLabel_UseDrawInfo();
		
		return TRUE;
	}
	
	return FALSE;
	
}

//=========================================================================
/**
 *	@brief	ラベルの初期化
 *
 *	@param	p			ラベルのポインタ
 */
//=========================================================================
void KLLabel_Init( KLLabel* p ){
	
	memset( p, 0, sizeof(KLLabel) );
	
	p->fontsize		= 32;
	p->x			= 0;
	p->y			= p->fontsize;
	p->w			= KLGL_GetMaxTextureSize();
	p->h			= p->fontsize;
	p->spacew		= p->fontsize*0.5f-1;
	
	KLLabel_SetColorBG( p, 0,0,0,0 );
	KLLabel_SetColorStroke( p, 0,0,0,0 );
	KLLabel_SetColorFill( p, 255,255,255,255 );
	
	p->len			= 0;
	p->drawMode		= kCGTextFill;
	p->isAntialias	= 1;
	p->isSmoothing	= 1;
	p->nInfo		= 0;
	p->nInfoMax		= KLLABEL_DEFAULT_INFO_NUM;
	p->pInfo		= (KLLabelDrawInfo*)Calloc( sizeof(KLLabelDrawInfo)*p->nInfoMax );

	sfree(p->pMap);
	sCGContextRelease(p->context);
	sfree( p->pUv );
	
}

//=========================================================================
/**
 *	@brief	フォントサイズを設定する
 */
 //========================================================================= 
void KLLabel_SetFontSize( KLLabel* p, u16 pixel ){

	p->fontsize = pixel;
	
}

//=========================================================================
/**
 *	@brief	指定文字が描画可能かを返す
 *
 *	@param	p	ラベルのポインタ
 *	@param	kc	文字
 *
 *	@return	描画する文字ならTRUE
 */
//=========================================================================
bl KLLabel_IsDrawableCharacter( KLLabel* p, wchar kc ){
	
	return kc>=20;
	
}

//=========================================================================
/**
 *	@brief	指定文字が描画可能かを返す(u32文字列)
 *
 *	@param	p	ラベルのポインタ
 *	@param	c32	u32文字
 *
 *	@return	描画する文字ならTRUE
 */
//=========================================================================
bl KLLabel_IsDrawableCharacter32( KLLabel* p, u32 c32 ){
	
	return c32>=20;
	
}

//=========================================================================
/**
 *	@brief	指定文字が改行コードかを返す
 *
 *	@param	p	ラベルのポインタ
 *	@param	kc	文字
 *
 *	@return	改行文字ならTRUE
 */
//=========================================================================
bl KLLabel_IsNewLineCharacter( KLLabel* p, wchar kc ){

	// todo:描画拒否文字リストで複数管理する
	return kc=='\n';
	
}

//=========================================================================
/**
 *	@brief	指定文字が改行コードかを返す(u32文字列用)
 *
 *	@param	p	ラベルのポインタ
 *	@param	c32	文字
 *
 *	@return	改行文字ならTRUE
 */
//=========================================================================
bl KLLabel_IsNewLineCharacter32( KLLabel* p, u32 c32 ){
	
	// todo:描画拒否文字リストで複数管理する
	return c32=='\n';
	
}

bl KLLabel_ToSprite( KLLabel* p, char* pStr, KLFont* pFont ){

	CTFontRef	ctref		= NULL;
	CGRect		imageRect;
	size_t		sz			= 0;
	size_t		wlen		= 0;
	u32			line_w		= 0;
	u32			ds			= KLFont_GetDescent(pFont, p->fontsize);
	u32			line_y		= ds;
	u32			x			= 0;
	u32			w			= 0;
	u32			maxw		= 0;
	u32*		pStr32		= NULL;
	int*		pAdv		= NULL;
	wchar*		pWchar		= NULL;
	CGRect*		pBBox		= NULL;
	
	KLBench_Start();
	
	p->descent = ds;
	
	// スプライトを紐付け
	KLSprite_Init( &p->sprite );
	p->sprite.isReverseUv = TRUE;
	
	// 作成後の拡大縮小用に設定時の基準サイズを保持する
	p->originsize	= p->fontsize;
	p->leading		= KLFont_GetLeading( pFont, p->fontsize );
	
	// 行間を適当に決める(描画前に変更すれば描画時に随時反映)
	if( p->lineSpacing==0 )
	{
		p->lineSpacing = p->originsize * 1.4f;
	}
	
	// 文字列から必要なデータを計算する
	// @todo	他言語や斜体など横幅が前後を横断するフォントをどう対応するか
	//=========================================
	
	// 文字数(strlenは文字数でなくバッファサイズなので注意)
	p->len = cstrlen(pStr);
	
	// 必要に応じてchar→u32変換バッファを再確保
	KLLabel_ReallocStr32Buffer( p->len+1 );
	
	// 4byteで1文字に変換し絵文字を判別しやすくする
	KLLabel_pStr32 = chartou32( pStr, KLLabel_pStr32 );
	
	// 後のバイナリサーチのために文字コードを昇順にソートする
	KLSort_u32( KLLabel_pStr32, p->len );	// ソート時に終端NULLは不要
	
	// 重複文字をなくす
	KLString_TrimSameChars32( KLLabel_pStr32, &p->len );
	
	if( p->len )
	{
		// 必要に応じてu32→char変換バッファを再確保
		KLLabel_ReallocStrBuffer( p->len*4+4 );
		
		// 絵文字は絵文字用フォントにしか含まれないので
		// 普段使うようなフォントに絵文字グリフは存在しない。
		// よって絵文字コードが必要な場合には一緒にマッピングする。
		// が、まだうまく絵文字を描画する方法が見つかっていないので
		// それまでは非対応として取り除く
		
		//if( !pFont->isEmojiMix )
		{
			//KLString_TrimEmoji32( KLLabel_pStr32, &p->len );
		}
		
		// 文字数を再計算
		p->len = strlen32( KLLabel_pStr32 );
		
		// u32文字列には4byteで1文字を表現する
		// サロゲートペアや文字を組み合わせて表示するコンバインド文字が入っている可能性がある
		// Glyphにした時にこれらが原因でおかしな挙動をしていないか要チェック
		// 特に英語以外の言語文字は動作確認をしていないので注意
		
		// マップバッファの確保
		p->pMap32 = (u32*)Malloc( sizeof(u32)*(p->len) );
		if( !p->pMap32 )
		{
			
			KLLog("[ KLLabel ] Glyph mapping buffer alloc error.\n");
			KLLabel_Unload(p);
			return FALSE;
			
		}
		
		// 先ほどソートしたu32をマップにコピー
		memcpy( p->pMap32, KLLabel_pStr32, sizeof(u32)*(p->len) );
		//KLLog("[ KLLabel ] Map generated.\n");
		
		// グリフ変換の為にソートしたu32をcharに戻す
		KLLabel_pStr8 = u32tochar( KLLabel_pStr32, KLLabel_pStr8, p->len );
		
		// CoreTextの準備
		ctref = CTFontCreateWithName(
					char2cfs(pFont->pName,NULL), p->fontsize, NULL);
		
		pWchar = char2wchar(KLLabel_pStr8,&wlen,0,0);
		
		// wlenがlenより増えてたら
		// サロゲートによる長さ変更がありうる？
		
		if( p->len != wlen )
		{
			KLLog("[KLLabel] len check: %zu %zu\n", p->len, wlen);
		}
		
		// 長さからグリフと矩形バッファを作成
		KLLabel_ReallocGlyphBuffer( wlen );
		KLLabel_ReallocBBoxBuffer( wlen );
		
		// CoreTextのフォント経由でグリフ一覧を取得 ここで0になった文字はフォントに存在しない文字
		CTFontGetGlyphsForCharacters( ctref, pWchar, KLLabel_pGlyphs, wlen );
		
		//KLLog("G0:%u G1:%u G2:%u G3:%u\n", KLLabel_pGlyphs[0], KLLabel_pGlyphs[1], KLLabel_pGlyphs[2], KLLabel_pGlyphs[3]);
		
		// グリフから描画範囲を取得
		CGFontGetGlyphBBoxes( pFont->ref, KLLabel_pGlyphs, wlen, KLLabel_pBBox );
		
		// UVバッファ作成
		p->pUv = (GLfloat*)Calloc( sizeof(GLfloat)*4 * wlen );
		if( !p->pUv )
		{
			KLLog("[ KLLabel ] UV buffer alloc failed.\n");
			KLLabel_Unload(p);
			return FALSE;
		}
		
		// 文字列から各文字情報を取得
		KLLabel_ReallocAdvBuffer( wlen );
		CGFontGetGlyphAdvances(pFont->ref, KLLabel_pGlyphs, wlen, KLLabel_pAdv);
		//KLLog("wUnit:%f\nunitPerEm:%d\n", pFont->wUnit, pFont->unitsPerEm );
		
		// 横幅を算出しつつ必要な高さも求める
		for( int i=0, idx=0; i<p->len; ++i )
		{
			// 2グリフ1文字の時は2BYTE目が0っぽいのでスルー
			if( !KLLabel_pGlyphs || !KLLabel_pGlyphs[i] )
				continue;
			
			if( pFont->isEmojiMix )
			{
				// 別途確保してあるグリフ情報テーブルからサイズを計測
				// まだ実装してません。
			}
			else
			{
				pStr32	= KLLabel_pStr32;
				pBBox	= KLLabel_pBBox;
				pAdv	= KLLabel_pAdv;
				idx		= i;
			}
			
			// グリフ次第で j などの文字が左にマイナスでめり込む要素があるので補正する
			x = pBBox ? (pBBox[idx].origin.x < 0 ? -pBBox[idx].origin.x : 0) : 0;
			x = (int)(x* p->fontsize / (CGFloat)pFont->unitsPerEm+0.5f);
			
			w = pAdv ? (int)(pAdv[idx]* p->fontsize / (CGFloat)pFont->unitsPerEm+0.5f)+x : 0;
			
			// 最大幅を超えるなら改行 改行グリフも然り
			if( line_w + w>=p->w || KLLabel_IsNewLineCharacter32(p,pStr32[i]) )
			{
				line_w =  0;
				line_y += p->leading + ds;
			}
		
			p->pUv[i*4+2] = w;
			p->pUv[i*4+3] = p->fontsize;
			
			p->pUv[i*4+0] = line_w+x;
			p->pUv[i*4+1] = line_y;	// この時点でYはベースラインなので描画時にディセンダも考慮する
			
			//KLLog("[%d] X:%.0f\tY:%.0f\tW:%.0f\tH:%.0f\tBX:%.0f\tBY:%.2f\tBW:%.2f\tBY:%.2f\n", i, pRects[i*4+0], pRects[i*4+1], pRects[i*4+2], pRects[i*4+3], pBBox[i].origin.x, pBBox[i].origin.y, pBBox[i].size.width, pBBox[i].size.height );
			
			line_w += ((pStr32[i]=='\n')? 0: p->pUv[i*4+2])+x;
			
			// 最大幅の更新
			if( line_w > maxw )
			{
				maxw = line_w;
			}
			
		}
		
		// 最低限の必要な高さ
		line_y +=p->fontsize;
		
		// OpenGLテクスチャを2の累乗に揃えたい場合はここで揃える
		//maxw		= KLGL_ValidateTextureSize(maxw);
		//line_h	= KLGL_ValidateTextureSize(line_h);
		
		p->w = maxw;
		p->h = line_y;
		
		// CGContextの作成
		sz		= (sizeof(GLubyte)*p->w*p->h)<<2;
		bin8* pPixels	= (bin8*)Malloc( sz==0?4:sz );
		
		if( !pPixels )
		{
			KLLog("[ KLLabel_ToSprite ] !!! Couldn't alloc the pixel buffer.\n");
			KLLabel_Unload(p);
			return FALSE;
		}
		p->context	= CGBitmapContextCreate (
						pPixels, p->w, p->h, 8, p->w<<2,
						KLLabel_colorSpace, kCGImageAlphaPremultipliedLast );
		
		if( !p->context )
		{
			KLLog("[ KLLabel_ToSprite ] !!! Couldn't alloc CGContext.\n");
			sfree(pPixels);
			KLLabel_Unload(p);
			return FALSE;
		}
		
		CGContextSetInterpolationQuality( p->context, kCGInterpolationHigh );
		
		imageRect.origin.x		= 0;
		imageRect.origin.y		= 0;
		imageRect.size.width	= p->w;
		imageRect.size.height	= p->h;
		
		CGContextClearRect( p->context, imageRect );
		CGContextSetBlendMode( p->context, kCGBlendModeCopy );
		CGContextSetRGBFillColor( p->context, p->color.bg.r/255., p->color.bg.g/255., p->color.bg.b/255., p->color.bg.a/255. );
		CGContextFillRect( p->context, imageRect );
		CGContextSetTextDrawingMode( p->context, p->drawMode );
		CGContextSetFont( p->context, pFont->ref );
		CGContextSetFontSize( p->context, p->fontsize );
		CGContextSetShouldAntialias( p->context, p->isAntialias );
		CGContextSetAllowsAntialiasing( p->context, p->isAntialias );
		CGContextSetShouldSmoothFonts( p->context, p->isSmoothing );
		CGContextSetAllowsFontSmoothing( p->context, p->isSmoothing );
		CGContextSetRGBStrokeColor( p->context, p->color.stroke.r/255., p->color.stroke.g/255., p->color.stroke.b/255., p->color.stroke.a/255. );
		CGContextSetRGBFillColor( p->context, p->color.fill.r/255., p->color.fill.g/255., p->color.fill.b/255., p->color.fill.a/255. );
		
		p->y = KLFont_GetDescent( pFont, p->fontsize );
		p->x = 0;
		
		// 実際に描画する
		for( int i=0; i<p->len; ++i )
		{
			if( KLLabel_IsDrawableCharacter32(p,KLLabel_pStr32[i]) )
			{
				// 2グリフ1文字のものもあるので0チェックで判別(2byte目が0じゃない時もありそう)
				if( KLLabel_pGlyphs[i] )
				{
					CGContextShowGlyphsAtPoint( p->context, p->pUv[i*4+0], p->pUv[i*4+1],  &KLLabel_pGlyphs[i], 1 );
				}
			}
		}
	
		KLSprite_LoadFromPixels( &p->sprite, p->w, p->h, pPixels );
		KLSprite_SetUVFit( &p->sprite,0,0,p->w,p->h );
	
		sfree(pPixels);
	}
	
	KLBench_Stop(1);
	
	return TRUE;
	
}

bl KLLabel_InitSystem(){
	
	if( KLLabel_ReallocGlyphBuffer(0) && KLLabel_ReallocRectBuffer(0) )
	{
		
		if( KLLabel_ReallocGlyphBuffer(0) )
		{
			
			if( KLLabel_ReallocStr32Buffer(0) && KLLabel_ReallocStrBuffer(0) )
			{
				
				if( KLLabel_ReallocBBoxBuffer(0) && KLLabel_ReallocAdvBuffer(0) )
				{
					//KLLabel_InitEmoji( KLSTRSET_EMOJI );
					KLLabel_colorSpace	= CGColorSpaceCreateDeviceRGB();
					return KLLabel_colorSpace != NULL;
				}
				
			}
			
		}
		
	}
	
	return FALSE;
}

void KLLabel_UnloadSystem(){
	
	KLLabel_FreeAdvBuffer();
	KLLabel_FreeBBoxBuffer();
	KLLabel_FreeGlyphBuffer();
	KLLabel_FreeRectBuffer();
	KLLabel_FreeStrBuffer();
	KLLabel_FreeStr32Buffer();
	KLLabel_FreeEmojiStrBuffer();
	KLLabel_FreeEmojiStr32Buffer();
	
	if( KLLabel_colorSpace )
	{
		CGColorSpaceRelease(KLLabel_colorSpace);
	}
	
}

KLSprite* KLLabel_GetSprite( void* pLabel ){
	
	KLLabel* p = (KLLabel*)pLabel;
	
	return &p->sprite;
	
}

GLuint KLLabel_GetTextureID( void* pLabel ){
	
	KLLabel* p = (KLLabel*)pLabel;
	
	return p->sprite.tid;
	
}

static KLLabelAlign KLLabel_nextAlignX = 0;

void KLLabel_NextAlignX( KLLabelAlign align ){
	
	KLLabel_nextAlignX = align;

}

static s32 last_x = 0;
static s32 last_y = 0;

inline s32 KLLabel_GetDrawCharLastX(){
	
	return last_x;

}

inline s32 KLLabel_GetDrawCharLastY(){
	
	return last_y;

}

static bl	isNextDrawCursor= FALSE;
static u32	cursor_w		= 2;
static Rgba	cursor_rgba		= (Rgba){0,0,0,255};

void KLLabel_DrawCursorNext( bl isNextDraw, u32 w, Rgba rgba ){
	
	isNextDrawCursor = isNextDraw;
	cursor_w = w;
	cursor_rgba = rgba;

}

void KLLabel_ResetWordBuffer( s32* pWordStart, s32* pWordLen, u32* pWordTotalW ){
	
    if( pWordStart )
    {
        if( *pWordStart>=0 )
        {
            *pWordStart = 0;
        }
    }
    
    if( pWordLen )
    {
        *pWordLen = 0;
    }
    
    if( pWordTotalW )
    {
        *pWordTotalW = 0;
    }
	
}


//=============================================================================
/**
 *	@brief	再帰で文字を描画する
 */
//=============================================================================
static s32 KLLabel_DrawCharLoop32(
	KLLabel* p, u32* p32, u32 len,
	float x, float y, u32 maxw, Rgba rgba, KLLabelAlign align,
	float offsetx, u32 i, u32 top_i,
	float* pTotalW, s32* pWordStart, s32* pWordLen, u32* pWordTotalW,
	float* pWidth ){

	/// wordStartが最後に単語が始まった位置だが-1でワードラップを使用しない
	float	cw      = 0;
	float	rate	= p->fontsize / p->originsize;
	s32		idx     = KLLabel_SearchIndex32( p, p32[i] );
	u32		next_i  = i;
	bl		isDraw	= FALSE;
    
	// 終端
	if( p32[i]==0 )
	{
        KLLabel_ResetWordBuffer( pWordStart, pWordLen, pWordTotalW );
		return i;
	}
	// 改行コード
	else if( p32[i]=='\n' )
	{
		KLLabel_ResetWordBuffer( pWordStart, pWordLen, pWordTotalW );
		return i;//return i+1;
	}
	// 改行コードを２文字で表したもの
	else if( p32[i]=='\\' && i+1<len && p32[i+1]=='n' )
	{
		KLLabel_ResetWordBuffer( pWordStart, pWordLen, pWordTotalW );
		return i+1;
	}
	// スペース
	else if( p32[i]==' ' )
	{
        KLLabel_ResetWordBuffer( pWordStart, pWordLen, pWordTotalW );
		
		cw	= p->spacew*rate;
		
		if( offsetx+cw >= maxw*rate )
		{
			return i;
		}
		else
		{
			*pTotalW += cw;
			
			// 横幅最大値更新
			if( pWidth )
			{
				if( *pWidth < *pTotalW )
				{
					*pWidth = *pTotalW;
				}
			}
			
			next_i = KLLabel_DrawCharLoop32
			(
			 p, p32, len,
			 x, y, maxw,
			 rgba, align,
			 offsetx+cw, i+1, top_i,
			 pTotalW, pWordStart, pWordLen, pWordTotalW,
			 pWidth
			);
		}
		
	}
	// BOM とりあえず0xFEFFのみ or マッピングテーブルに存在しない文字
	else if( KLString_IsEmoji32(p32[i]) || p32[i]==0xFEFF || idx==p->len )
	{
        KLLabel_ResetWordBuffer( pWordStart, pWordLen, pWordTotalW );
        
		// 描画しないで次へ
		next_i = KLLabel_DrawCharLoop32
		(
		 p, p32, len,
		 x, y, maxw,
		 rgba, align,
		 offsetx+cw,
		 i+1, top_i,
		 pTotalW, pWordStart, pWordLen, pWordTotalW,
		 pWidth
		 );
	}
	// マッピングに存在する文字
	else
	{
		KLLabel_lastIsNeedWrap = !KLLabel_IsNotForceWrap && p32[i]<='}' && i!=len-1;

		cw = p->pUv[4*idx+2]*rate;
		
        if( pWordStart && pWordLen && *pWordStart>=0 )
        {
            if( *pWordLen==0 )
            {
                *pWordStart = i;
            }
            
			if( *pWordLen < len )
			{
            	++(*pWordLen);
			}
        }
        
		if( *pWordLen>len )
        {
		
		}
		else if( (offsetx+cw >= maxw) || ( i>=len ) )
		{
			
			next_i = i-1; //(コール元のループで+1されるのであらかじめ-1しておく)
			return next_i;
		
		}
		else
		{
			*pTotalW += cw;
			
			// 横幅最大値更新
			if( pWidth )
			{
				if( *pWidth < *pTotalW )
				{
					*pWidth = *pTotalW;
				}
			}
			
			next_i = KLLabel_DrawCharLoop32
			(
			 p, p32, len,
			 x, y, maxw,
			 rgba, align,
			 offsetx+cw, i+1, top_i,
			 pTotalW, pWordStart, pWordLen, pWordTotalW,
			 pWidth
			);
		}

	}
	
    if( pWordStart && pWordLen && pWordTotalW && *pWordStart>=0 && *pWordLen )
    {
        // 1行の開始が単語開始と同じ＝＝次の行でも収まらないので要描画
        if( i<len && (!KLLabel_lastIsNeedWrap || *pWordStart == top_i) )
        {
            isDraw  = TRUE;
        }
        else if( i<len && (*pWordStart > i) )
        {
            isDraw      = TRUE;
            next_i      = *pWordStart-1; // 呼び出し元でiをインクリメントしつつループしてるのでそれ考慮の-1
            //*pTotalW    -= cw;
        }
		else
		{
			*pTotalW -= cw;
		}
    }
    else if( i<len )
    {
        isDraw = TRUE;
    }
    
    if( !pWidth && isDraw )
    {
		u8 r = rgba.r;
		u8 g = rgba.g;
		u8 b = rgba.b;
		u8 a = rgba.a;
		KLLabelDrawInfo* pInfo = NULL;
		bl isUnderline = FALSE;
		
		if( !KLLabel_ignoreinfo )
		{
			for( int info=p->nInfo-1; info>=0; --info )
			{
				pInfo = &p->pInfo[info];
				
				if( pInfo->offset<=i && i<pInfo->offset+pInfo->length )
				{
					isUnderline = pInfo->mode == KLLabel_DRAWINFO_UNDERLINE;
					r = pInfo->rgba.r;
					g = pInfo->rgba.g;
					b = pInfo->rgba.b;
					a = pInfo->rgba.a;
					break;
				}
			}
		}

		float ty = p->pUv[4*idx+1]-(p->descent+0.5);
		
		if(p->isflipx)
		{
			KLSprite_SetUVFit(	&p->sprite,
							  p->pUv[4*idx+0]+p->pUv[4*idx+2], ty,
							  p->pUv[4*idx+0], ty+p->pUv[4*idx+3]+p->descent+0.5 );
		}
		else
		{
        	KLSprite_SetUVFit(	&p->sprite,
						p->pUv[4*idx+0], ty,
					  	p->pUv[4*idx+0]+p->pUv[4*idx+2], ty+p->pUv[4*idx+3]+p->descent+0.5 );
		}
		KLSprite_SetWH( &p->sprite, p->pUv[4*idx+2]*rate, p->pUv[4*idx+3]*rate+p->descent+0.5 );
        KLSprite_SetRgbaAdd( &p->sprite, r, g, b, 0 );
		//KLSprite_SetRgba( &p->sprite, r,g,b,a );
		KLSprite_SetRgba( &p->sprite, 0,0,0, a );
		
        if( align==KLLABEL_ALIGN_CENTER )
        {
            x = x-*pTotalW*0.5f;
            KLSprite_SetXY( &p->sprite, (x+offsetx), y );
        }
        else if( align==KLLABEL_ALIGN_R )
        {
            x = x-*pTotalW;
            KLSprite_SetXY( &p->sprite, (x+offsetx), y );
        }
		else
        {
            KLSprite_SetXY( &p->sprite, (x+offsetx), y );
        }
		
		if( isUnderline )
		{
			KL_DrawRect( p->sprite.x, p->sprite.y+p->sprite.h, p->sprite.w, 3*rate, (Rgba){r,g,b,a} );
		}
		else
		{
        	KLSprite_Draw( &p->sprite );
        }
		
        last_x = x+offsetx;
    
    }
	
	if( isDraw  )
	{
		if( last_y < y + p->fontsize )
		{
			last_y = y + p->fontsize;
		}
	}
    
	return next_i;
	
}


float KLLabel_GetStringW( KLLabel* p, char* pStr, u32 len, float maxw ){
	
	float			totalw			= 0;
	float			calcwidth		= 0;
	float			x				= 0;
	float			y				= 0;
	float			rate			= p->fontsize / p->originsize;
	Rgba			rgba			= (Rgba){255,255,255,0};
	KLLabelAlign	align_x_type	= KLLABEL_ALIGN_L;
	s32				wordStart   	= 0;
	s32				wordLen     	= 0;
	u32				wordTotalW  	= 0;
	u32*			p32				= chartou32(pStr, NULL);
	
	KLLabel_lastIsNeedWrap	= FALSE;
	
	if( maxw==0 )
	{
		maxw = 0xFFFFF;
	}
	
	for( int i=0; i>=0&&i<len; ++i )
	{
		int in = i;
		i = KLLabel_DrawCharLoop32(
								   p, p32, len,
								   x, y, maxw,
								   rgba, align_x_type,
								   0, i, i,
								   &totalw, &wordStart, &wordLen, &wordTotalW, &calcwidth );
		
		if(in>i)
		{
			i = in;
		}
		
		y		+= p->lineSpacing*rate;
		totalw	= 0;
	}
	
	KLLabel_useCustomLineHnextOnly	= FALSE;
	KLLabel_IsNotForceWrap			= FALSE;
	isNextDrawCursor				= FALSE;
	
	return calcwidth;
	
}

float KLLabel_GetStringH( KLLabel* p, char* pStr, u32 len, float maxw ){
	
	float			totalw			= 0;
	float			x				= 0;
	float			y				= 0;
	float			rate			= p->fontsize / p->originsize;
	Rgba			rgba			= (Rgba){255,255,255,0};
	KLLabelAlign	align_x_type	= KLLABEL_ALIGN_L;
	s32				wordStart   	= 0;
	s32				wordLen     	= 0;
	u32				wordTotalW  	= 0;
	u32*			p32				= chartou32(pStr, NULL);
	
	KLLabel_lastIsNeedWrap	= FALSE;
	
	if( maxw==0 )
	{
		maxw = 0xFFFFF;
	}
	
	for( int i=0; i>=0&&i<len; ++i )
	{
		int in = i;
		i = KLLabel_DrawCharLoop32(
								   p, p32, len,
								   x, y, maxw,
								   rgba, align_x_type,
								   0, i, i,
								   &totalw, &wordStart, &wordLen, &wordTotalW, NULL );
		
		if(in>i)
		{
			i = in;
		}
		
		y		+= p->lineSpacing*rate;
		totalw	= 0;
	}
	
	KLLabel_useCustomLineHnextOnly	= FALSE;
	KLLabel_IsNotForceWrap			= FALSE;
	isNextDrawCursor				= FALSE;
	
	return y;
	
}

// 次の描画でリセットされる１回限り有効な行幅を設定する
void KLLabel_SetCustomLineH( u32 newline_h )
{
	KLLabel_useCustomLineHnextOnly = TRUE;
	KLLabel_customLineH = newline_h;
}

s32 KLLabel_Draw32( KLLabel* p, u32* p32, u32 len, float x, float y, float maxw, Rgba rgba, KLLabelAlign align_x_type ){
	
	float		totalw		= 0;
	float		rate		= p->fontsize / p->originsize;
	s32			wordStart   = 0;
    s32			wordLen     = 0;
    u32			wordTotalW  = 0;
	
	KLLabel_lastIsNeedWrap	= FALSE;
	last_y					= 0;
	
	if( maxw==0 )
	{
		maxw = 0xFFFFF;
	}
	
    for( int i=0; i>=0&&i<len; ++i )
	{
		int in = i;
		i = KLLabel_DrawCharLoop32(
				p, p32, len,
				x, y, maxw,
				rgba, align_x_type,
				0, i, i,
				&totalw, &wordStart, &wordLen, &wordTotalW, NULL );
        
		if(in>i)
		{
			i = in;
		}
		
		y		+= p->lineSpacing*rate;
		totalw	= 0;
	}
	
	KLLabel_useCustomLineHnextOnly	= FALSE;
	KLLabel_IsNotForceWrap			= FALSE;
	isNextDrawCursor				= FALSE;
	
	return y;
	
}

s32 KLLabel_Draw( KLLabel* p, char* pChar, u32 len, float x, float y, float maxw, Rgba rgba, KLLabelAlign align_x_type ){
	
	return KLLabel_Draw32( p, chartou32(pChar, NULL), len, x, y, maxw, rgba, align_x_type );
	
}

void KLLabel_NextIsNotWrap( void* pLabel ){
	
	KLLabel_IsNotForceWrap = TRUE;

}

//=========================================================================
/**
 *	@brief	テキストオブジェクトのアンロード
 */
//=========================================================================
void KLLabel_Unload( KLLabel* p ){
	
	if( p )
	{
		KLSprite_UnloadWithTexture(&p->sprite);
		sfree(p->pMap);
		sfree(p->pMap32);
		sfree(p->pInfo);
		sfree(p->pUv);
		sCGContextRelease(p->context);
	}
	
}

//=========================================================================
/**
 *	@brief	ディセンダが前方に食い込む幅を取得
 */
//=========================================================================
u32 KLLabel_GetGlyphDecentW( KLLabel* p, CGFontRef fontRef, CGGlyph* pGlyphs, size_t len ){
	
	CGRect grect,frect = CGFontGetFontBBox( fontRef );
	CGFontGetGlyphBBoxes( fontRef, pGlyphs, len, &grect );	
	float fw = 0;
	if( grect.origin.x>=0.0f )
	{
		return 0.0f;
	}
	
	fw			= frect.size.width;
	fw			+= -grect.origin.x * p->fontsize;
	
	return (u32)fw;
	
}



