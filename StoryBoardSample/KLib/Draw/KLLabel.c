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


static bl 		KLLabel_useCustomLineHnextOnly = FALSE;
static u32		KLLabel_customLineH = 24;

static bl KLLabel_lastIsNeedWrap = FALSE;

typedef struct {
	
	/// 解析バッファ
	u32*		pParse;
	
	/// グリフバッファ
	CGGlyph*	pGlyphs;

	/// ToSprite用charバッファ
	char*		pStr8;

	/// ToSprite用u32バッファ
	u32*		pStr32;
	
	u64*		pStr64;
	
	/// ToSprite用utf16バッファ
	wchar*		pUtf16;

} KLLabelBuffer;

typedef struct {
	
	/// 解析バッファサイズ
	size_t		parse;
	
	/// グリフバッファサイズ
	size_t		glyph;
	
	/// ToSprite用charバッファサイズ
	size_t		str8;
	
	/// ToSprite用u32バッファサイズ
	size_t		str32;
	
	size_t		str64;
	
	/// ToSprite用utf16バッファサイズ
	size_t		utf16;
	
} KLLabelBufferSize;

typedef struct {
	
	KLLabelBuffer		buf;
	KLLabelBufferSize	sz;
	
} KLLabelWork;

/// 各種内部処理用バッファ
static KLLabelWork work;

/// KLLabel共用カラースペース
static CGColorSpaceRef KLLabel_colorSpace = NULL;

inline u16 KLLabel_GetSpaceW( KLLabel* p ){ return p->spacew; };
inline void KLLabel_SetSpaceW( KLLabel* p, u32 spacew ){ p->spacew = spacew; };
inline u16 KLLabel_GetW( KLLabel* p ){ return p->w; };
inline u16 KLLabel_GetH( KLLabel* p ){ return p->h; };
inline void KLLabel_SetColorBG( KLLabel* p, float r, float g, float b, float a ){ p->color.bg.r=r; p->color.bg.g=g; p->color.bg.b=b; p->color.bg.a=a; };
inline void KLLabel_SetColorFill( KLLabel* p, float r, float g, float b, float a ){ p->color.fill.r=r; p->color.fill.g=g; p->color.fill.b=b; p->color.fill.a=a; };
inline void KLLabel_SetColorStroke( KLLabel* p, float r, float g, float b, float a ){ p->color.stroke.r=r; p->color.stroke.g=g; p->color.stroke.b=b; p->color.stroke.a=a; };
u64* KLLabel_Parse( KLLabel* p, u64* p64 );

static bl KLLabel_IsNotForceWrap = FALSE;
static bl KLLabel_ignoreinfo = FALSE;

static void KLLabel_UnloadParseBuffer ( KLLabel* p ) {
	
	sfree(p->pColList);
	p->nColList = 0;

}

static void KLLabel_ResetParseBuffer ( KLLabel* p ) {

	p->nColList = 0;
	
}

static bl KLLabel_ReallocParseBuffer( size_t sz ){
	
	if( sz==0 || sz>work.sz.parse )
	{
		work.sz.parse	= sz==0 ? KLLABEL_DEFAULT_PARSEBUF_SIZE : sz;
		
		//KLLog("[ KLLabel ] Realloc parsebuf.(%zubytes)\n", work.sz.parse);
		
		work.buf.pParse	= (u32*)realloc( work.buf.pParse, sizeof(u32)*work.sz.parse );
		
		memset ( work.buf.pParse, 0, sizeof(CGGlyph)*work.sz.parse );
		
	}
	
	return work.buf.pParse != NULL;
	
}

static bl KLLabel_ReallocGlyphBuffer( size_t sz ){
	
	if( sz==0 || sz>work.sz.glyph )
	{
	
		work.sz.glyph = sz==0 ? KLLABEL_DEFAULT_GLYPH_WORK_SIZE : sz;
		
		KLLog("[ KLLabel ] Realloc glyph.(%zubytes)\n", sizeof(CGGlyph)*2*work.sz.glyph);
		
		work.buf.pGlyphs = (CGGlyph*)realloc( work.buf.pGlyphs, sizeof(CGGlyph)*2*work.sz.glyph );
		
		memset ( work.buf.pGlyphs, 0, sizeof(CGGlyph)*2*work.sz.glyph );
	
	}
	return work.buf.pGlyphs!=NULL;
	
}

static bl KLLabel_ReallocUtf16Buffer( size_t sz ){
	
	if( sz==0 || sz>work.sz.utf16 )
	{
		work.sz.utf16	= sz==0 ? KLLABEL_DEFAULT_UTF16_WORK_SIZE : sz;
		
		KLLog("[ KLLabel ] Realloc utf16.(%zubytes)\n", work.sz.utf16);
		
		work.buf.pUtf16	= (wchar*)realloc( work.buf.pUtf16, sizeof(wchar)*work.sz.utf16 );
		
		memset ( work.buf.pUtf16, 0, sizeof(wchar)*work.sz.utf16 );
		
	}
	
	return work.buf.pUtf16!=NULL;
	
}

static bl KLLabel_ReallocStrBuffer( size_t sz ){
	
	if( sz==0 || sz>work.sz.str8 )
	{
		work.sz.str8 = sz==0 ? KLLABEL_DEFAULT_STR_WORK_SIZE : sz;
		
		KLLog("[ KLLabel ] Realloc str8.(%zubytes)\n", work.sz.str8);
		
		work.buf.pStr8 = (char*)realloc( work.buf.pStr8, work.sz.str8 );
	
		memset ( work.buf.pStr8, 0, work.sz.str8 );
		
	}
	
	return work.buf.pStr8!=NULL;
	
}

static bl KLLabel_ReallocStr32Buffer( size_t sz ){

	if( sz==0 || sz>work.sz.str32 )
	{
		work.sz.str32 = sz==0 ? KLLABEL_DEFAULT_STR32_WORK_SIZE : sz;
		
		KLLog("[ KLLabel ] Realloc str32.(%zubytes)\n", work.sz.str32);
		
		work.buf.pStr32 = (u32*)realloc( work.buf.pStr32, sizeof(u32)*work.sz.str32 );
		if( work.buf.pStr32 )
		{
			memset( work.buf.pStr32, 0, sizeof(u32)*work.sz.str32 );
		}
	}
	
	return work.buf.pStr32!=NULL;

}

static bl KLLabel_ReallocStr64Buffer( size_t sz ){
	
	if( sz==0 || sz>work.sz.str64 )
	{
		work.sz.str64 = sz==0 ? KLLABEL_DEFAULT_STR64_WORK_SIZE : sz;
		
		KLLog("[ KLLabel ] Realloc str64.(%zubytes)\n", work.sz.str64);
		
		work.buf.pStr64 = (u64*)realloc( work.buf.pStr64, sizeof(u64)*work.sz.str64 );
		if( work.buf.pStr64 )
		{
			memset( work.buf.pStr64, 0, sizeof(u32)*work.sz.str64 );
		}
	}
	
	return work.buf.pStr32!=NULL;
	
}

static void KLLabel_FreeGlyphBuffer(){
	
	sfree(work.buf.pGlyphs);
	work.sz.glyph = 0;
	
}

static void KLLabel_FreeUtf16Buffer(){
	
	sfree(work.buf.pUtf16);
	work.sz.utf16 = 0;
	
}

static void KLLabel_FreeStrBuffer(){
	
	sfree(work.buf.pStr8);
	work.sz.str8 = 0;
	
}

static void KLLabel_FreeStr32Buffer(){
	
	sfree(work.buf.pStr32);
	work.sz.str32 = 0;
	
}

void KLLabel_IgnoreDrawInfo(){
	
	KLLabel_ignoreinfo = TRUE;

}

void KLLabel_UseDrawInfo(){
	
	KLLabel_ignoreinfo = FALSE;

}

//=========================================================================
/**
 *	@brief	昇順ソート済み配列から指定インデクスを探す(u64文字列用)
 *
 *	@param	p			ラベルのポインタ
 *	@param	search64	検索するu64値
 *
 *	@return	発見したインデクス / 存在しない数値の場合はMAX値を返す
 */
//=========================================================================
u32 KLLabel_SearchIndex( KLLabel* p, u64 search64 ){
	
	int min		= 0;
	int max		= (int)p->len;
	int idx		= 0;
	u32 n		= max;
	
	while( n && max>min )
	{
		idx	= (max-min)*0.5f+min;
		
		if( p->pMap64[idx] > search64 )
		{
			max = idx;
		}
		else if( p->pMap64[idx] < search64 )
		{
			min = idx;
			
		}
		else if( p->pMap64[idx] == search64 )
		{
			return idx;
		}
		
		n >>= 1;
		
	}
	
	// Not found
	return (u32)p->len;
	
}

#define KLLABEL_DEFAULT_INFO_NUM (64)
#define KLTEXT_DEFAULT_MAXLEN (256)

void KLLabel_ResetInfo( KLLabel* p ){
	
	p->nColList = 0;

}

bl KLLabel_AddInfo( KLLabel* p, u32 mode, u32 offset, u32 len, Rgba rgba ){
	
	KLLabelAttributes* pAttr = NULL;
	
	if( p->nColList >= p->reservedColList )
	{
		
		p->reservedColList++;
		p->pColList = realloc( p->pColList, sizeof(KLLabelAttributes)*(p->reservedColList) );
		
	}
	
	
	pAttr = &p->pColList[p->nColList++];
	
	pAttr->mode		= mode;
	pAttr->offset	= offset;
	pAttr->length	= len;
	pAttr->rgba		= rgba;
	
	KLLabel_UseDrawInfo();

	return TRUE;
	
}

void KLLabel_InitTagInfo ( KLLabelTag* pTagInfo ) {
	
	pTagInfo->colorStartChr	= KLLABEL_TAG_COL_START;
	pTagInfo->colorEndChr	= KLLABEL_TAG_COL_END;
	
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
	p->y			= p->fontsize;
	p->w			= KLGL_GetMaxTextureSize();
	p->h			= p->fontsize;
	p->spacew		= p->fontsize*0.25f+0.5f;
	p->escapeTag	= FALSE;
	
	KLLabel_SetColorFill( p, 255,255,255,255 );
	
	KLLabel_InitTagInfo ( &p->taginfo );
	
	p->isAntialias	= 1;
	p->isSmoothing	= 1;
	p->reservedColList		= KLLABEL_DEFAULT_RESERVE_COLLIST;
	
	p->pColList = Malloc(sizeof(KLLabelAttributes)*p->reservedColList);
	memset( p->pColList, 0, sizeof(KLLabelAttributes)*p->reservedColList );
	
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
bl KLLabel_IsDrawableCharacter( KLLabel* p, u64 kc ){
	
	return kc>=20;
	
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


static bl KLLabel_IsEmoji ( u64 str64 ) {

	return UTF32_IsEmoji( str64>>32 & 0xFFFFFFFF ) || UTF32_IsEmoji( str64 & 0xFFFFFFFF );
	
}

/// 結合文字を１文字のUTF-16コードに変換
static bl KLLabel_CombinedWstr (wchar* pWstr, size_t* wclen) {

	bl ret = FALSE;
	
	if ( wclen && *wclen>1 && pWstr )
	{
	
		switch ( pWstr[1] ) {
			
			// 囲い文字
			case 0x20E3:
			{
				if ( pWstr[0]==0x23 || (pWstr[0]>=0x30 && pWstr[0]<=0x39) )
				{
				
					// #
					if ( pWstr[0]==0x23 )
					{
					
						pWstr[0] = 0xE210;
						
					}
					// 0 - 9
					else
					{
					
						switch ( pWstr[0]-0x30 )
						{
						
							case 0: pWstr[0] = 0xE225; break;
							default:pWstr[0] = pWstr[0]-0x31 + 0xE21C; break;
							
						}
						
					}
					
					pWstr[1]	= 0;
					*wclen		= 1;
					ret			= TRUE;
					
				}
				
			}
			break;
				
		}
		
		if ( *wclen>3 )
		{
			bl isConv = FALSE;
			
			if ( pWstr[0]==0xD83C && pWstr[2]==0xD83C )
			{
			
				if ( pWstr[1]>=0xDDE6 && pWstr[1]<=0xDDFF && pWstr[3]>=0xDDE6 && pWstr[3]<=0xDDFF )
				{
					
					// JP
					if ( pWstr[1]==0xDDEF && pWstr[3]==0xDDF5 )
					{
					
						pWstr[0] = 0xE50B;
						isConv = TRUE;
						
					}
					// KR
					else if ( pWstr[1]==0xDDF0 && pWstr[3]==0xDDF7 )
					{
						
						pWstr[0] = 0xE514;
						isConv = TRUE;
						
					}
					// DE
					else if ( pWstr[1]==0xDDE9 && pWstr[3]==0xDDEA )
					{
						
						pWstr[0] = 0xE50E;
						isConv = TRUE;
						
					}
					// CN
					else if ( pWstr[1]==0xDDE8 && pWstr[3]==0xDDF3 )
					{
						
						pWstr[0] = 0xE513;
						isConv = TRUE;
						
					}
					// US
					else if ( pWstr[1]==0xDDFA && pWstr[3]==0xDDF8 )
					{
						
						pWstr[0] = 0xE50C;
						isConv = TRUE;
						
					}
					// FR
					else if ( pWstr[1]==0xDDEB && pWstr[3]==0xDDF7 )
					{
						
						pWstr[0] = 0xE50D;
						isConv = TRUE;
						
					}
					// ES
					else if ( pWstr[1]==0xDDEA && pWstr[3]==0xDDF8 )
					{
						
						pWstr[0] = 0xE511;
						isConv = TRUE;
						
					}
					// IT
					else if ( pWstr[1]==0xDDEE && pWstr[3]==0xDDF9 )
					{
						
						pWstr[0] = 0xE50F;
						isConv = TRUE;
						
					}
					// RU
					else if ( pWstr[1]==0xDDF7 && pWstr[3]==0xDDFA )
					{
						
						pWstr[0] = 0xE512;
						isConv = TRUE;
						
					}
					// GB
					else if ( pWstr[1]==0xDDEC && pWstr[3]==0xDDE7 )
					{
						
						pWstr[0] = 0xE510;
						isConv = TRUE;
						
					}
					
				}
				
			}
			
			if (isConv)
			{

				pWstr[1]	= 0;
				*wclen		= 1;
			
			}
			
		}
		
	}
	
	return ret;
	
}

//============================================================
/**
 *	@brief	ラベルに使用する文字をテクスチャにマッピングする
 *
 *	@param	p		ラベルのポインタ
 *	@param	pStr	マッピングする文字を文字列で複数指定
 *	@param	pFont	マッピングに使用するフォント
 *
 *	@return	正常終了でTRUE
 */
//============================================================
bl KLLabel_Setup ( KLLabel* p, char* pStr, KLFont* pFont ) {
	
	CTFontRef		ctref		= NULL;
	CTFontRef		ctref2nd	= NULL;
	CTFontRef		ctref3rd	= NULL;
	CTFontRef		ctrefEmoji	= NULL;
	CFStringRef		emojicfs	= NULL;
	CFStringRef		cfs2nd		= NULL;
	CFStringRef		cfs3rd		= NULL;
	CGContextRef	context		= NULL;
	CGRect			imageRect;
	size_t			szBytes = strlen(pStr);
	
	size_t sz64	= 0;
	size_t wclen= 0;
	u32 offset	= 0;
	u32 goffset	= 0;
	u32 wcoffset= 0;
	u8	c		= 0;
	u64 a		= 0;
	
	u64* p64	= NULL;
	
	u32			line_w		= 0;
	u32			ds			= KLFont_GetDescent(pFont, p->fontsize);
	u32			line_y		= ds;
	u32			x			= 0;
	u32			w			= 0;
	u32			maxw		= 0;
	u64*		pStr64		= NULL;
	u32*		pStr32		= NULL;
	u8*			pFlg2nd		= NULL;
	u8*			pFlg3rd		= NULL;
	u32*		pCounts		= NULL;
	
	size_t		moffset		= 0;
	size_t		szPixels	= 0;
	bin8*		pPixels		= NULL;
	char		temp[256];
	
	bl			ret			= TRUE;
	
	if( p->pMap64 )
	{
		KLLabel_Unload(p);
		KLLabel_Init(p);
	}
	
	//KLBench_Start();
	
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
	
	// @todo	他言語や斜体など横幅が前後を横断するフォントをどう対応するか
	
	// CoreTextの準備
	emojicfs	= char2cfs(KLFont_name[KLFontType_AppleColorEmoji],NULL);
	cfs2nd		= char2cfs(KLFont_name[KLFontType_ArialUnicodeMS],NULL);
	cfs3rd		= char2cfs(KLFont_name[KLFontType_AppleGothic], NULL);
	ctref		= CTFontCreateWithGraphicsFont( pFont->ref, p->fontsize, NULL, NULL );
	ctref2nd	= CTFontCreateWithName ( cfs2nd, p->fontsize, NULL );
	ctref3rd	= CTFontCreateWithName ( cfs3rd, p->fontsize, NULL );
	ctrefEmoji	= CTFontCreateWithName ( emojicfs, p->fontsize, NULL );
	sCFRelease(cfs3rd);
	sCFRelease(cfs2nd);
	sCFRelease(emojicfs);
	
	// UTF-8を１文字ずつU64に分割
	p64 = UTF8_toU64all( pStr, &p64, &sz64 );
	
	// ソートして重複バイトを結合
	KLSort_u64( p64, sz64 );
	KLString_TrimSameChars64( p64, &sz64 );
	
	// グリフとwchar(UTF-16)用バッファの確保
	KLLabel_ReallocUtf16Buffer( sz64+1 );
	KLLabel_ReallocGlyphBuffer( sz64 );
	
	// 現在の文字数（この後の処理で減少する事あり）
	p->len = sz64;
	
	// 長さからバッファを作成
	p->pBBox = (CGRect*) Malloc( sizeof(CGRect)*p->len );
	memset ( p->pBBox, 0, sizeof(CGRect)*p->len );
	
	// Advバッファを確保
	p->pAdv	= (CGSize*) Malloc ( sizeof(CGSize)*p->len );
	memset ( p->pAdv, 0, sizeof(CGSize)*p->len );
	
	// カウントバッファ
	pCounts = (u32*) Malloc( sizeof(u32)*p->len );
	if( !pCounts )
	{
		
		KLLog("[ KLLabel ] Glyph count buffer alloc error.\n");
		ret = FALSE;
		goto finally;
		
	}
	
	// マップバッファの確保
	p->pMap64 = (u64*)Malloc( sizeof(u64)*(p->len) );
	if( !p->pMap64 )
	{
		
		KLLog("[ KLLabel ] Glyph mapping buffer alloc error.\n");
		ret = FALSE;
		goto finally;
		
	}
	
	// 代替フォントフラグの確保
	pFlg2nd = (u8*)Malloc( sizeof(u8)*(p->len/8+1) );
	if( !pFlg2nd )
	{
		
		KLLog("[ KLLabel ] Glyph mapping flags(2) alloc error.\n");
		ret = FALSE;
		goto finally;
		
	}
	memset( pFlg2nd, 0, sizeof(u8)*(p->len/8+1) );
	
	pFlg3rd = (u8*)Malloc( sizeof(u8)*(p->len/8+1) );
	if( !pFlg3rd )
	{
		
		KLLog("[ KLLabel ] Glyph mapping flags(3) alloc error.\n");
		ret = FALSE;
		goto finally;
		
	}
	memset( pFlg3rd, 0, sizeof(u8)*(p->len/8+1) );
	
	
	// 存在するグリフだけチェックする
	// グリフ変換自体はまとめてCTFontGetGlyphsForCharactersで行えるが、
	// 現状１文字ずつ変換しないと個々のグリフ有無判別ができない
	for (offset=0; offset<p->len; ++offset)
	{
		
		// グリフカウント
		size_t glyphlen = 0;
		
		// UTF-16化用に１文字ずつcharに戻す
		char* pStr8			= U64_toUTF8 (p64[offset]);
		
		// char to UTF-16
		wchar* pWstr		= char2wchar(pStr8, &wclen, 0, 0);
		
		// temp glyphs
		CGGlyph glyphs[]	= {0,0,0,0, 0,0,0,0, 0};
		
		int n				= 1;
		
		bl isEmoji			= FALSE;
		bl isNotFound		= FALSE;
		bl is2nd			= FALSE;
		bl is3rd			= FALSE;
		
		// フォントに存在するか？(同一コードが複数フォント内に混在する場合は絵文字優先)
		if ( KLLabel_IsEmoji (p64[offset]) )
		{
			size_t before = wclen;
			
			isEmoji = TRUE;
			
			// ここで結合文字のエントリーポイントを変換しないとグリフも結合しない状態２文字扱いになる
			KLLabel_CombinedWstr (pWstr, &wclen);
			
		}
		
		// 絵文字
		if ( isEmoji )
		{
			
			isNotFound = !CTFontGetGlyphsForCharacters( ctrefEmoji, pWstr, glyphs, wclen );
			
		}
		else
		{
			
			isNotFound = !CTFontGetGlyphsForCharacters( ctref, pWstr, glyphs, wclen );
			
			// 代替で特殊文字を再チェック
			if (isNotFound)
			{
				
				isNotFound	= !CTFontGetGlyphsForCharacters( ctref2nd, pWstr, glyphs, wclen );
				is2nd		= !isNotFound;
			
				if (isNotFound)
				{
					
					isNotFound	= !CTFontGetGlyphsForCharacters( ctref3rd, pWstr, glyphs, wclen );
					is3rd		= !isNotFound;
					
				}
				
			}
			
		}
		
		// フォントに存在しない場合は詰める
		if ( isNotFound )
		{
			
			// u64リストの詰め
			KLLog ("[KLLabel] ! Notfound character(0x%llx) in this font.\n", p64[offset] );
			
		}
		else
		{
			CTFontRef _ctref = ctref;
			
			glyphlen = wclen;
			
			// 有効なグリフだけリストに詰めていく
			p->pMap64[moffset] = p64[offset];
			
			if (is2nd)
			{
				
				pFlg2nd[moffset/8] |= 1<<(moffset%8);
				
			}
			
			glyphlen++;
			
			memcpy (&work.buf.pGlyphs[goffset], glyphs, sizeof(wchar)*glyphlen);
			
			// 有効なグリフのUTF-16だけリストに詰めていく
			memcpy (&work.buf.pUtf16[wcoffset], pWstr, sizeof(wchar)*wclen);
			
			if ( isEmoji )
			{
				
				_ctref = ctrefEmoji;
				
			}
			
			if ( is2nd )
			{
				
				_ctref = ctref2nd;
				
			}
			
			if ( is3rd )
			{
				
				_ctref = ctref3rd;
				
			}
			
			// UTF-16リストに沿ったBBOXを取得(ここも１文字ずつ)
			CTFontGetBoundingRectsForGlyphs(_ctref,
											kCTFontOrientationDefault,
											glyphs,
											&p->pBBox[moffset],
											1 );
			
			// 文字列から各文字情報を取得(ここも１文字ずつ)
			CTFontGetAdvancesForGlyphs(_ctref,
									   kCTFontOrientationDefault,
									   glyphs,
									   &p->pAdv[moffset],
									   1 );
			
			// 有効なグリフのUTF-16総数は wcoffset で管理
			wcoffset += wclen;
			
			// 各カウントを配列に記録
			pCounts[moffset] = (u32) glyphlen;
			
			goffset += glyphlen;
			
			moffset++;
			
		}
		
	}

	p->len = moffset;
	
	// UVバッファ作成
	p->pUv = (GLfloat*)Calloc( sizeof(GLfloat)*4 * p->len );
	if( !p->pUv )
	{
		KLLog("[ KLLabel ] UV buffer alloc failed.\n");
		ret = FALSE;
		goto finally;
	}
	
	// 横幅を算出しつつ必要な高さも求める
	for( int i=0; i<p->len; ++i )
	{
		bl isEmoji = KLLabel_IsEmoji( p->pMap64[i] );

		x = p->pBBox[i].origin.x * (p->pBBox[i].origin.x < 0 ? -1 : 0);
		
		w = p->pAdv[i].width + x + 1.5f;
	
		// 最大幅を超えるなら改行 改行グリフも然り
		if( line_w + w + x>=p->w || KLLabel_IsNewLineCharacter32(p, (u32)p->pMap64[i]) )
		{
			line_w =  0;
			line_y += p->leading + ds + p->fontsize;
		}
		
		p->pUv[i*4+2] = w;
		p->pUv[i*4+3] = p->fontsize;
		
		p->pUv[i*4+0] = line_w+x;
		p->pUv[i*4+1] = line_y;	// この時点でYはベースラインなので描画時にディセンダも考慮する
		
		//KLLog("[%d] X:%.0f\tY:%.0f\tW:%.0f\tH:%.0f\tBX:%.0f\tBY:%.2f\tBW:%.2f\tBY:%.2f\n", i, p->pUv[i*4+0], p->pUv[i*4+1], p->pUv[i*4+2], p->pUv[i*4+3], pBBox[i].origin.x, pBBox[i].origin.y, pBBox[i].size.width, pBBox[i].size.height );
		
		line_w += ((p->pMap64[i]=='\n')? 0: w)+x;
		
		// 最大幅の更新
		if( line_w > maxw )
		{
			
			maxw = line_w;
			
		}
		
	}
	
	// 最低限の必要な高さ
	line_y += p->leading + ds + p->fontsize;
	
	// OpenGLテクスチャを2の累乗に揃えたい場合はここで揃える
	maxw	= KLGL_ValidateTextureSize(maxw);
	line_y	= KLGL_ValidateTextureSize(line_y);
	
	p->w = maxw;
	p->h = line_y;
	
	// CGContextの作成
	szPixels	= (sizeof(GLubyte)*p->w*p->h)<<2;
	pPixels		= (bin8*)Malloc( szPixels==0?4:szPixels );
	
	if( !pPixels )
	{
		KLLog("[ KLLabel_Setup ] !!! Couldn't alloc the pixel buffer.\n");
		ret = FALSE;
		goto finally;
	}
	
	context	= CGBitmapContextCreate (	pPixels, p->w, p->h, 8, p->w<<2,
										KLLabel_colorSpace, kCGImageAlphaPremultipliedLast );
	
	if( !context )
	{
		KLLog("[ KLLabel_Setup ] !!! Couldn't alloc CGContext.\n");
		ret = FALSE;
		goto finally;
	}

	CGContextSetAllowsAntialiasing( context, p->isAntialias );
	CGContextSetAllowsFontSmoothing( context, p->isSmoothing );
	
	imageRect.origin.x		= 0;
	imageRect.origin.y		= 0;
	imageRect.size.width	= p->w;
	imageRect.size.height	= p->h;
	
	CGContextClearRect( context, imageRect );
	
	p->y = KLFont_GetDescent( pFont, p->fontsize );
	p->x = 0;
	
	// 実際に描画する
	for( int i=0,c=0; i<p->len; ++i )
	{
		CGPoint pt = CGPointMake(p->pUv[i*4+0], p->pUv[i*4+1]);
		
		// 絵文字
		if ( KLLabel_IsEmoji(p->pMap64[i]) )
		{
			
			CTFontDrawGlyphs( ctrefEmoji, &work.buf.pGlyphs[c], &pt, 1, context );
			
		}
		// 絵文字以外
		else
		{
		
			if ( (pFlg3rd[i/8]>>(i%8))&1 )
			{
				
				CTFontDrawGlyphs( ctref3rd, &work.buf.pGlyphs[c], &pt, 1, context );
				
			}
			else if ( (pFlg2nd[i/8]>>(i%8))&1 )
			{

				CTFontDrawGlyphs( ctref2nd, &work.buf.pGlyphs[c], &pt, 1, context );
			
			}
			else
			{
			
				CTFontDrawGlyphs( ctref, &work.buf.pGlyphs[c], &pt, 1, context );
				
			}
			
		}
		
		c += pCounts[i];
	
	}
	
	KLSprite_LoadFromPixels( &p->sprite, p->w, p->h, pPixels );
	KLSprite_SetUVFit( &p->sprite,0,0,p->w,p->h );
	
finally:
//--------------------
	
	sCGContextRelease(context);
	sCFRelease(ctref);
	sCFRelease(ctrefEmoji);
	sCFRelease(ctref3rd);
	sCFRelease(ctref2nd);
	
	sfree(pCounts);
	sfree(pPixels);
	sfree(p64);
	
	sfree(pFlg2nd);
	sfree(pFlg3rd);
	
	if (!ret)
	{
		KLLabel_Unload(p);
	}
	//KLBench_Stop(1);
	
	return ret;
	
}

bl KLLabel_InitSystem(){
	
	if( KLLabel_ReallocGlyphBuffer(0) )
	{
		
		if( KLLabel_ReallocGlyphBuffer(0) )
		{
			
			if( KLLabel_ReallocStr32Buffer(0) && KLLabel_ReallocStrBuffer(0) )
			{
				
				KLLabel_colorSpace	= CGColorSpaceCreateDeviceRGB();
					
				if ( KLLabel_colorSpace != NULL)
				{
					
					return KLLabel_ReallocParseBuffer(0);
					
				}
				
			}
			
		}
		
	}
	
	return FALSE;
}

void KLLabel_UnloadSystem(){
	
	KLLabel_FreeGlyphBuffer();
	KLLabel_FreeUtf16Buffer();
	KLLabel_FreeStrBuffer();
	KLLabel_FreeStr32Buffer();
	
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
static s32 KLLabel_DrawChar(
	KLLabel* p, u64* p64, size_t len,
	float x, float y, u32 maxw, Rgba rgba, KLLabelAlign align,
	float offsetx, u32 i, u32 top_i,
	float* pTotalW, s32* pWordStart, s32* pWordLen, u32* pWordTotalW,
	float* pWidth ){

	/// wordStartが最後に単語が始まった位置だが-1でワードラップを使用しない
	float	cw      = 0;
	float	rate	= p->fontsize / p->originsize;
	s32		idx     = KLLabel_SearchIndex( p, p64[i] );
	u32		next_i  = i;
	bl		isDraw	= FALSE;
	bl		isEmoji = KLLabel_IsEmoji(p64[i]);
	
	// 終端
	if( p64[i]==0 )
	{
        KLLabel_ResetWordBuffer( pWordStart, pWordLen, pWordTotalW );
		return i;
	}
	// 改行コード
	else if( p64[i]=='\n' )
	{
		KLLabel_ResetWordBuffer( pWordStart, pWordLen, pWordTotalW );
		return i;//return i+1;
	}
	// 改行コードを２文字で表したもの
	else if( p64[i]=='\\' && i+1<len && p64[i+1]=='n' )
	{
		KLLabel_ResetWordBuffer( pWordStart, pWordLen, pWordTotalW );
		return i+1;
	}
	// スペース
	else if( p64[i]==' ' )
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
			
			next_i = KLLabel_DrawChar
			(
			 p, p64, len,
			 x, y, maxw,
			 rgba, align,
			 offsetx+cw, i+1, top_i,
			 pTotalW, pWordStart, pWordLen, pWordTotalW,
			 pWidth
			);
		}
		
	}
	// BOM とりあえず0xFEFFのみ or マッピングテーブルに存在しない文字
	else if( p64[i]==0xFEFF || (!isEmoji && idx==p->len) )
	{
        KLLabel_ResetWordBuffer( pWordStart, pWordLen, pWordTotalW );
        
		// 描画しないで次へ
		next_i = KLLabel_DrawChar
		(
		 p, p64, len,
		 x, y, maxw,
		 rgba, align,
		 offsetx+cw,
		 i+1, top_i,
		 pTotalW, pWordStart, pWordLen, pWordTotalW,
		 pWidth
		 );
	}
	// マッピングに存在する文字 or 絵文字
	else
	{
		KLLabel_lastIsNeedWrap = !KLLabel_IsNotForceWrap && p64[i]<='}' && i!=len-1;

		cw = p->pUv[4*idx+2]*rate + p->marginx +(p->pBBox[idx].origin.x<0?p->pBBox[idx].origin.x:0);
		
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
			
			next_i = KLLabel_DrawChar
			(
			 p, p64, len,
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
		
		bl isUnderline = FALSE;
		
		if( !KLLabel_ignoreinfo )
		{
			
			for ( int c=p->nColList-1; p->nColList && c>=0; --c  )
			{
				KLLabelAttributes* pAttr = (KLLabelAttributes*) &p->pColList[c];
				
				// 暫定で指定後、長さ関係なしで継続
				if( pAttr->offset<=i )//&& i<pAttr->offset+pAttr->length )
				{
					isUnderline = pAttr->mode == KLLabel_DRAWINFO_UNDERLINE;
					
					if ( pAttr->mode == KLLabel_DRAWINFO_COLOR )
					{
						r = pAttr->rgba.r;
						g = pAttr->rgba.g;
						b = pAttr->rgba.b;
						a = pAttr->rgba.a;
					}
					
					break;
				}
				
			}
		}

		float ty = p->pUv[4*idx+1] - (p->descent+0.5);
		
		if ( isEmoji )
		{
		
			r = g = b = a = 0xFF;
			
		}
		
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
		KLSprite_SetWH( &p->sprite, p->pUv[4*idx+2]*rate, p->pUv[4*idx+3]*rate +(p->descent+0.5) );
        
		if ( isEmoji )
		{
			KLSprite_SetRgbaAdd( &p->sprite, 0, 0, 0, 0);
			KLSprite_SetRgba( &p->sprite, 0xFF,0xFF,0xFF, a );
		}
		else
		{
			KLSprite_SetRgbaAdd( &p->sprite, r, g, b, 0 );
			KLSprite_SetRgba( &p->sprite, 0,0,0, a );
		}
		
		
        if( align==KLLABEL_ALIGN_CENTER )
        {
            x = x-*pTotalW*0.5f +(p->pBBox[idx].origin.x<0?p->pBBox[idx].origin.x:0);
            KLSprite_SetXY( &p->sprite, (x+offsetx), y );
        }
        else if( align==KLLABEL_ALIGN_R )
        {
            x = x-*pTotalW +(p->pBBox[idx].origin.x<0?p->pBBox[idx].origin.x:0);
            KLSprite_SetXY( &p->sprite, (x+offsetx), y );
        }
		else
        {
            KLSprite_SetXY( &p->sprite, (x+offsetx) +(p->pBBox[idx].origin.x<0?p->pBBox[idx].origin.x:0), y );
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


float KLLabel_GetStringW( KLLabel* p, char* pStr, size_t len, float maxw ){
	
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
	size_t			sz				= 0;
	u64*			p64				= UTF8_toU64all(pStr, NULL, &sz);
	
	// 描画するごとにタグ解析情報をリセットする
	KLLabel_ResetParseBuffer(p);
	
	// 新たにタグを解析
	KLLabel_Parse( p, p64 );
	
	KLLabel_lastIsNeedWrap	= FALSE;
	
	if( maxw==0 )
	{
		maxw = 0xFFFFF;
	}
	
	for( int i=0; i>=0&&i<len; ++i )
	{
		int in = i;
		i = KLLabel_DrawChar (
								   p, p64, len,
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

float KLLabel_GetStringH( KLLabel* p, char* pStr, size_t len, float maxw ){
	
	float			totalw			= 0;
	float			x				= 0;
	float			y				= 0;
	float			rate			= p->fontsize / p->originsize;
	Rgba			rgba			= (Rgba){255,255,255,0};
	KLLabelAlign	align_x_type	= KLLABEL_ALIGN_L;
	s32				wordStart   	= 0;
	s32				wordLen     	= 0;
	u32				wordTotalW  	= 0;
	size_t			sz				= 0;
	u64*			p64				= UTF8_toU64all(pStr, NULL, &sz);
	
	// 描画するごとにタグ解析情報をリセットする
	KLLabel_ResetParseBuffer(p);
	
	// 新たにタグを解析
	KLLabel_Parse( p, p64 );
	
	KLLabel_lastIsNeedWrap	= FALSE;
	
	if( maxw==0 )
	{
		maxw = 0xFFFFF;
	}
	
	for( int i=0; i>=0&&i<len; ++i )
	{
		int in = i;
		i = KLLabel_DrawChar (
								   p, p64, len,
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

// 色変更タグの開始文字と終了文字を設定する
//
// 全角など、1 byte であらわせない文字があるため、charではなく、
// charのポインタで渡すことになっているが、これは文字列を渡せるわけではない
// 内部では u32 化したマルチバイト文字 1 文字分しかバッファを持たないので、
// 2 文字以上、文字列は設定せず"文字"(全角でもOK)を指定する事
// ラベルごとに異なる指定もできるが、まとめて変更したい場合は
// KLConfig.h 内の KLLABEL_TAG_COL_START と、 KLLABEL_TAG_COL_END を
// 変更することをおすすめする
void KLLabel_SetTagChar( KLLabel* p, char* pStartTag, char* pEndTag) {

	chartou32(pStartTag, &p->taginfo.colorStartChr );
	chartou32(pStartTag, &p->taginfo.colorEndChr );
	
}

u64* KLLabel_Parse( KLLabel* p, u64* p64 ) {

	size_t	len		= strlen64 (p64);
	
	//KLLabel_ReallocParseBuffer(len);
	
	if ( !p->pStr64WithoutAttr || p->lenWithoutAttr < len )
	{
		
		p->pStr64WithoutAttr = (u64*) realloc ( p->pStr64WithoutAttr, sizeof(u64)*(len+1) );
		memset( p->pStr64WithoutAttr, 0, sizeof(u64)*(len+1) );
		
	}
	
	p->lenWithoutAttr = 0;
	
	for ( u32 i=0; i<len;   ) {
		
		// 色変更指定
		//===============================================================
		// 色変更は "[FF9966]" のような 8byte 固定文字列で指定する
		// i+7 が len を超えていると解析しないようにする
		if ( i+7 < len  &&  p64[i]==p->taginfo.colorStartChr  &&  p64[i+7]==p->taginfo.colorEndChr  ) {
		
			char colstr[7] = {	(char)(p64[i+1]), (char)(p64[i+2]),
								(char)(p64[i+3]), (char)(p64[i+4]),
								(char)(p64[i+5]), (char)(p64[i+6]), 0
			};
			
			u32 col = (u32) KLString_ParseInt( colstr, TRUE );
			
			KLLabel_AddInfo( p, KLLabel_DRAWINFO_COLOR, p->lenWithoutAttr, 0, RGBA( col>>16 & 0xFF, col>>8 & 0xFF, col & 0xFF, 0xFF ) );
			
			// タグは文字列のみのバッファに詰めないで次に読むインデクスだけ進める
			i += 8;
			
		}
		else
		{
			
			// 文字列のみのバッファに詰める
			p->pStr64WithoutAttr[p->lenWithoutAttr++] = p64[i++];
			
		}
		
	}
	
	return p->pStr64WithoutAttr;
	
}

static s32 KLLabel_Draw64( KLLabel* p, u64* p64, size_t len, float x, float y, float maxw, Rgba rgba, KLLabelAlign align_x_type ){
	
	float		totalw		= 0;
	float		rate		= p->fontsize / p->originsize;
	s32			wordStart   = 0;
    s32			wordLen     = 0;
    u32			wordTotalW  = 0;
	
	if ( !p->pMap64 )
	{
		return y;
	}
	
	if ( !p->escapeTag )
	{
		
		// 描画するごとにタグ解析情報をリセットする
		KLLabel_ResetParseBuffer(p);
		
		// 新たにタグを解析
		KLLabel_Parse( p, p64 );

	}
	
	KLLabel_lastIsNeedWrap	= FALSE;
	last_y					= 0;
	
	if( maxw==0 )
	{
		maxw = 0xFFFFF;
	}
	
    for( int i=0;  i>=0 && i<len;  ++i )
	{
		int in = i;
		i = KLLabel_DrawChar (
				p,
				p->escapeTag? p64:p->pStr64WithoutAttr,
				len,
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

s32 KLLabel_Draw( KLLabel* p, char* pChar, size_t len, float x, float y, float maxw, Rgba rgba, KLLabelAlign align_x_type ){
	
	return KLLabel_Draw64( p, UTF8_toU64all(pChar, NULL, NULL), len, x, y, maxw, rgba, align_x_type );
	
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
		KLLabel_UnloadParseBuffer(p);
		sfree(p->pAdv);
		sfree(p->pBBox);
		sfree(p->pMap64);
		sfree(p->pUv);
		sfree(p->pColList);
		sfree(p->pStr64WithoutAttr);
		
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
	
	fw = frect.size.width;
	fw += -grect.origin.x * p->fontsize;
	
	return (u32)fw;
	
}
