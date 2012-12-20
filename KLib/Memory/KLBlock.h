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

#ifndef	___KLIB_ALLOC_H___
#define	___KLIB_ALLOC_H___

#include "c_extern.h"
#include "KLibDefine.h"
#include <string.h>

/// デバッグ情報の有無(ブロックヘッダサイズ消費数が増加するので注意)
#define KLIBALLOC_DEBUG (1)

/// アラインメントサイズ
#define KLIBALLOC_ALIGN_BYTES (4)

/// デバッグ情報用ファイルネームバッファサイズ
#define KLIBALLOC_FILENAME_LEN (64)

/// メモリ状態
typedef enum{

	KLBLOCK_STATE_EMPTY = 0,
	KLBLOCK_STATE_ACTIVE,
	KLBLOCK_STATE_MAX,
	
}KLBlockState;

/// ブロックヘッダ(必ず4の倍数のアラインメントにする事)
typedef struct KLBlockHeader{
	
	KLBlockState			state;
	size_t					szHeap, szBlock;
	u8*						pBlockHead, *pHeapHead;	///< 存在する全ブロックのリスト(状態無視)
	struct KLBlockHeader*	pPrev,*pNext;
	
	#if KLibAlloc_DEBUG
	
		char				filename[KLIBALLOC_FILENAME_LEN];
		u32					line;
	
	#endif
	
}KLBlockHeader;

/// ヒープマネージャ
typedef struct{

	u8*		pHeap;			///< ヒープバッファ
	size_t	nBlock;			///< 全ブロック数
	size_t	nEmptyBlock;	///< 空きブロック数
	size_t	nActiveBlock;	///< 使用ブロック数
	size_t	szTotal;		///< 使用サイズ
	size_t	szRemain;		///< 残り使用可能サイズ
	
	KLBlockHeader *pTop,*pTail;				///< 全ブロックの先頭と最後尾
	KLBlockHeader *pEmptyTop,*pEmptyTail;		///< 空きブロックの先頭と最後尾
	KLBlockHeader *pActiveTop,*pActiveTail;	///< 使用中ブロックの先頭と最後尾
	
}KLBlock;

bl KLBlock_Init( KLBlock* pKLibAlloc, size_t, bl );
void* KLBlock_Alloc( KLBlock* pKLibAlloc, size_t, char*, u32 );
void* KLBlock_Free( KLBlock* pKLibAlloc, void* );
void KLBlock_FreeAll( KLBlock* pKLibAlloc );
void KLBlock_Quit( KLBlock* pKLibAlloc );
size_t KLBlock_Sizeof();
size_t KLBlock_SizeofHeader();

// mallocと切り替えやすいようにklallocという前小文字名の関数にしています
#if KLBlock_DEBUG
	
	/// ライブラリヒープマネージャからメモリ確保(＋デバッグ情報付与)
	#define klalloc( szHeap ) KLBlock_Alloc( &klib.heap, szHeap, __FILE__, __LINE__ )
	
	/// 指定ヒープマネージャからメモリ確保(＋デバッグ情報付与)
	#define klallocx( pHeapManager, szHeap ) KLBlock_Alloc( pHeapManager, szHeap, __FILE__, __LINE__ )
	
#else
	
	/// ライブラリヒープマネージャからメモリ確保
	#define klalloc( szHeap ) KLBlock_Alloc( &globalHeapManager, szHeap, NULL, 0 )
	
	/// 指定ヒープマネージャからメモリ確保
	#define klallocx( pHeapManager, szHeap ) KLBlock_Alloc( pHeapManager, szHeap, NULL, 0 )
	
#endif
	
	/// ライブラリヒープマネージャ内のヒープを解放
	#define klfree( pAllocated ) KLBlock_Free( &klib.heap, pAllocated )
	
	/// 指定ヒープマネージャ内のヒープを解放
	#define klfreex( pHeapManager, pAllocated ) MEM_FreeProxy( pHeapManager, pAllocated )
	
	/// ライブラリヒープマネージャ内の全ヒープを解放
	#define klfreeall() KLBlock_FreeAll( &klib.heap )
	
	/// 指定ヒープマネージャ内のヒープを解放
	#define klfreeallx( pHeapManager ) KLBlock_FreeAll( pHeapManager )
	

#include "c_extern_close.h"
#endif