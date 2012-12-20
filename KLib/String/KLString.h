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

#ifndef ___KL_STRING_H___
#define ___KL_STRING_H___

#include "c_extern.h"

#include "KLibDefine.h"
#include <string.h>
#include <stdlib.h>
#include <CoreFoundation/CFString.h>
#include <CoreGraphics/CGContext.h>
#include <CoreGraphics/CGFont.h>
#include <CoreGraphics/CGBitmapContext.h>
	
// D E F I N E
//==============================================================
	
	/// デフォルトエンコーディング
	#define	KLSTRING_DEFAULT_ENCODE	kCFStringEncodingUTF8
	
	/// デフォルトの最大文字列バッファ長
	#define	KL_DEFAULT_TXT_LEN	(256);
	
// M A C R O
//==============================================================
	
	//=========================================================================
	/**
	 *	@brief	マルチバイト文字列長を返す
	 */
	//=========================================================================
	#define strlencf CFStringGetLength
	
	//=========================================================================
	/**
	 *	@brief	wchar文字列長を返す
	 */
	//=========================================================================	
	#define wstrlen KLString_GetLen

	//=========================================================================
	/**
	 *	@brief	u32文字列長を返す
	 */
	//=========================================================================
	#define strlen32 KLString_GetLen32

	//=========================================================================
	/**
	 *	@brief	wcharでstrcmp
	 */
	//=========================================================================	
	#define wstrcmp KLString_Comp
	
	#define wlowercase KLString_Lowercase
	#define wuppercase KLString_Uppercase
	
	typedef struct{
		
		size_t	offset;
		size_t	len;
		wchar*	pWchar;
		
	}KLString;
	
	CFStringRef	char2cfs( const char*, size_t* );
	char*		cfs2char( CFStringRef*, char*, CFIndex );
	wchar*		char2wchar( const char*, size_t*, wchar*, size_t );
	char*		u32tochar( u32* pStr32, char* pStr, u32 len32 );
	u32*		chartou32( char* pStr, u32* pStr32 );
	wchar*		KLString_Alloc( size_t, wchar* );
	void		KLString_TrimSameChars( wchar*, size_t* );
	void		KLString_TrimSameChars32( u32* pStr32, size_t* pMaxLength );
	void		KLString_TrimEmoji32( u32* pStr32, size_t* pMaxLength );
	size_t 		KLString_GetSplitArrayNum( wchar*, size_t, wchar );
	KLString*	KLString_SplitToPointers( wchar*, size_t, wchar, size_t* );
	wchar**		KLString_SplitToUnichars( wchar*, size_t, wchar, size_t* );
	wchar**		KLString_SplitToUnicharPointers( wchar*, size_t, wchar, size_t* );
	wchar*		KLString_TrimL( wchar*, wchar* );
	size_t		KLString_GetLen( wchar* );
	size_t		KLString_GetLen32( u32* );
	s64			KLString_ParseInt( const wchar* );
	s64			parseInt( const char* pChar );
	s32			KLString_Comp(wchar*,wchar*,bl);
	u32			KLString_GetWordLen( wchar* pUnichars, u32 idx, u32 len );
	s32			KLString_GetActiveWordIndex( wchar* pUnichars, u32 offset, u32 len );
	wchar		KLString_Lowercase( wchar );
	wchar		KLString_Uppercase( wchar );
	u32			KLString_GetSpaceLen( wchar* pUnichars, u32 idx, u32 len, bl onlyHankaku );
	bl			KLString_IsEmoji32( u32 c );
	void		KLString_UnloadSystem();
	u32			cstrlen( const char* s );
	u32			ucharlen( const char* s );

#include "c_extern_close.h"
#endif
