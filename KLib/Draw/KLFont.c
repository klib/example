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

#include "KLib.h"
#include "KLFont.h"
#include "KLString.h"
#include <string.h>

/// システムフォント名
const char* KLFont_name[KLFontTypeMax] = {
	
	"AmericanTypewriter-Bold",
	"AppleGothic",
	"AppleColorEmoji",
	"ArialMT",
	"Arial-BoldMT",
	"Arial-ItalicMT",
	"Arial-BoldItalicMT",
	"ArialRoundedMTBold",
	"ArialUnicodeMS",
	"Courier",
	"Courier-Bold",
	"Courier-Oblique",
	"Courier-BoldOblique",
	"CourierNewPSMT",
	"CourierNewPS-BoldMT",
	"CourierNewPS-ItalicMT",
	"CourierNewPS-BoldItalicMT",
	"DBLCDTempBlack",
	"Georgia",
	"Georgia-Bold",
	"Georgia-Italic",
	"Georgia-BoldItalic",
	"Helvetica",
	"Helvetica-Bold",
	"Helvetica-Oblique",
	"Helvetica-BoldOblique",
	"HelveticaNeue",
	"HelveticaNeue-Bold",
	"HiraKakuProN-W3",
	"HiraKakuProN-W6",
	"MarkerFelt-Thin",
	"STHeitiJ-Light",
	"STHeitiJ-Medium",
	"STHeitiK-Light",
	"STHeitiK-Medium",
	"STHeitiSC-Light",
	"STHeitiSC-Medium",
	"STHeitiTC-Light",
	"STHeitiTC-Medium",
	"TimesNewRomanPSMT",
	"TimesNewRomanPS-BoldMT",
	"TimesNewRomanPS-ItalicMT",
	"TimesNewRomanPS-BoldItalicMT",
	"TrebuchetMS",
	"TrebuchetMS-Bold",
	"TrebuchetMS-Italic",
	"Verdana",
	"Verdana-Bold",
	"Verdana-Italic",
	"Verdana-BoldItalic",
	"Zapfino",
	"LockClock-Light",
	"PhonepadTwo",

};

//=========================================================
/**
 *	@brief	フォントリファレンスの取得
 *
 *	@param	pFont			KLfontのポインタ
 */
//=========================================================
inline CGFontRef KLFont_GetFontRef( KLFont* pFont ){

	return pFont->ref;
	
}

//=========================================================
/**
 *	@brief	フォントの読み込みとフォントオブジェクトの初期化
 *
 *	@param	pFont			フォントオブジェクトのポインタ
 *	@param	pName			読み込むフォントファイル名
 *	@param	isCustomFont	カスタムフォントならTRUE
 */
//=========================================================
bl KLFont_Load( KLFont* pFont, const path8 pName, const bl isCustomFont ){
	
	if( pFont && pName )
	{
		CGRect				bbox;
		CFStringRef			cfs;
		size_t				len = strlen(pName);
		char				temp[256];
		
		klmemclear(pFont, KLFont);
		
		sfree( pFont->pName );
		sCGFontRelease( pFont->ref );
		memset(pFont, 0, sizeof(KLFont));
		
		pFont->pName = (char*)Calloc( len+1 );
		strcpy( pFont->pName, pName );
		
		if( isCustomFont )
		{
			CGDataProviderRef	fontDataProvider;		
			
			if( isCustomFont )
			{
				sprintf( temp, "%s/%s", klib.path.appRoot, pName );
			}
			else
			{
				sprintf( temp, "%s/%s", "/System/Library/Fonts", pName );
			}
			
			cfs = char2cfs( temp, &len );
			if( len<=0 )
			{
				KLLog("[ KLFont ] Font name cfs alloc error.\n");
				KLFont_Unload(pFont);
				return FALSE;
			}
			
			if( !cfs2char( &cfs, temp, len ) )
			{
				KLLog("[ KLFont ] Font name cfconvert error.\n");
				sCFRelease(cfs);
				KLFont_Unload(pFont);
				return FALSE;
			}
			
			fontDataProvider	= CGDataProviderCreateWithFilename( temp );
			pFont->ref			= CGFontCreateWithDataProvider(fontDataProvider);
			CGDataProviderRelease( fontDataProvider );
			
		}
		else
		{
			
			cfs = char2cfs( pName, &len );
			if( len<=0 )
			{
				KLLog("[ KLFont ] Font name cfs alloc error.\n");
				KLFont_Unload(pFont);
				return FALSE;
			}
			pFont->ref = CGFontCreateWithFontName( cfs );
			
		}
		
		bbox = CGFontGetFontBBox( pFont->ref );
		
		pFont->wUnit		= bbox.size.width;
		pFont->unitsPerEm	= CGFontGetUnitsPerEm( pFont->ref );
		pFont->ascent		= CGFontGetAscent( pFont->ref );
		pFont->descent		= CGFontGetDescent( pFont->ref );
		pFont->leading		= CGFontGetLeading( pFont->ref );
		pFont->bbox			= CGFontGetFontBBox(pFont->ref);
		
		
		KLLog("as:%d\tds:%d\tnum:%zu\n", pFont->ascent, pFont->descent, CGFontGetNumberOfGlyphs(pFont->ref) );
		KLLog("x:%.02f\ny:%.02f\nw:%.02f\nh:%.02f\n", pFont->bbox.origin.x, pFont->bbox.origin.y, pFont->bbox.size.width, pFont->bbox.size.height );
		return TRUE;
		
	}
	else
	{
		KLLog("[ KLFont ] Init parameter error.\n");
	}
	
	return FALSE;
	
}

//=========================================================
/**
 *	@brief	指定フォントオブジェクトの1文字あたりの最大高を取得
 *
 *	@param	pFont		フォントオブジェクトのポインタ
 *	@param	fontSize	フォントサイズ(ピクセル)
 */
//=========================================================
float KLFont_GetHeight(KLFont* pFont, float fontSize){

	return fontSize * (pFont->ascent-pFont->descent) / (CGFloat) pFont->unitsPerEm;

}

//=========================================================
/**
 *	@brief	指定フォントオブジェクトの1文字あたりのアセンダ高を取得
 *
 *	@param	pFont		フォントオブジェクトのポインタ
 *	@param	fontSize	フォントサイズ(ピクセル)
 *
 *	@return	アセンダ
 */
//=========================================================
float KLFont_GetAscent(KLFont* pFont, float fontSize){
	
	// なぜ2048かについてはまだ詳しく調べてません…
	// 今のところつじつまが合う数値がこの計算でした
	float rate = 2048.0f / pFont->unitsPerEm;
	return	pFont->ascent * fontSize / pFont->unitsPerEm * rate + 0.5f;

}

//=========================================================
/**
 *	@brief	指定フォントオブジェクトの1文字あたりのディセンダ高を取得
 *
 *	@param	pFont		フォントオブジェクトのポインタ
 *	@param	fontSize	フォントサイズ(ピクセル)
 *
 *	@return	ディセンダ
 */
//=========================================================
float KLFont_GetDescent(KLFont* pFont, float fontSize){

#if 1
	float rate = 1.0f;
	
	if( pFont->ascent-pFont->descent==pFont->unitsPerEm )
	{
		rate = (pFont->bbox.size.height-pFont->bbox.origin.y) / (float)pFont->unitsPerEm;//2048.0f / pFont->unitsPerEm;
	}
	
	return -pFont->descent * fontSize / pFont->unitsPerEm * rate + 0.5f;
#else

	// ヒラギノだけ例外あり? 数値があってないしずれる原因が特殊な気がする
	float d = (-pFont->descent / (float)pFont->unitsPerEm) * fontSize;
	return d+0.5f;

#endif
}

//=========================================================
/**
 *	@brief	指定フォントオブジェクトの1文字あたりのリーディング高を取得
 *
 *	@param	pFont		フォントオブジェクトのポインタ
 *	@param	fontSize	フォントサイズ(ピクセル)
 *
 *	@return	リーディング高
 */
//=========================================================
u32 KLFont_GetLeading(KLFont* pFont, float fontSize){
	
	float rate = 1.0f;
	
	if( pFont->ascent-pFont->descent==pFont->unitsPerEm )
	{
		rate = (pFont->bbox.size.height-pFont->bbox.origin.y) / (float)pFont->unitsPerEm;
	}
	
	return pFont->leading * fontSize / pFont->unitsPerEm * rate + 0.5f;
	
}

//=========================================================
/**
 *	@brief	フォントのアンロード
 *
 *	@param	pFont	フォントオブジェクトのポインタ
 */
//=========================================================
inline void KLFont_Unload(KLFont* pFont){
	
	sfree(pFont->pName);
	sCGFontRelease(pFont->ref);

}

// 下記コードのライセンスについて
// CoreTextを使用してグリフを取得するまでは下記コードを使用していました。
// このコードは下記URLを参考させて頂きましたが、
// ライセンスについての明記が特に見受けられなかったため
// 作者の方針次第で該当コードが使用不可となる可能性があります。
// License issue
// Thanks for:http://www.mexircus.com/codes/GlyphDrawing.mm
// however the license is unknown.
// so I might be to change code.

// ===========================================================================
//
// CMap -- struct + functions.
// A cmap is a part of each truetype font that contains the 
// TADAA!!: character map. (which is what we want)
//


/** 
 Only save a ref to the cmap fonttable and the segment offset
 of segment with the relevant platform
 When we build a cmap we decide which segment to use.
 */
 
typedef struct CMap {
	
	CFDataRef	fontTable;
	u32			segmentOffset;
	s32			format; // 4 or 12 only support right now.
	
}CMap;


// We prefer platform 0
typedef struct PlatformPriority {
	int plat;
	int prio;
}PlatformPriority;

u16 getUInt16WithByteIndex(CFDataRef data, CFIndex index) {
    u16 value = 0;
    CFDataGetBytes(data, CFRangeMake(index, 2), (UInt8 *)&value);
    return CFSwapInt16BigToHost(value);
}

u32 getUInt32WithByteIndex(CFDataRef data, CFIndex index) {
    u32 value = 0;
    CFDataGetBytes(data, CFRangeMake(index, 4), (UInt8 *)&value);
    return CFSwapInt32BigToHost(value);
}

u16 getUInt16(CFDataRef data, CFIndex index) {
    u16 value = 0;
    CFDataGetBytes(data, CFRangeMake(index * 2, 2), (UInt8 *)&value);
    return CFSwapInt16BigToHost(value);
}

// UNICODE helpers

#define kUnicodeHighSurrogateStart 0xD800
#define kUnicodeHighSurrogateEnd 0xDBFF
#define kUnicodeLowSurrogateStart 0xDC00
#define kUnicodeLowSurrogateEnd 0xDFFF

inline bl UNI_IsHighSurrogate(wchar c);
inline bl UNI_IsLowSurrogate(wchar c);
inline u32 UNI_ConvertSurrogatePairToUTF32(wchar high, wchar low);

bl UNI_IsHighSurrogate(wchar c){
	
	return		( c >= kUnicodeHighSurrogateStart ) &&
			( c <= kUnicodeHighSurrogateEnd   );
		 
}

bl UNI_IsLowSurrogate(wchar c){

	return		( c >= kUnicodeLowSurrogateStart ) &&
			( c <= kUnicodeLowSurrogateEnd	 );
			
}

u32 UNI_ConvertSurrogatePairToUTF32(wchar high, wchar low){

	return ( (high - 0xD800) * 0x400 + (low - 0xDC00) + 0x10000 );
	
}

// Create a cmap struction (retains font table)
CMap * CMapCreate(CFDataRef fontTable, u32 segmentOffset, int format) 
{
	
	CMap * cmap			= (CMap *)calloc(1, sizeof(CMap));
	cmap->segmentOffset = segmentOffset;
	CFRetain(fontTable);
	cmap->fontTable		= fontTable;
	cmap->format		= format;
	
	return cmap;
	
}

// Delete cmap struction (relases the font table)
void CMapRelease(CMap * cmap) 
{
	CFRelease(cmap->fontTable);
	free(cmap);
}

/** CMap cache -- We have a global cache of all the fonts we have encountered so far.
 The cache specifically caches the plaform decision we've made in the form of the segment offset.
 */
static CFMutableDictionaryRef cmapCache = NULL; 

// Decide the contents of the cmap (platform + format)
CMap * CMGetCMapForFont(CGFontRef cgFont) 
{
	// Create dictionary to cache cmaps
	if (cmapCache == NULL) {
	
		CFDictionaryValueCallBacks nonRetainingDictionaryValueCallbacks = kCFTypeDictionaryValueCallBacks;
		nonRetainingDictionaryValueCallbacks.retain = NULL;
		nonRetainingDictionaryValueCallbacks.release = NULL;
		cmapCache = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &nonRetainingDictionaryValueCallbacks);
	
	}
	
	// extract font name to insert the font into the cache.
	CFStringRef fontName = CGFontCopyFullName(cgFont);
	// Check dictionary to see if a cmap exists for given font name
	CMap * cmap = (CMap *)CFDictionaryGetValue(cmapCache, fontName);
	if (cmap != NULL) {
		CFRelease(fontName);
		return cmap;
	}
	
	CFDataRef fontTable = CGFontCopyTableForTag(cgFont, 'cmap');
	if (fontTable != NULL) {
		s32 bestPriority	= 2000000000; // Smaller is better so start with big...
		//u16 version			= getUInt16WithByteIndex(fontTable, 0);
		getUInt16WithByteIndex(fontTable, 0);
		u16 subtableCount	= getUInt16WithByteIndex(fontTable, 2);
		u32 segmentOffset	= 0;
		u16 bestPlatformID	= 0;
		
		PlatformPriority const platforms[] = {
			{0, 0}, // best
			{3, 1}, // second best
		};
		
		// each platform is given a priority - Lower is better
		
		// anything else is ignored 
		// -- The iPhone fonts typically also have platform 1, but we ignore them for now
		
		// Iterate through all subtables to find the best platform
		for (u16 subtableIndex = 0; subtableIndex < subtableCount; ++subtableIndex) {
			u16 platformID = getUInt16WithByteIndex(fontTable, subtableIndex * 8 + 4);
			
			// look for this platformID in our list. Are we interested in it at all?
			for (s32 p = 0; p < sizeof(platforms)/sizeof(PlatformPriority); ++p) {
				if (platformID == platforms[p].plat && bestPriority > platforms[p].prio) {
					segmentOffset = getUInt32WithByteIndex(fontTable, subtableIndex * 8 + 8);;
					bestPriority = platforms[p].prio;
					bestPlatformID = platformID;
				}
			}
		}
		
		// Only support Unicode... (platform 0 and 3) and format 4 and 12
		if (bestPlatformID == 0 || bestPlatformID == 3) {
			u16 format = getUInt16WithByteIndex(fontTable, segmentOffset);
			if (format == 4 || format == 12) {
				cmap = CMapCreate(fontTable, segmentOffset, format);
				
				// Assign cmap to font name and cache it in the dictionary
				CFDictionaryAddValue(cmapCache, fontName, cmap);
				CFRelease(fontName);
				CFRelease(fontTable);
				return cmap;
			}
		}
		
		// not supported -- Just leave
		CFRelease(fontTable);
	}
	
	CFRelease(fontName);	
    return NULL;
}


size_t
CMap4GetGlyphIndicesForUnichar(CMap * cmap, wchar const * s, CGGlyph * result, size_t count) 
{
	// Unlike other implementations, I don't cache all this stuff. It is relatively easy
	// to extract, and we use plenty of memory on the iPhone as it is.
	u32 segmentOffset = 	cmap->segmentOffset;
	u16 segCountX2 =  getUInt16WithByteIndex(cmap->fontTable, segmentOffset + 6);
	u16 segCount = segCountX2 / 2;
	
	u8*		ptr					= (u8*)CFDataGetBytePtr(cmap->fontTable);
	u32		endCodeOffset		= segmentOffset + 14;
	u16*	endCode				= (u16*)&ptr[endCodeOffset];
	u32		startCodeOffset		= endCodeOffset + 2 + segCountX2;
	u16*	startCode			= (u16*)&ptr[startCodeOffset];
	u32		idDeltaCodeOffset	= startCodeOffset + segCountX2;
	u16*	idDelta				= (u16*)&ptr[idDeltaCodeOffset];;
	u32		idRangeOffsetOffset = idDeltaCodeOffset + segCountX2;
	u16*	idRangeOffset		= (u16*)&ptr[idRangeOffsetOffset];
	u32		glyphIndexArrayOffset= idRangeOffsetOffset + segCountX2;
	u16*	glyphIndexArray		= (u16*)&ptr[glyphIndexArrayOffset];
	
	
	for (size_t j = 0; j < count; ++j) { 
		wchar c = s[j];
		bl found = FALSE;
		for (int i = 0; i < segCount; i++) {
			// Find first endcode greater or equal to the char code
			u16 end = CFSwapInt16BigToHost(endCode[i]);
			u16 start = CFSwapInt16BigToHost(startCode[i]);
			if (end >= c && start <= c) {
				u16 delta = CFSwapInt16BigToHost(idDelta[i]);
				u16 rangeOffset = CFSwapInt16BigToHost(idRangeOffset[i]);
				if(rangeOffset == 0) {
					result[j] = delta + c;
					found = TRUE;
					break;
				} else {
					result[j] = CFSwapInt16BigToHost(glyphIndexArray[(rangeOffset >> 1) + (c - start) - (segCount - i)]);
					found = TRUE;
					break;
				}
			}
		}
		if (!found) {
			// failed. Set the glyph to zero.
			result[j] = 0;
		}
	}
	return count;
}

size_t
CMap12GetGlyphIndicesForUnichar(CMap * cmap, wchar const * s, CGGlyph * result, size_t count) 
{
	u32 nGroups = getUInt32WithByteIndex(cmap->fontTable, cmap->segmentOffset + 12);
	size_t glyphOutputCount = 0;
	for (size_t j = 0; j < count; ++j) {
		// Format of groups are:
		// UInt32 startCode
		// UInt32 endCode
		// startGlyph
		bl found = FALSE;
		
		// figure out what the 32-bit version of this is:
		wchar c = s[j];
		u32 c32 = c;
		if (UNI_IsHighSurrogate(c)) {
			if (j+1 < count) { 
				wchar cc = s[j+1];
				if (UNI_IsLowSurrogate(cc)) {
					c32 = UNI_ConvertSurrogatePairToUTF32(c, cc);
					j++;
				}
			}
		}
		int offset = cmap->segmentOffset + 16;
		for (u32 g = 0; g < nGroups; ++g) {
			u32 start  = getUInt32WithByteIndex(cmap->fontTable, offset);
			u32 end  = getUInt32WithByteIndex(cmap->fontTable, offset + 4);
			
			if (c32 <= end && c32 >= start) {
				u32 startGlyph  = getUInt32WithByteIndex(cmap->fontTable, offset + 8);
				result[glyphOutputCount++] = startGlyph + c32 - start;
				found = TRUE;
				break;
			}
			offset += 12;
		}
		if (!found) {
			// add blank
			result[glyphOutputCount++] = 0;
		}
	}
	
	return glyphOutputCount;
}


// ===========================================================================
//
// Main functions:
// The lock will keep the cache thread safe. You'll need some place to set 
// it up though
static bl lock = 0;

size_t KLFont_GetGlyphsForWchars(CGFontRef cgFont, const wchar* buffer, CGGlyph* glyphs, size_t numGlyphs)
{
	size_t res = 0;
	// only act on the lock if we have one
	if(!lock){
		
		lock = TRUE;
		
		CMap * cmap = CMGetCMapForFont(cgFont);
		if(cmap){
			
			if (cmap->format == 4) {
				res = CMap4GetGlyphIndicesForUnichar(cmap, buffer, glyphs, numGlyphs);
			} else if(cmap->format == 12) {
				res = CMap12GetGlyphIndicesForUnichar(cmap, buffer, glyphs, numGlyphs);
			} else {
				// just fill in blanks
				for (int i = 0; i < numGlyphs; ++i) {
					glyphs[i] = 0;
				}
				res = numGlyphs;
			}
		}
		lock = FALSE;

	}
	return res;
}
