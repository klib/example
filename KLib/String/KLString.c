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

#include "KLString.h"
#include <wchar.h>

/// 文字列用リサイクルバッファサイズ
static size_t	kl_temp_size	= 0;

/// 文字列用リサイクルバッファ
static char*	kl_temp			= NULL;


/// ワイド文字列用リサイクルバッファサイズ
static size_t	kl_tempk_size	= 0;

/// ワイド文字列用リサイクルバッファ
static wchar*	kl_tempk		= NULL;


/// u32文字列用リサイクルバッファサイズ
static size_t	kl_temp32_size	= 0;

/// u32文字列用リサイクルバッファ
static u32*	kl_temp32		= NULL;

void KLString_UnloadSystem(){
	
	sfree(kl_temp);
	sfree(kl_tempk);
	sfree(kl_temp32);
	
	kl_temp32_size	= 0;
	kl_tempk_size	= 0;
	kl_temp_size	= 0;
	
}

//===============================================================
/**
 *	@brief	ワイド文字列の作成
 *
 *	@param	len			確保する文字数(内部でこの数値+１文字分で確保されます)
 *	@param	pFirstValue	セットする文字列 / 確保だけでいいならNULL
 *
 *	@return	確保したkchar文字列のポインタ / 失敗時はNULL
 */
//===============================================================
wchar* KLString_Alloc( size_t len, wchar* pFirstValue ){
	
	wchar* p = (wchar*)Malloc( sizeof(wchar)*len );
	if( !p )
	{
		KLLog( "[ KLString ] Allocation failed." );
		return NULL;
	}
	
	memset( p, 0, sizeof(wchar)*len );
	
	if( pFirstValue )
	{
		memcpy( p, pFirstValue, sizeof(wchar)*len );
	}
	
	return p;
	
}


//=========================================================================
/**
 *	@brief	マルチバイトcharの文字列をCFStringに変換
 */
//=========================================================================
CFStringRef char2cfs( const char* pCstring, size_t* pLen ){
	
	CFStringRef	cfs;
	size_t		len	= 0;
	
	cfs = CFStringCreateWithCString( NULL, pCstring, KLSTRING_DEFAULT_ENCODE );
	
	if( cfs )
	{
		len = strlencf( cfs );
	}
	
	if( pLen )
	{
		*pLen = len;
	}
	
	return cfs;
	
}

//=========================================================================
/**
 *	@brief	CFStringをマルチバイトchar文字列に変換
 */
//=========================================================================
char* cfs2char( CFStringRef* pCfs, char* pCstring, CFIndex cflen ){
	
	CFIndex	max = CFStringGetMaximumSizeForEncoding( cflen, KLSTRING_DEFAULT_ENCODE );
	bl		ok	= CFStringGetCString( *pCfs, pCstring, max+1, KLSTRING_DEFAULT_ENCODE );
	
	return ok ? pCstring : NULL;
	
}

//=========================================================================
/**
 *	@brief	1文字辺りu32に拡張した文字列を可変長unicode文字列に戻す
 *
 *	@param	pStr32	u32文字列のポインタ
 *	@param	pStr	変換先のchar文字列のポインタ
 *	@param	len32	u32文字列の文字数
 */
//=========================================================================
char* u32tochar( u32* pStr32, char* pStr, u32 len32 ){
	
	u32 c = 0;
	
	if( !pStr )
	{
		if( kl_temp_size<len32*4+4 )
		{
			kl_temp_size = len32*4+4;
			kl_temp = (char*)realloc( kl_temp, kl_temp_size );
		}
		
		pStr = kl_temp;
	}
		
	for( u32 i=0; i<len32; ++i )
	{
		
		if( pStr32[i]&0xFF000000 )
		{
			pStr[c++] = (pStr32[i]    )&0xFF;
			pStr[c++] = (pStr32[i]>>8 )&0xFF;
			pStr[c++] = (pStr32[i]>>16)&0xFF;
			pStr[c++] = (pStr32[i]>>24)&0xFF;
		}
		else if( pStr32[i]&0x00FF0000 )
		{
			pStr[c++] = (pStr32[i]    )&0xFF;
			pStr[c++] = (pStr32[i]>>8 )&0xFF;
			pStr[c++] = (pStr32[i]>>16)&0xFF;
		}
		else if( pStr32[i]&0x0000FF00 )
		{
			pStr[c++] = (pStr32[i]    )&0xFF;
			pStr[c++] = (pStr32[i]>>8 )&0xFF;
		}
		else
		{
			pStr[c++] = (pStr32[i]    )&0xFF;
		}
		
	}
	
	pStr[c] = 0;
	
	return pStr;
	
}

//=========================================================================
/**
 *	@brief	1文字辺りu32に拡張した文字列に変換する
 *
 *	@param	pStr	char文字列のポインタ
 *	@param	pStr32	変換先のu32文字列のポインタ<br/>
 *					NULLの場合、内部で一時バッファをreallocする。
 *
 *	@return 変換後のu32文字列ポインタ
 *
 *	@warning	pStr32をNULLにした場合、内部で一時バッファを確保するが<br/>
 *				保持しておきたい場合は、戻り先でコピーしないと<br/>
 *				次にこの関数をコールした時や、他の処理で値が変更される可能性がある
 */
//=========================================================================
u32* chartou32( char* pStr, u32* pStr32 ){
	
	unsigned char* pStr2 = (unsigned char*)pStr;
	u8	c		= 0;
	u32 i32		= 0;
	u32 szStr	= strlen(pStr);
	
	if( !pStr32 )
	{
		if( kl_temp32_size<szStr+1 )
		{
			kl_temp32_size = szStr+1;
			kl_temp32 = (u32*)realloc( kl_temp32, sizeof(u32)*(szStr+1) );
			KLLog("[ chartou32 ] realloc %lu bytes.\n", sizeof(u32)*(szStr+1));
		}
		pStr32 = kl_temp32;
	}
	
	for( u32 i=0; i<szStr; )
	{
		c = ucharlen(&pStr[i]);
		
		switch( c )
		{
			case 0:
			default:
			{
				KLLog("[ KLString ] invalid string.\n");
			}
			return NULL;
				
			case 1:
			{
				pStr32[i32] = pStr[i];
			}
			break;
				
			case 2:
			{
				pStr32[i32] = pStr2[i] + pStr2[i+1]*0x100;
			}
			break;
				
			case 3:
			{
				pStr32[i32] = pStr2[i] + pStr2[i+1]*0x100 + pStr2[i+2]*0x10000;
			}
			break;
				
			case 4:
			{
				pStr32[i32] = pStr2[i] + pStr2[i+1]*0x100 + pStr2[i+2]*0x10000 + pStr2[i+3]*0x1000000;
			}
			break;
				
		}
		
		++i32;
		i += c;
		
	}
	
	pStr32[i32] = 0;
	
	return pStr32;
}

//=========================================================================
/**
 *	@brief	マルチバイトchar文字列をkcode文字列に変換
 *
 *	@param	pCstring	マルチバイト文字列
 *	@param	pLen		長さを返す先
 *	@param	pDest		kcharコピー先（NULL指定可、その場合システムの一時バッファを使用し戻り値で返す）
 *	@param	destLen		コピー先の長さ
 *
 *	@return	kchar文字列のポインタ
 *
 *	@warning	内部でtemp16を使用します
 */
//=========================================================================
wchar* char2wchar( const char* pCstring, size_t* pLen, wchar* pDest, size_t destLen ){
	
	size_t		len		= 0, dlen = 0;
	CFStringRef	cfs		= char2cfs( pCstring, &len );
	wchar*		temp	= NULL;
	wchar*		pBuf	= NULL;
	CFRange		range;
	
	if( kl_tempk_size<len+1 )
	{
		
		kl_tempk_size = len+1;
		kl_tempk = (wchar*)realloc( kl_tempk, sizeof(wchar)*kl_tempk_size );
		KLLog("[char2kchar] realloc buffer %lu bytes.\n", sizeof(wchar)*kl_tempk_size);
		
	}
	
	temp = kl_tempk;
	pBuf = pDest ? pDest : temp;
	
	if( len==0 )
	{
		sCFRelease( cfs );
		return NULL;
	}
	
	if( pDest )
	{
		dlen = destLen;
	}
	else
	{
		dlen = len;
	}
	
	if( pLen )
	{
		*pLen = len;
	}
	
	range.location = 0;
	range.length = dlen;
	CFStringGetCharacters(cfs, range, pBuf);
	
	if( pBuf==temp )
	{
		pBuf[dlen] = 0;
	}
	else
	{
		pBuf[dlen-1] = 0;
	}
	sCFRelease( cfs );
	
	return pBuf;
	
}

//=========================================================================
/**
 *	@brief	同じ文字が「連続」して存在する部分全てを一文字にまとめる<br>
 *			例）”AAABCCDDD” → ”ABCD”<br>
 *			離れた同文字をまとめるわけではないので注意<br>
 *			つまり、"ABCDABCC"のような文字列 は "ABCD" とならず "ABCDABC" となる
 *
 *	@param	pWchar		文字列のポインタ
 *	@param	pMaxLength	最大長へのポインタ
 */
//=========================================================================
void KLString_TrimSameChars( wchar* pWchar, size_t* pMaxLength ){
	
	u32 len		= *pMaxLength;
	
	for( int i=1; i<len; )
	{
		if( pWchar[i]==pWchar[i-1] )
		{
			if( (s32)*pMaxLength-i>0 )
			{
				memmove( &pWchar[i-1], &pWchar[i], sizeof(wchar)*(((s32)*pMaxLength)-i) );
				pWchar[(*pMaxLength)-1] = 0;
				(*pMaxLength)--;
			}
			else
			{
				break;
			}
			
		}
		else
		{
			++i;
		}
		
	}
	
}

//=========================================================================
/**
 *	@brief	（u32文字列版）同じ文字が「連続」して存在する部分全てを一文字にまとめる<br>
 *			例）”AAABCCDDD” → ”ABCD”<br>
 *			離れた同文字をまとめるわけではないので注意<br>
 *			つまり、"ABCDABCC"のような文字列 は "ABCD" とならず "ABCDABC" となる
 *
 *	@param	pStr32		u32文字列のポインタ
 *	@param	pMaxLength	最大長へのポインタ
 */
//=========================================================================
void KLString_TrimSameChars32( u32* pStr32, size_t* pMaxLength ){
	
	u32 len		= *pMaxLength;
	
	for( int i=1; i<len; )
	{
		if( pStr32[i]==pStr32[i-1] )
		{
			if( *pMaxLength>i )
			{
				//KLLog("[ KLString ] Trim32 %x\n", pStr32[i]);
				memmove( &pStr32[i-1], &pStr32[i], sizeof(u32)*(((u32)*pMaxLength)-i) );
				pStr32[(*pMaxLength)-1] = 0;
				(*pMaxLength)--;
				
			}
			else
			{
				break;
			}
			
		}
		else
		{
			++i;
		}
		
	}
	
	if( len - *pMaxLength )
	{
		KLLog("Trim same %lu chars.\n", len-*pMaxLength);
	}
	
}

//=========================================================================
/**
 *	@brief	u32文字列から絵文字コードを削除する
 *
 *	@param	pStr32		u32文字列のポインタ
 *	@param	pMaxLength	最大長へのポインタ
 */
//=========================================================================
void KLString_TrimEmoji32( u32* pStr32, size_t* pMaxLength ){
	
	u32 len		= *pMaxLength;
	
	for( int i=0; i<len; )
	{
		if( KLString_IsEmoji32(pStr32[i]) )
		{
			if( i+1<*pMaxLength )
			{
				memmove( &pStr32[i], &pStr32[i+1], sizeof(u32)*(((u32)*pMaxLength)-i-1) );
				pStr32[(*pMaxLength)-1] = 0;
				(*pMaxLength)--;
			}
			else
			{
				break;
			}
			
		}
		else
		{
			++i;
		}
		
	}
	
}

//=============================================================================
/**
 *	@brief	指定文字列を指定区切り文字で区切った時に何分割されるかを返す<br>
 *
 *	@param	pWchar	調べるkchar文字列
 *	@param	maxlen	調べる文字列の長さ
 *	@param	split	区切り文字
 */
//=============================================================================
size_t KLString_GetSplitArrayNum( wchar* pWchar, size_t maxlen, wchar split ){
	
	size_t ret = 1;
	
	for( int i=0; i<maxlen; ++i )
	{
		if( pWchar[i]==split )
		{
			++ret;
		}
	}
	
	return ret;
	
}

//=============================================================================
/**
 *	@brief	元文字列を指定文字で擬似分割し<br>
 *			各分割頭のポインタにする
 *
 *	@param	pWchar	区切られる元文字列
 *	@param	maxlen	文字列の長さ
 *	@param	split	区切り文字
 *	@param	pNum	区切った配列数を返す先(NULL可)
 *
 *	@return	区切り文字列情報オブジェクトの配列
 *
 *	@warning	関数内部では、新しく文字列バッファを作成しません<br>
 *				代わりに各区切りの頭をポインタとして保持します<br>
 *				そのため、この中で作成したポインタより<br>
 *				先に元文字列を解放したりすると<br>
 *				以後、ポインタも不正な値を参照する事になるので注意が必要です<br>
 *				<br>
 *				つまりは、引数pWcharにローカル変数は使えません
 *				<br>
 *				また、内部でセパレータ文字を終端文字に置き換えます
 */
//=============================================================================
KLString* KLString_SplitToPointers( wchar* pWchar, size_t maxlen, wchar split, size_t* pNum ){
	
	size_t		nArray	= KLString_GetSplitArrayNum( pWchar, maxlen, split );
	KLString*	pInfo	= (KLString*)Malloc( sizeof(KLString)*nArray );
	int i,n,len,start	= 0;
	
	// 分割数を保持
	if(pNum)
	{
		*pNum = nArray;
	}
	
	if(!pInfo)
	{
		KLLog("[ KLString_SplitToPointers ] alloc error.\n");
		return NULL;
	}	
	
	//	配列数 or 文字列長までループ
	for( i=0,n=0,len=0; i<maxlen&&n<nArray; ++i )
	{
		if( pWchar[i]==split )
		{
			pInfo[n].offset		= start;
			pInfo[n].pWchar 	= &pWchar[start];
			pInfo[n].len		= len;
			pWchar[i]			= 0;	// splitを終端に置き換える
			i					+=1;
			start				= i;
			len					= 0;
			++n;
		}
		else
		{
			len++;
		}
	}
	
	// 最後の要素
	if( n<nArray )
	{
		pInfo[n].offset	= start;
		pInfo[n].pWchar = &pWchar[start];
		pInfo[n].len	= len;
		pWchar[i]		= 0;	// splitを終端に置き換える
		++n;
	}
	
	return pInfo;
	
}


//=============================================================================
/**
 *	@brief	元文字列を指定文字で分割する<br>
 *
 *	@param	pWchar	区切られる元文字列
 *	@param	maxlen	文字列の長さ
 *	@param	split	区切り文字
 *	@param	pNum	区切った配列数を返す先(NULL可)
 *
 *	@return	区切り文字列の配列
 *
 *	@warning	関数内部では、新しく文字列バッファを作成しますが<br>
 *				区切り数が多いほどメモリ効率が悪くなる可能性があります
 */
//=============================================================================
wchar** KLString_SplitToUnichars( wchar* pWchar, size_t maxlen, wchar split, size_t* pNum ){
	
	size_t		nArray			= KLString_GetSplitArrayNum( pWchar, maxlen, split );
	wchar**		pp				= (wchar**)Malloc( sizeof(wchar*)*nArray );
	int			i,n,len,start	= 0;
	
	// 分割数を保持
	if(pNum)
	{
		*pNum = nArray;
	}
	
	if(!pp)
	{
		KLLog("[ KLString_SplitToUnichars ] alloc error.\n");
		return NULL;
		
	}
	
	//	配列数 or 文字列長までループ
	for( i=0,n=0,len=0; i<maxlen&&n<nArray; ++i )
	{
		if( pWchar[i]==split )
		{
			wchar* p	= (wchar*)Malloc( sizeof(wchar)*(len+1) );
			
			if(!p)
			{
				KLLog("[ KLString_SplitToUnichars ] alloc error.(2)\n");
				for(n=n-1;n>=0;--n)
				{
					sfree(pp[n]);
				}
				sfree(pp);
				
				return  NULL;
			}
			else
			{
				p[len]		= 0;
				
				if(len)
				{
					memcpy( p, &pWchar[start], sizeof(wchar)*len );
				}
				
				pp[n]	= p;
				i		+=1;
				start	= i;
				len		= 0;
				++n;
			}
		}
		else
		{
			len++;
		}
		
	}
	
	// 最後の要素
	if( n<nArray )
	{
		wchar* p	= (wchar*)Malloc( sizeof(wchar)*(len+1) );
		
		if(!p)
		{
			KLLog("[ KLString_SplitToUnichars ] alloc error.(3)\n");
			for(n=n-1;n>=0;--n)
			{
				sfree(pp[n]);
			}
			sfree(pp);
			
			return  NULL;
		}
		else
		{
			p[len]		= 0;
			
			if(len)
			{
				memcpy( p, &pWchar[start], sizeof(wchar)*len );
			}
			
			pp[n]	= p;
			++n;
		}
	}
	
	return pp;

}

#if 0
//=============================================================================
/**
 *	@brief	元文字列を指定文字で分割する<br>
 *			(使い勝手を犠牲にしたSplitToUnicharsのメモリ節約版)
 *
 *	@param	pWchar	区切られる元文字列
 *	@param	maxlen	文字列の長さ
 *	@param	split	区切り文字
 *	@param	pNum	区切った配列数を返す先(NULL可)
 *
 *	@return	区切り文字列の配列
 *
 *	@warning	関数内部では、新しく文字列バッファを作成しますが<br>
 *				確保しているのは元文字列のコピー１つとダブルポインタ１つの計２つです<br>
 *				戻り値の区切られた各文字列は、実際は１つのコピー内のポインタです<br>
 *				メモリを有効に使える代わりに、解放処理の誤解を招きやすい形になっています<br>
 *
 *				最終的に解放すべき要素は、戻り値*ppとppの２つです<br>
 *				3つ以上に分割されても*pp(==&pp[0])とpp以外は解放しないでください
 */
//=============================================================================
wchar** KLString_SplitToUnicharPointers( wchar* pWchar, size_t maxlen, wchar split, size_t* pNum ){
	
	size_t	nArray			= KLString_GetSplitArrayNum( pWchar, maxlen, split );
	wchar**	pp				= (wchar**)Malloc( sizeof(wchar*)*nArray );
	wchar*	pCore			= (wchar*)Malloc( sizeof(wchar)*(maxlen+1) );
	int		i,n,len,start	= 0;
	
	// 分割数を保持
	if(pNum)
	{
		*pNum = nArray;
	}
	
	if(!pp)
	{
		sfree(pCore);
		KLLog("[ KLString_SplitToUnicharPointers ] alloc error.");
		return NULL;
	}
	
	if(!pCore)
	{
		KLLog("[ KLString_SplitToUnicharPointers ] alloc error.");
		sfree(pp);
		return NULL;
	}
	
	memcpy( pCore, pWchar, sizeof(wchar)*(maxlen+1) );
	
	//	配列数 or 文字列長までループ
	for( i=0,n=0,len=0; i<maxlen&&n<nArray; ++i )
	{
		if( pCore[i]==split )
		{
			pp[n]		= &pCore[start];
			pCore[i]	= 0;	// splitを終端に置き換える
			start		= i+1;
			len			= 0;
			++n;
		}
		else
		{
			len++;
		}
	}
	
	// 最後の要素
	if( n<nArray )
	{
		pp[n]		= &pCore[start];
		pCore[i]	= 0;
		++n;
	}
	
	// todo : analyze を通す
	return pp;
	
}
#endif

//=============================================================================
/**
 *	@brief	指定文字列の指定文字を取り除く(先頭から指定文字以外が来るまで)
 *
 *	@param	pWchar			処理したい文字列
 *	@param	pTrimUnichar	除去したい文字(複数指定可)
 *
 *	@return	除去後の文字列(元の文字列の並びを操作したもの)
 */
//=============================================================================
wchar* KLString_TrimL( wchar* pWchar, wchar* pTrimUnichar ){
	
	size_t len	= wstrlen(pWchar);
	size_t tlen	= wstrlen(pTrimUnichar);
	size_t n	= 0;
	
	for( int i=0; i<len; ++i )
	{
		int hit = 0;
		
		for( int t=0; t<tlen; ++t )
		{
			if( pWchar[i]==pTrimUnichar[t] )
			{
				++n;
				++hit;
				break;
			}
		}
		
		if(!hit)
		{
			break;
		}
		
	}
	
	if(n)
	{
		memmove( pWchar, &pWchar[n], sizeof(wchar)*(len-n) );
		pWchar[len-n] = 0;
	}
	
	return pWchar;
	
}

size_t KLString_GetLen( wchar* pWchar ){
	
	size_t sz = 0;
	
	if( !pWchar )
	{
		return 0;
	}
	
	while( *pWchar )
	{
		pWchar++;
		sz++;
	}
	
	return sz;
	
}

size_t KLString_GetLen32( u32* pStr32 ){
	
	size_t sz = 0;
	
	if( !pStr32 )
	{
		return 0;
	}
	
	while( *pStr32 )
	{
		pStr32++;
		sz++;
	}
	
	return sz;
	
}

size_t KLString_CountIntChar( const wchar* pWchar ){
	
	size_t	res	= 0;
	wchar*	p	= (wchar*)pWchar;
	bl		end	= FALSE;
	
	while( *p && !end && res<32 )
	{
		switch( *p )
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			{
				res++;
			}
			break;
				
			default:
			{
				end = TRUE;
			}
			break;
				
		}
		p++;
	
	}
	
	return res;
	
}

s64 KLString_ParseInt( const wchar* pWchar ){
	
	if( !pWchar )
	{
		return 0;
	}
		
	int		sign	= pWchar[0]=='-' ? -1 :1;
	wchar*	p		= (wchar*)pWchar;
	int		nDigit	= KLString_CountIntChar( sign==1? pWchar : &pWchar[1] );
	u64		rate	= 1;
	s64		res		= 0;
	
	for( int i=1; i<nDigit; ++i )
	{
		rate *= 10;
	}
	
	while( nDigit && *p && rate )
	{
		int hit = -1;
		
		switch( *p )
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			{
				hit = *p-'0';
			}
			break;
		}
		
		if( hit>=0 && hit<=10 )
		{
			res += rate*hit;
		}
		else
		{
			break;
		}
		
		rate /= 10;
		p++;
	
	}
	
	return res*sign;
	
}


size_t CountIntChar( const char* pChar ){
	
	size_t	res	= 0;
	char*	p	= (char*)pChar;
	bl		end	= FALSE;
	
	while( *p && !end && res<32 )
	{
		switch( *p )
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			{
				res++;
			}
				break;
				
			default:
			{
				end = TRUE;
			}
				break;
				
		}
		p++;
		
	}
	
	return res;
	
}

s64 parseInt( const char* pChar ){
	
	int		sign	= pChar ? (pChar[0]=='-' ? -1 :1) : 1;
	char*	p		= (char*)pChar;
	int		nDigit	= pChar ? CountIntChar( sign==1? pChar : &pChar[1] ) : 1;
	u64		rate	= 1;
	s64		res		= 0;

	if( !pChar )
	{
		return 0;
	}
	
	for( int i=1; i<nDigit; ++i )
	{
		rate *= 10;
	}
	
	while( nDigit && *p && rate )
	{
		int hit = -1;
		
		switch( *p )
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			{
				hit = *p-'0';
			}
			break;
		}
		
		if( hit>=0 && hit<=10 )
		{
			res += rate*hit;
		}
		else
		{
			break;
		}
		
		rate /= 10;
		p++;
		
	}
	
	return res*sign;
	
}

/// 次の単語とみなす部分が始まる箇所を返す
s32 KLString_GetActiveWordIndex( wchar* pWchar, u32 offset, u32 len ){
	
	u32		ret			= offset;
	bl		isSkipMode	= TRUE;
	
	for( int i=offset; i<len; ++i )
	{
		// 頭のスペース等のみスルー
		if( pWchar[i]<' ' || pWchar[i]==L'　' )
		{
			if( !isSkipMode )
			{
				return ret;
			}
		}
		else
		{
			if( isSkipMode )
			{
				return ret;
			}
			
			// スキップモードでスペース以外がきたら次のスペース系で終了
			isSkipMode = FALSE;
		}

		++ret;
	}
	
	return ret;
}

/// 単語とみなす部分の長さを取得
u32 KLString_GetWordLen( wchar* pWchar, u32 idx, u32 len ){
	
	u32		ret			= 0;
	
    if( idx==0xFFFFFFFF )
    {
        return 0;
    }
    
	for( int i=idx; i<len; ++i )
	{
		if( pWchar[i]==0 || pWchar[i]<=' ' || pWchar[i]==(wchar)L'　' )
		{
			return ret;
		}
		
		++ret;
	}
	
	return ret;
}

/// スペースとみなす部分の長さを取得
u32 KLString_GetSpaceLen( wchar* pWchar, u32 idx, u32 len, bl onlyHankaku ){
	
	u32		ret			= 0;
	
    if( idx==0xFFFFFFFF )
    {
        return 0;
    }
    
	for( int i=idx; i<len; ++i )
	{
		if( onlyHankaku )
		{
			if( pWchar[i]>' ' )
			{
				return ret;
			}	
		}
		else
		{
			if( pWchar[i]>' ' && pWchar[i]!=(wchar)L'　' )
			{
				return ret;
			}
		}
		++ret;
	}
	
	return ret;
}

wchar KLString_Lowercase( wchar kc ){
	
	if( kc>='A' && kc<='Z' )
	{
		return kc-'A'+'a';
	}
	
	return kc;
}

wchar KLString_Uppercase( wchar kc ){
	
	if( kc>='a' && kc<='z' )
	{
		return kc-'a'+'A';
	}
	
	return kc;
}

s32 KLString_Comp( wchar* pStr1, wchar* pStr2, bl ignoreCase ){
	
	u32 i = 0;
	
	for( i=0; pStr1[i]&&pStr2[i]; ++i )
	{
		if( pStr1[i] )
		{
			if( pStr2[i] )
			{
				if( ignoreCase )
				{
					if( wlowercase(pStr1[i])==wlowercase(pStr2[i]) )
					{
						
					}
					else
					{
						if( wlowercase(pStr1[i])<wlowercase(pStr2[i]) )
						{
							return 1;
						}
						
						return -1;
					}
				}
				else
				{
					if( pStr1[i]==pStr2[i] )
					{
						
					}
					else
					{
						if( pStr1[i]<pStr2[i] )
						{
							return 1;
						}
						
						return -1;
					}
				}
			}
			else
			{
				return 1;
			}

		}
		else if( pStr2[i] )
		{
			return -1;
		}
		
	}
	
	return i==0;
	
}

u32 ucharlen( const char* s ){
	
	u32	ret = 0;
    u32 len = 0;
    u8  c[ 4 ];

	if(s == NULL)
	{
		return 0;
	}

    len = strlen(s);

	if (len == 0)
	{
		return 0;
	}
	
	c[ 0 ] = s[ 0 ];
    
	if( (c[ 0 ] >= 0x01) && (c[ 0 ] <= 0x7F) )
	{
		ret = 1;                     // 1バイト ASCII : 0x00～0x7F
	}
	else if (len >= 2)
	{
		c[ 1 ] = s[ 1 ];
        
		if(	((c[ 0 ] >= 0xC0) && (c[ 0 ] <= 0xDF)) &&
			((c[ 1 ] >= 0x80) && (c[ 1 ] <= 0xBF)))
        {
            ret = 2;                 // 2バイト
		}
		else if( len >= 3 )
		{
			c[ 2 ] = s[ 2 ];
			if ((c[ 0 ] == 0xEF) && (c[ 1 ] == 0xBB) && (c[ 2 ] == 0xBF))
			{
				ret = 0;             // BOM 0xEF 0xBB 0xBF
			}
			else if(	((c[ 0 ] >= 0xE0) && (c[ 0 ] <= 0xEF)) &&
					 	((c[ 1 ] >= 0x80) && (c[ 1 ] <= 0xBF)) &&
					 	((c[ 2 ] >= 0x80) && (c[ 2 ] <= 0xBF)))
			{
				ret = 3;             // 3バイト
			}
			else if( len >= 4 )
			{
				c[ 3 ] = s[ 3 ];
				if(	((c[ 0 ] >= 0xF0) && (c[ 0 ] <= 0xF7)) &&
					((c[ 1 ] >= 0x80) && (c[ 1 ] <= 0xBF)) &&
					((c[ 2 ] >= 0x80) && (c[ 2 ] <= 0xBF)) &&
					((c[ 3 ] >= 0x80) && (c[ 3 ] <= 0xBF)))
				{
					ret = 4;         // 4バイト
				}
				
			}
		
		}
		
	}
	
	return ret;
}

//===========================================
/**
 *	@brief デフォルトchar文字列（UTF-8）対応のstrlen
 *
 *	@param	s	char文字列
 *
 *	@return	文字数
 */
//===========================================
u32 cstrlen( const char* s ){
	
	u32 len	= strlen(s);
	u32 u	= 0;
	u32 ret	= 0;
	
	for( u32 i=0; i<len; )
	{
		u = ucharlen(&s[i]);
		
		if( !u )
		{
			break;
		}
		
		i += u;
		ret++;
	}
	
	return ret;
}

//=========================================================================
/**
 *	@brief	絵文字コードか否かを判別する
 *
 *	@param	c		u32文字
 *
 *	@return	絵文字とみなされた場合にTRUE
 */
//=========================================================================
bl KLString_IsEmoji32( u32 c ){
	
	u32 u = c;
	
	// 有効な桁数だけでエンディアン反転
	if( 0xFF000000 & c )
	{
		u = ((c&0xFF)<<24) | (((c>>8)&0xFF)<<16) | (((c>>16)&0xFF)<<8) | ((c>>24)&0xFF);
	}
	else if( 0xFF0000 & c )
	{
		u = ((c&0xFF)<<16) | (((c>>8)&0xFF)<<8) | ((c>>16)&0xFF);
	}
	else if( 0xFF00 & c )
	{
		u = ((c&0xFF)<<8) | ((c>>8)&0xFF);
	}
	
	switch( u )
	{
		case 0xF09F9884:
		case 0xF09F9883:
		case 0xF09F9880:
		case 0xF09F988A:
		case 0xE298BA:
		case 0xF09F9889:
		case 0xF09F988D:
		case 0xF09F9898:
		case 0xF09F989A:
		case 0xF09F9897:
		case 0xF09F9899:
		case 0xF09F989C:
		case 0xF09F989D:
		case 0xF09F989B:
		case 0xF09F98B3:
		case 0xF09F9881:
		case 0xF09F9894:
		case 0xF09F988C:
		case 0xF09F9892:
		case 0xF09F989E:
		case 0xF09F98A3:
		case 0xF09F98A2:
		case 0xF09F9882:
		case 0xF09F98AD:
		case 0xF09F98AA:
		case 0xF09F98A5:
		case 0xF09F98B0:
		case 0xF09F9885:
		case 0xF09F9893:
		case 0xF09F98A9:
		case 0xF09F98AB:
		case 0xF09F98A8:
		case 0xF09F98B1:
		case 0xF09F98A0:
		case 0xF09F98A1:
		case 0xF09F98A4:
		case 0xF09F9896:
		case 0xF09F9886:
		case 0xF09F988B:
		case 0xF09F98B7:
		case 0xF09F988E:
		case 0xF09F98B4:
		case 0xF09F98B5:
		case 0xF09F98B2:
		case 0xF09F989F:
		case 0xF09F98A6:
		case 0xF09F98A7:
		case 0xF09F9888:
		case 0xF09F91BF:
		case 0xF09F98AE:
		case 0xF09F98AC:
		case 0xF09F9890:
		case 0xF09F9895:
		case 0xF09F98AF:
		case 0xF09F98B6:
		case 0xF09F9887:
		case 0xF09F988F:
		case 0xF09F9891:
		case 0xF09F91B2:
		case 0xF09F91B3:
		case 0xF09F91AE:
		case 0xF09F91B7:
		case 0xF09F9282:
		case 0xF09F91B6:
		case 0xF09F91A6:
		case 0xF09F91A7:
		case 0xF09F91A8:
		case 0xF09F91A9:
		case 0xF09F91B4:
		case 0xF09F91B5:
		case 0xF09F91B1:
		case 0xF09F91BC:
		case 0xF09F91B8:
		case 0xF09F98BA:
		case 0xF09F98B8:
		case 0xF09F98BB:
		case 0xF09F98BD:
		case 0xF09F98BC:
		case 0xF09F9980:
		case 0xF09F98BF:
		case 0xF09F98B9:
		case 0xF09F98BE:
		case 0xF09F91B9:
		case 0xF09F91BA:
		case 0xF09F9988:
		case 0xF09F9989:
		case 0xF09F998A:
		case 0xF09F9280:
		case 0xF09F91BD:
		case 0xF09F92A9:
		case 0xF09F94A5:
		case 0xE29CA8:
		case 0xF09F8C9F:
		case 0xF09F92AB:
		case 0xF09F92A5:
		case 0xF09F92A2:
		case 0xF09F92A6:
		case 0xF09F92A7:
		case 0xF09F92A4:
		case 0xF09F92A8:
		case 0xF09F9182:
		case 0xF09F9180:
		case 0xF09F9183:
		case 0xF09F9185:
		case 0xF09F9184:
		case 0xF09F918D:
		case 0xF09F918E:
		case 0xF09F918C:
		case 0xF09F918A:
		case 0xE29C8A:
		case 0xE29C8C:
		case 0xF09F918B:
		case 0xE29C8B:
		case 0xF09F9190:
		case 0xF09F9186:
		case 0xF09F9187:
		case 0xF09F9189:
		case 0xF09F9188:
		case 0xF09F998C:
		case 0xF09F998F:
		case 0xE2989D:
		case 0xF09F918F:
		case 0xF09F92AA:
		case 0xF09F9AB6:
		case 0xF09F8F83:
		case 0xF09F9283:
		case 0xF09F91AB:
		case 0xF09F91AA:
		case 0xF09F91AC:
		case 0xF09F91AD:
		case 0xF09F928F:
		case 0xF09F9291:
		case 0xF09F91AF:
		case 0xF09F9986:
		case 0xF09F9985:
		case 0xF09F9281:
		case 0xF09F998B:
		case 0xF09F9286:
		case 0xF09F9287:
		case 0xF09F9285:
		case 0xF09F91B0:
		case 0xF09F998E:
		case 0xF09F998D:
		case 0xF09F9987:
		case 0xF09F8EA9:
		case 0xF09F9191:
		case 0xF09F9192:
		case 0xF09F919F:
		case 0xF09F919E:
		case 0xF09F91A1:
		case 0xF09F91A0:
		case 0xF09F91A2:
		case 0xF09F9195:
		case 0xF09F9194:
		case 0xF09F919A:
		case 0xF09F9197:
		case 0xF09F8EBD:
		case 0xF09F9196:
		case 0xF09F9198:
		case 0xF09F9199:
		case 0xF09F92BC:
		case 0xF09F919C:
		case 0xF09F919D:
		case 0xF09F919B:
		case 0xF09F9193:
		case 0xF09F8E80:
		case 0xF09F8C82:
		case 0xF09F9284:
		case 0xF09F929B:
		case 0xF09F9299:
		case 0xF09F929C:
		case 0xF09F929A:
		case 0xE29DA4:
		case 0xF09F9294:
		case 0xF09F9297:
		case 0xF09F9293:
		case 0xF09F9295:
		case 0xF09F9296:
		case 0xF09F929E:
		case 0xF09F9298:
		case 0xF09F928C:
		case 0xF09F928B:
		case 0xF09F928D:
		case 0xF09F928E:
		case 0xF09F91A4:
		case 0xF09F91A5:
		case 0xF09F92AC:
		case 0xF09F91A3:
		case 0xF09F92AD:
		case 0xF09F90B6:
		case 0xF09F90BA:
		case 0xF09F90B1:
		case 0xF09F90B9:
		case 0xF09F90B0:
		case 0xF09F90B8:
		case 0xF09F90AF:
		case 0xF09F90A8:
		case 0xF09F90BB:
		case 0xF09F90B7:
		case 0xF09F90BD:
		case 0xF09F90AE:
		case 0xF09F9097:
		case 0xF09F90B5:
		case 0xF09F9092:
		case 0xF09F90B4:
		case 0xF09F9091:
		case 0xF09F9098:
		case 0xF09F90BC:
		case 0xF09F90A7:
		case 0xF09F90A6:
		case 0xF09F90A4:
		case 0xF09F90A5:
		case 0xF09F90A3:
		case 0xF09F9094:
		case 0xF09F908D:
		case 0xF09F90A2:
		case 0xF09F909B:
		case 0xF09F909D:
		case 0xF09F909C:
		case 0xF09F909E:
		case 0xF09F908C:
		case 0xF09F9099:
		case 0xF09F909A:
		case 0xF09F90A0:
		case 0xF09F909F:
		case 0xF09F90AC:
		case 0xF09F90B3:
		case 0xF09F908B:
		case 0xF09F9084:
		case 0xF09F908F:
		case 0xF09F9080:
		case 0xF09F9083:
		case 0xF09F9085:
		case 0xF09F9087:
		case 0xF09F9089:
		case 0xF09F908E:
		case 0xF09F9090:
		case 0xF09F9093:
		case 0xF09F9095:
		case 0xF09F9096:
		case 0xF09F9081:
		case 0xF09F9082:
		case 0xF09F90B2:
		case 0xF09F90A1:
		case 0xF09F908A:
		case 0xF09F90AB:
		case 0xF09F90AA:
		case 0xF09F9086:
		case 0xF09F9088:
		case 0xF09F90A9:
		case 0xF09F90BE:
		case 0xF09F9290:
		case 0xF09F8CB8:
		case 0xF09F8CB7:
		case 0xF09F8D80:
		case 0xF09F8CB9:
		case 0xF09F8CBB:
		case 0xF09F8CBA:
		case 0xF09F8D81:
		case 0xF09F8D83:
		case 0xF09F8D82:
		case 0xF09F8CBF:
		case 0xF09F8CBE:
		case 0xF09F8D84:
		case 0xF09F8CB5:
		case 0xF09F8CB4:
		case 0xF09F8CB2:
		case 0xF09F8CB3:
		case 0xF09F8CB0:
		case 0xF09F8CB1:
		case 0xF09F8CBC:
		case 0xF09F8C90:
		case 0xF09F8C9E:
		case 0xF09F8C9D:
		case 0xF09F8C9A:
		case 0xF09F8C91:
		case 0xF09F8C92:
		case 0xF09F8C93:
		case 0xF09F8C94:
		case 0xF09F8C95:
		case 0xF09F8C96:
		case 0xF09F8C97:
		case 0xF09F8C98:
		case 0xF09F8C9C:
		case 0xF09F8C9B:
		case 0xF09F8C99:
		case 0xF09F8C8D:
		case 0xF09F8C8E:
		case 0xF09F8C8F:
		case 0xF09F8C8B:
		case 0xF09F8C8C:
		case 0xF09F8CA0:
		case 0xE2AD90:
		case 0xE29880:
		case 0xE29B85:
		case 0xE29881:
		case 0xE29AA1:
		case 0xE29894:
		case 0xE29D84:
		case 0xE29B84:
		case 0xF09F8C80:
		case 0xF09F8C81:
		case 0xF09F8C88:
		case 0xF09F8C8A:
		case 0xF09F8E8D:
		case 0xF09F929D:
		case 0xF09F8E8E:
		case 0xF09F8E92:
		case 0xF09F8E93:
		case 0xF09F8E8F:
		case 0xF09F8E86:
		case 0xF09F8E87:
		case 0xF09F8E90:
		case 0xF09F8E91:
		case 0xF09F8E83:
		case 0xF09F91BB:
		case 0xF09F8E85:
		case 0xF09F8E84:
		case 0xF09F8E81:
		case 0xF09F8E8B:
		case 0xF09F8E89:
		case 0xF09F8E8A:
		case 0xF09F8E88:
		case 0xF09F8E8C:
		case 0xF09F94AE:
		case 0xF09F8EA5:
		case 0xF09F93B7:
		case 0xF09F93B9:
		case 0xF09F93BC:
		case 0xF09F92BF:
		case 0xF09F9380:
		case 0xF09F92BD:
		case 0xF09F92BE:
		case 0xF09F92BB:
		case 0xF09F93B1:
		case 0xE2988E:
		case 0xF09F939E:
		case 0xF09F939F:
		case 0xF09F93A0:
		case 0xF09F93A1:
		case 0xF09F93BA:
		case 0xF09F93BB:
		case 0xF09F948A:
		case 0xF09F9489:
		case 0xF09F9488:
		case 0xF09F9487:
		case 0xF09F9494:
		case 0xF09F9495:
		case 0xF09F93A2:
		case 0xF09F93A3:
		case 0xE28FB3:
		case 0xE28C9B:
		case 0xE28FB0:
		case 0xE28C9A:
		case 0xF09F9493:
		case 0xF09F9492:
		case 0xF09F948F:
		case 0xF09F9490:
		case 0xF09F9491:
		case 0xF09F948E:
		case 0xF09F92A1:
		case 0xF09F94A6:
		case 0xF09F9486:
		case 0xF09F9485:
		case 0xF09F948C:
		case 0xF09F948B:
		case 0xF09F948D:
		case 0xF09F9B81:
		case 0xF09F9B80:
		case 0xF09F9ABF:
		case 0xF09F9ABD:
		case 0xF09F94A7:
		case 0xF09F94A9:
		case 0xF09F94A8:
		case 0xF09F9AAA:
		case 0xF09F9AAC:
		case 0xF09F92A3:
		case 0xF09F94AB:
		case 0xF09F94AA:
		case 0xF09F928A:
		case 0xF09F9289:
		case 0xF09F92B0:
		case 0xF09F92B4:
		case 0xF09F92B5:
		case 0xF09F92B7:
		case 0xF09F92B6:
		case 0xF09F92B3:
		case 0xF09F92B8:
		case 0xF09F93B2:
		case 0xF09F93A7:
		case 0xF09F93A5:
		case 0xF09F93A4:
		case 0xE29C89:
		case 0xF09F93A9:
		case 0xF09F93A8:
		case 0xF09F93AF:
		case 0xF09F93AB:
		case 0xF09F93AA:
		case 0xF09F93AC:
		case 0xF09F93AD:
		case 0xF09F93AE:
		case 0xF09F93A6:
		case 0xF09F939D:
		case 0xF09F9384:
		case 0xF09F9383:
		case 0xF09F9391:
		case 0xF09F938A:
		case 0xF09F9388:
		case 0xF09F9389:
		case 0xF09F939C:
		case 0xF09F938B:
		case 0xF09F9385:
		case 0xF09F9386:
		case 0xF09F9387:
		case 0xF09F9381:
		case 0xF09F9382:
		case 0xE29C82:
		case 0xF09F938C:
		case 0xF09F938E:
		case 0xE29C92:
		case 0xE29C8F:
		case 0xF09F938F:
		case 0xF09F9390:
		case 0xF09F9395:
		case 0xF09F9397:
		case 0xF09F9398:
		case 0xF09F9399:
		case 0xF09F9393:
		case 0xF09F9394:
		case 0xF09F9392:
		case 0xF09F939A:
		case 0xF09F9396:
		case 0xF09F9496:
		case 0xF09F939B:
		case 0xF09F94AC:
		case 0xF09F94AD:
		case 0xF09F93B0:
		case 0xF09F8EA8:
		case 0xF09F8EAC:
		case 0xF09F8EA4:
		case 0xF09F8EA7:
		case 0xF09F8EBC:
		case 0xF09F8EB5:
		case 0xF09F8EB6:
		case 0xF09F8EB9:
		case 0xF09F8EBB:
		case 0xF09F8EBA:
		case 0xF09F8EB7:
		case 0xF09F8EB8:
		case 0xF09F91BE:
		case 0xF09F8EAE:
		case 0xF09F838F:
		case 0xF09F8EB4:
		case 0xF09F8084:
		case 0xF09F8EB2:
		case 0xF09F8EAF:
		case 0xF09F8F88:
		case 0xF09F8F80:
		case 0xE29ABD:
		case 0xE29ABE:
		case 0xF09F8EBE:
		case 0xF09F8EB1:
		case 0xF09F8F89:
		case 0xF09F8EB3:
		case 0xE29BB3:
		case 0xF09F9AB5:
		case 0xF09F9AB4:
		case 0xF09F8F81:
		case 0xF09F8F87:
		case 0xF09F8F86:
		case 0xF09F8EBF:
		case 0xF09F8F82:
		case 0xF09F8F8A:
		case 0xF09F8F84:
		case 0xF09F8EA3:
		case 0xE29895:
		case 0xF09F8DB5:
		case 0xF09F8DB6:
		case 0xF09F8DBC:
		case 0xF09F8DBA:
		case 0xF09F8DBB:
		case 0xF09F8DB8:
		case 0xF09F8DB9:
		case 0xF09F8DB7:
		case 0xF09F8DB4:
		case 0xF09F8D95:
		case 0xF09F8D94:
		case 0xF09F8D9F:
		case 0xF09F8D97:
		case 0xF09F8D96:
		case 0xF09F8D9D:
		case 0xF09F8D9B:
		case 0xF09F8DA4:
		case 0xF09F8DB1:
		case 0xF09F8DA3:
		case 0xF09F8DA5:
		case 0xF09F8D99:
		case 0xF09F8D98:
		case 0xF09F8D9A:
		case 0xF09F8D9C:
		case 0xF09F8DB2:
		case 0xF09F8DA2:
		case 0xF09F8DA1:
		case 0xF09F8DB3:
		case 0xF09F8D9E:
		case 0xF09F8DA9:
		case 0xF09F8DAE:
		case 0xF09F8DA6:
		case 0xF09F8DA8:
		case 0xF09F8DA7:
		case 0xF09F8E82:
		case 0xF09F8DB0:
		case 0xF09F8DAA:
		case 0xF09F8DAB:
		case 0xF09F8DAC:
		case 0xF09F8DAD:
		case 0xF09F8DAF:
		case 0xF09F8D8E:
		case 0xF09F8D8F:
		case 0xF09F8D8A:
		case 0xF09F8D8B:
		case 0xF09F8D92:
		case 0xF09F8D87:
		case 0xF09F8D89:
		case 0xF09F8D93:
		case 0xF09F8D91:
		case 0xF09F8D88:
		case 0xF09F8D8C:
		case 0xF09F8D90:
		case 0xF09F8D8D:
		case 0xF09F8DA0:
		case 0xF09F8D86:
		case 0xF09F8D85:
		case 0xF09F8CBD:
		case 0xF09F8FA0:
		case 0xF09F8FA1:
		case 0xF09F8FAB:
		case 0xF09F8FA2:
		case 0xF09F8FA3:
		case 0xF09F8FA5:
		case 0xF09F8FA6:
		case 0xF09F8FAA:
		case 0xF09F8FA9:
		case 0xF09F8FA8:
		case 0xF09F9292:
		case 0xE29BAA:
		case 0xF09F8FAC:
		case 0xF09F8FA4:
		case 0xF09F8C87:
		case 0xF09F8C86:
		case 0xF09F8FAF:
		case 0xF09F8FB0:
		case 0xE29BBA:
		case 0xF09F8FAD:
		case 0xF09F97BC:
		case 0xF09F97BE:
		case 0xF09F97BB:
		case 0xF09F8C84:
		case 0xF09F8C85:
		case 0xF09F8C83:
		case 0xF09F97BD:
		case 0xF09F8C89:
		case 0xF09F8EA0:
		case 0xF09F8EA1:
		case 0xE29BB2:
		case 0xF09F8EA2:
		case 0xF09F9AA2:
		case 0xE29BB5:
		case 0xF09F9AA4:
		case 0xF09F9AA3:
		case 0xE29A93:
		case 0xF09F9A80:
		case 0xE29C88:
		case 0xF09F92BA:
		case 0xF09F9A81:
		case 0xF09F9A82:
		case 0xF09F9A8A:
		case 0xF09F9A89:
		case 0xF09F9A8E:
		case 0xF09F9A86:
		case 0xF09F9A84:
		case 0xF09F9A85:
		case 0xF09F9A88:
		case 0xF09F9A87:
		case 0xF09F9A9D:
		case 0xF09F9A8B:
		case 0xF09F9A83:
		case 0xF09F9A8C:
		case 0xF09F9A8D:
		case 0xF09F9A99:
		case 0xF09F9A98:
		case 0xF09F9A97:
		case 0xF09F9A95:
		case 0xF09F9A96:
		case 0xF09F9A9B:
		case 0xF09F9A9A:
		case 0xF09F9AA8:
		case 0xF09F9A93:
		case 0xF09F9A94:
		case 0xF09F9A92:
		case 0xF09F9A91:
		case 0xF09F9A90:
		case 0xF09F9AB2:
		case 0xF09F9AA1:
		case 0xF09F9A9F:
		case 0xF09F9AA0:
		case 0xF09F9A9C:
		case 0xF09F9288:
		case 0xF09F9A8F:
		case 0xF09F8EAB:
		case 0xF09F9AA6:
		case 0xF09F9AA5:
		case 0xE29AA0:
		case 0xF09F9AA7:
		case 0xF09F94B0:
		case 0xE29BBD:
		case 0xF09F8FAE:
		case 0xF09F8EB0:
		case 0xE299A8:
		case 0xF09F97BF:
		case 0xF09F8EAA:
		case 0xF09F8EAD:
		case 0xF09F938D:
		case 0xF09F9AA9:
		case 0xF09F87AF:
		case 0xF09F87B5:
		case 0xF09F87B0:
		case 0xF09F87B7:
		case 0xF09F87A9:
		case 0xF09F87AA:
		case 0xF09F87A8:
		case 0xF09F87B3:
		case 0xF09F87BA:
		case 0xF09F87B8:
		case 0xF09F87AB:
		case 0xF09F87AE:
		case 0xF09F87B9:
		case 0xF09F87AC:
		case 0xF09F87A7:
		case 0xF09F949F:
		case 0xF09F94A2:
		case 0xE283A3:
		case 0xF09F94A3:
		case 0xE2AC86:
		case 0xE2AC87:
		case 0xE2AC85:
		case 0xE29EA1:
		case 0xF09F94A0:
		case 0xF09F94A1:
		case 0xF09F94A4:
		case 0xE28697:
		case 0xE28696:
		case 0xE28698:
		case 0xE28699:
		case 0xE28694:
		case 0xE28695:
		case 0xF09F9484:
		case 0xE29780:
		case 0xE296B6:
		case 0xF09F94BC:
		case 0xF09F94BD:
		case 0xE286A9:
		case 0xE286AA:
		case 0xE284B9:
		case 0xE28FAA:
		case 0xE28FA9:
		case 0xE28FAB:
		case 0xE28FAC:
		case 0xE2A4B5:
		case 0xE2A4B4:
		case 0xF09F8697:
		case 0xF09F9480:
		case 0xF09F9481:
		case 0xF09F9482:
		case 0xF09F8695:
		case 0xF09F8699:
		case 0xF09F8692:
		case 0xF09F8693:
		case 0xF09F8696:
		case 0xF09F93B6:
		case 0xF09F8EA6:
		case 0xF09F8881:
		case 0xF09F88AF:
		case 0xF09F88B3:
		case 0xF09F88B5:
		case 0xF09F88B4:
		case 0xF09F88B2:
		case 0xF09F8990:
		case 0xF09F88B9:
		case 0xF09F88BA:
		case 0xF09F88B6:
		case 0xF09F889A:
		case 0xF09F9ABB:
		case 0xF09F9AB9:
		case 0xF09F9ABA:
		case 0xF09F9ABC:
		case 0xF09F9ABE:
		case 0xF09F9AB0:
		case 0xF09F9AAE:
		case 0xF09F85BF:
		case 0xE299BF:
		case 0xF09F9AAD:
		case 0xF09F88B7:
		case 0xF09F88B8:
		case 0xF09F8882:
		case 0xE29382:
		case 0xF09F9B82:
		case 0xF09F9B84:
		case 0xF09F9B85:
		case 0xF09F9B83:
		case 0xF09F8991:
		case 0xE38A99:
		case 0xE38A97:
		case 0xF09F8691:
		case 0xF09F8698:
		case 0xF09F8694:
		case 0xF09F9AAB:
		case 0xF09F949E:
		case 0xF09F93B5:
		case 0xF09F9AAF:
		case 0xF09F9AB1:
		case 0xF09F9AB3:
		case 0xF09F9AB7:
		case 0xF09F9AB8:
		case 0xE29B94:
		case 0xE29CB3:
		case 0xE29D87:
		case 0xE29D8E:
		case 0xE29C85:
		case 0xE29CB4:
		case 0xF09F929F:
		case 0xF09F869A:
		case 0xF09F93B3:
		case 0xF09F93B4:
		case 0xF09F85B0:
		case 0xF09F85B1:
		case 0xF09F868E:
		case 0xF09F85BE:
		case 0xF09F92A0:
		case 0xE29EBF:
		case 0xE299BB:
		case 0xE29988:
		case 0xE29989:
		case 0xE2998A:
		case 0xE2998B:
		case 0xE2998C:
		case 0xE2998D:
		case 0xE2998E:
		case 0xE2998F:
		case 0xE29990:
		case 0xE29991:
		case 0xE29992:
		case 0xE29993:
		case 0xE29B8E:
		case 0xF09F94AF:
		case 0xF09F8FA7:
		case 0xF09F92B9:
		case 0xF09F92B2:
		case 0xF09F92B1:
		case 0xC2A9:
		case 0xC2AE:
		case 0xE284A2:
		case 0xE29D8C:
		case 0xE280BC:
		case 0xE28189:
		case 0xE29D97:
		case 0xE29D93:
		case 0xE29D95:
		case 0xE29D94:
		case 0xE2AD95:
		case 0xF09F949D:
		case 0xF09F949A:
		case 0xF09F9499:
		case 0xF09F949B:
		case 0xF09F949C:
		case 0xF09F9483:
		case 0xF09F959B:
		case 0xF09F95A7:
		case 0xF09F9590:
		case 0xF09F959C:
		case 0xF09F9591:
		case 0xF09F959D:
		case 0xF09F9592:
		case 0xF09F959E:
		case 0xF09F9593:
		case 0xF09F959F:
		case 0xF09F9594:
		case 0xF09F95A0:
		case 0xF09F9595:
		case 0xF09F95A1:
		case 0xF09F9596:
		case 0xF09F95A2:
		case 0xF09F9597:
		case 0xF09F95A3:
		case 0xF09F9598:
		case 0xF09F95A4:
		case 0xF09F9599:
		case 0xF09F95A5:
		case 0xF09F959A:
		case 0xF09F95A6:
		case 0xE29C96:
		case 0xE29E95:
		case 0xE29E96:
		case 0xE29E97:
		case 0xE299A0:
		case 0xE299A5:
		case 0xE299A3:
		case 0xE299A6:
		case 0xF09F92AE:
		case 0xF09F92AF:
		case 0xE29C94:
		case 0xE29891:
		case 0xF09F9498:
		case 0xF09F9497:
		case 0xE29EB0:
		case 0xE380B0:
		case 0xE380BD:
		case 0xF09F94B1:
		case 0xE297BC:
		case 0xE297BB:
		case 0xE297BE:
		case 0xE297BD:
		case 0xE296AA:
		case 0xE296AB:
		case 0xF09F94BA:
		case 0xF09F94B2:
		case 0xF09F94B3:
		case 0xE29AAB:
		case 0xE29AAA:
		case 0xF09F94B4:
		case 0xF09F94B5:
		case 0xF09F94BB:
		case 0xE2AC9C:
		case 0xE2AC9B:
		case 0xF09F94B6:
		case 0xF09F94B7:
		case 0xF09F94B8:
		case 0xF09F94B9:
			
		return TRUE;
	
	}
	
	return FALSE;

}