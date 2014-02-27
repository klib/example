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

#ifndef	___KL_FONT_H___
#define	___KL_FONT_H___

#include "c_extern.h"
	
	// I N C L U D E
	//=============================================================
	#include "KLibDefine.h"
	#include "KLSprite.h"
	#include <CoreGraphics/CGContext.h>
	#include <CoreFoundation/CFString.h>

	/// 1emを何ピクセルとしてみなすか
	#define KLFONT_PIXEL_OF_1EM (16.0f)

	/// システムフォント
	typedef enum{
		
		KLFontType_AmericanTypewriter_Bold,
		KLFontType_AppleGothic,
		KLFontType_AppleColorEmoji,
		KLFontType_ArialMT,
		KLFontType_Arial_BoldMT,
		KLFontType_Arial_ItalicMT,
		KLFontType_Arial_BoldItalicMT,
		KLFontType_ArialRoundedMTBold,
		KLFontType_ArialUnicodeMS,
		KLFontType_Courier,
		KLFontType_Courier_Bold,
		KLFontType_Courier_Oblique,
		KLFontType_Courier_BoldOblique,
		KLFontType_CourierNewPSMT,
		KLFontType_CourierNewPS_BoldMT,
		KLFontType_CourierNewPS_ItalicMT,
		KLFontType_CourierNewPS_BoldItalicMT,
		KLFontType_DBLCDTempBlack,
		KLFontType_Georgia,
		KLFontType_Georgia_Bold,
		KLFontType_Georgia_Italic,
		KLFontType_Georgia_BoldItalic,
		KLFontType_Helvetica,
		KLFontType_Helvetica_Bold,
		KLFontType_Helvetica_Oblique,
		KLFontType_Helvetica_BoldOblique,
		KLFontType_HelveticaNeue,
		KLFontType_HelveticaNeue_Bold,
		KLFontType_HiraKakuProN_W3,
		KLFontType_HiraKakuProN_W6,
		KLFontType_MarkerFelt_Thin,
		KLFontType_STHeitiJ_Light,
		KLFontType_STHeitiJ_Medium,
		KLFontType_STHeitiK_Light,
		KLFontType_STHeitiK_Medium,
		KLFontType_STHeitiSC_Light,
		KLFontType_STHeitiSC_Medium,
		KLFontType_STHeitiTC_Light,
		KLFontType_STHeitiTC_Medium,
		KLFontType_TimesNewRomanPSMT,
		KLFontType_TimesNewRomanPS_BoldMT,
		KLFontType_TimesNewRomanPS_ItalicMT,
		KLFontType_TimesNewRomanPS_BoldItalicMT,
		KLFontType_TrebuchetMS,
		KLFontType_TrebuchetMS_Bold,
		KLFontType_TrebuchetMS_Italic,
		KLFontType_Verdana,
		KLFontType_Verdana_Bold,
		KLFontType_Verdana_Italic,
		KLFontType_Verdana_BoldItalic,
		KLFontType_Zapfino,
		KLFontType_LockClock_Light,
		KLFontType_PhonepadTwo,
		KLFontTypeMax,
		
	}KLFontType;

	extern const char* KLFont_name[KLFontTypeMax];

	/// KLFont
	typedef struct{
		
		CGFontRef	ref;			///< フォントオブジェクト
		char*		pName;			///< フォント名
		int			unitsPerEm;		///< UnitsPerEm
		int			ascent;			///< アセンダ
		int			descent;		///< ディセンダ
		int			leading;		///< リーディング
		float		wUnit;			///< ユニット幅
		CGRect		bbox;			///< BBox
		bl			isCustom:1;		///< カスタムフォント
		
	}KLFont;
	
	CGFontRef	KLFont_GetFontRef( KLFont* pFont );
	bl			KLFont_Load( KLFont* pFont, const path8 pName, const bl isCustomFont );
	void		KLFont_Unload(KLFont* pFont);
	size_t		KLFont_GetGlyphsForWchars(CGFontRef cgFont, const wchar* buffer, CGGlyph* glyphs, size_t numGlyphs);
	//float		KLFont_GetWidth(KLFont* pFont, float fontSize);
	float		KLFont_GetHeight(KLFont* pFont, float fontSize);
	float		KLFont_GetAscent(KLFont* pFont, float fontSize);
	float		KLFont_GetDescent(KLFont* pFont, float fontSize);
	u32			KLFont_GetLeading(KLFont* pFont, float fontSize);

	CGFontRef KLFont_CreateCGFontRef ( const path8 pPath, bl isCustomFont );
	path8 KLFont_CreatePath ( const path8 pName, bl isCustomFont, char* ppOutPath );

#include "c_extern_close.h"
#endif