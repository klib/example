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

#ifndef ___KL_MEM_H___
#define ___KL_MEM_H___

#include "c_extern.h"
	
	// I N C L U D E
	//=============================================================
	
	#include <stdlib.h>
	#include "KLib.h"
	#include "KLList.h"
	
	
	// D E F I N E
	//=============================================================
			
	/// Safe release context
	#define sCGContextRelease(p) if( p ){ CGContextRelease(p); } p = NULL
	
	/// Safe release image
	#define sCGImageRelease(p) if( p ){ CGImageRelease(p); } p = NULL
	
	/// Safe free CF object
	#define sCFRelease(p) if( p ){ CFRelease(p); } p = NULL

	/// Safe release fontref
	#define sCGFontRelease( fontRef )	if( fontRef ){ CGFontRelease(fontRef); } fontRef = NULL
	
	void* malloc_test(char* pFileName, u32 line, size_t sz);
	
	// S T R U C T
	//=============================================================
	
	/// HeapInfo
	typedef struct{
		
		u32		id;			///< Unique ID
		size_t	size;		///< Heap size
		void*	pAddress;	///< Heap address
		
#if KL_DEBUG
		//------------------------------------------------------------------
		u16		uiLine;						///< Allocated line
		char	strFile[ KLIBMEM_PATHLEN ];	///< Allocated file path
		//------------------------------------------------------------------
#endif
		
	}KLibDebugHeapInfo;
	
	/// ヒープデバッグ用オブジェクト
	typedef struct{
		
		u32				uiTotalSize;	///< 合計サイズ
		u32				uiMaxSize;		///< 最大サイズ
		u32				uiHeapNum;		///< ヒープ数
		
		KLList		*pTop;		///< 先頭リストへのポインタ
		KLList		*pTail;		///< 終端リストのポインタ
		
	} KLHeapDebug;
	
// F U N C T I O N
//===============================================================

	void* KLHeapDebug_CoreAlloc( KLHeapDebug* pHd, size_t szTarget, char* pchFilePath, u32 uiLine  );
	void KLHeapDebug_CoreFree( KLHeapDebug* pHd, void* p, char* pchFilePath, u32 uiLine );

	struct _KLList *KLHeapDebug_SearchHeap( KLHeapDebug* pHd, void* p );
	size_t KLHeapDebug_GetRemain();
	size_t KLHeapDebug_Len( KLHeapDebug* pHd );
	void KLHeapDebug_OutputList( KLHeapDebug* pHd );

// M A C R O
//==========================================

	#if KL_DEBUG
		#define	KLHeapDebug_Alloc( pHd, size )	\
				KLHeapDebug_CoreAlloc( pHd, size, __FILE__, __LINE__ )
	#else
		#define	KLHeapDebug_Alloc( pHd, size )	\
				KLHeapDebug_CoreAlloc( pHd, size, NULL, 0 )
	#endif

	
	#if KL_DEBUG
		#define	KLHeapDebug_Free( pHd, p )	\
				if( p )		\
					KLHeapDebug_CoreFree( pHd, p, __FILE__, __LINE__ );	\
				p = NULL
	#else
		#define	KLHeapDebug_Free( pHd, p )	\
				if( p )		\
					KLHeapDebug_CoreFree( pHd, NULL, 0 );	\
				p = NULL						
	#endif				

	
#include "c_extern_close.h"

#endif
