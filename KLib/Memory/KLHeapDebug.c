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

#include <mach/mach.h>
#include <mach/mach_host.h>
#include <string.h>
#include "KLHeapDebug.h"
//========================================================================
/*
 *	ヒープデバッガ
 *		リニアリストにmalloc情報を追加していき
 *		リーク数や位置を調べたり、まとめて解放したりするコードです。
 *		自動でガベージコレクションはしませんが、NSAutoReleasePoolっぽい事をします。
 *
 *		実験的なもので検証ができていません。
 *		使用非推奨のコードで将来はずす可能性あり
 */
//========================================================================

static bl	KLHeapDebug_AddList( KLHeapDebug* pHd, void* p, u32 uiSize, char* pchFile, u32 uiLine );
static void	KLHeapDebug_DelList( KLHeapDebug* pHd, KLList* pList, char* pchFilePath, u32 uiLine );
static void	KLHeapDebug_DelListAll( KLHeapDebug* pHd, char* pchFilePath, u32 uiLine );

static KLHeapDebug klheapdbg;

// F U N C T I O N
//==============================================================
	
	void* malloc_test(char* pFileName, u32 line, size_t sz){
		
		void* m = malloc(sz);
		
		KLLog("%s:%d(%p)\n", pFileName, line, m);
		
		return m;
		
	}

	//==================================================================
	/**
	 * 	@brief	ヒープデバッガの初期化
	 *
	 *	@param	pHd	ヒープデバッガのポインタ<br/>
	 *				NULLでKLibデフォルトのヒープデバッガを使用
	 */
	//==================================================================
	void KLHeapDebug_Init( KLHeapDebug* pHd ){
		
		if( !pHd )
		{
			pHd = &klheapdbg;
		}
		
		memset( pHd, 0, sizeof(KLHeapDebug) );
		
	}

	//==================================================================
	/**
	 * 	@brief	ヒープデバッガでメモリを確保
	 *
	 *	@param	pHd				ヒープデバッガのポインタ<br/>
	 *							NULLでKLibデフォルトのヒープデバッガを使用
	 *	@param	szTarget		確保するサイズ
	 *	@param	*pchFilePath	呼び出しファイル
	 *	@param	uiLine			呼び出し元の行
	 *
	 *	@return	確保したデータのポインタ
	 *
	 *	@warning	直接コールせずにマクロ経由で呼び出す事
	 *
	 *	@see	命名がおかしいですが、入力補助の候補にAllocが複数でると紛らわしいのであえてこうしてみました。
	 */
	//==================================================================
	void* KLHeapDebug_CoreAlloc( KLHeapDebug* pHd, size_t szTarget, char* pchFilePath, u32 uiLine  ){

		void* p = Malloc( szTarget );	
		
		if( !pHd )
		{
			pHd = &klheapdbg;
		}
		
		// NULLチェック
		if( !p ){
			
			// デバッグ用
			#if KL_DEBUG && KL_DEBUG_ALLOC
					
					KLLog("[ KLHeapDebug ] %s(%d) alloc failed.\n", pchFilePath, uiLine );

			#endif
					
			return NULL;
			
		}
		
		// Heap is initialized by 0
		memset( p, 0, szTarget );
		
		// Add heap list and allocated heap count increase.
		pHd->uiHeapNum++;
		KLHeapDebug_AddList( pHd, p, szTarget, pchFilePath, uiLine );
		
		// デバッグ用
		#if KL_DEBUG && KL_DEBUG_ALLOC

			KLLog("\n");
			KLLog("[ KLHeapDebug ] Alloc\n");
			KLLog("**********************************\n");
			KLLog("\tHeapDebug: %p\n", pHd );
			KLLog("\t%s(%d)\n", pchFilePath, uiLine );
			KLLog("\t[%p]\n",p);
			KLLog("\t%d byte\n", szTarget);
			KLLog("\t@%d\n", pHd->uiHeapNum);
			KLLog("***********************************\n");
			KLLog("\n");

		#endif
		
		return p;
		
	}



	//==================================================================
	/**
	 * 	@brief	ヒープデバッガ上の指定ヒープまたは全てのヒープを解放
	 *
	 *	@param	pHd			ヒープデバッガのポインタ<br/>
	 *						NULLでKLibデフォルトのヒープデバッガを使用
	 *	@param	p			解放するヒープのポインタ<br/>
	 *						指定ヒープデバッガ上のすべてのヒープを解放するならNULLを指定
	 *	@param	pchFilePath	呼び出し元のファイルパス
	 *	@param	uiLine		呼び出し元の行数
	 *
	 *	@warning	直接コールせずにマクロ経由で呼び出す事
	 *
	 *	@see	命名がおかしいですが、入力補助の候補にFreeが複数でると紛らわしいのであえてこうしてみました。
	 */
	//==================================================================
	void KLHeapDebug_CoreFree( KLHeapDebug* pHd, void* p, char* pchFilePath, u32 uiLine ){

		if( !pHd )
		{
			pHd = &klheapdbg;
		}
		
		if( p )
		{
			
			KLList*				pList	= NULL;
			KLibDebugHeapInfo*	pInfo	= NULL;
			
			pList	= KLHeapDebug_SearchHeap( pHd, p );
			
			if( pList )
			{
				
				pInfo	= (KLibDebugHeapInfo*)pList->pData;
				
				if( pInfo )
				{
					
					KLHeapDebug_DelList( pHd, pList, pchFilePath, uiLine);
					
				}
				else
				{
					
					#if KL_DEBUG && KL_DEBUG_ALLOC
						
					KLLog("\n");
					KLLog("[ KLHeapDebug ] !!! WARNING !!!\n");
					KLLog("--------------\n");
					KLLog("\tNot found heap info.\n");
					KLLog("\tReleased already or invalid parameter..\n");
					KLLog("\tFree info:\n");
					KLLog("\t%s(%d)\n", pchFilePath, uiLine );
					KLLog("--------------\n");
					KLLog("\n");
							
					#else
						
					uiLine		= uiLine;
					pchFilePath	= pchFilePath;

					#endif
				}
				
				return;
				
			}

			if( pInfo )
			{
				
				free( pInfo );
				
			}
			pInfo = NULL;
			
			if( pList )
			{
				
				free( pList );
				
			}
			pList=NULL;
			
		}
		else
		{
			
			KLHeapDebug_DelListAll( pHd, pchFilePath, uiLine );
			
		}
		
	}




//====================================================================
/**
 *	@brief	ヒープデバッガにヒープを追加
 *
 *	@param	pHd			ヒープデバッガのポインタ
 *	@param	p			追加するヒープのポインタ
 *	@param	uiSize		確保するサイズ
 *	@param	*pchFile	呼び出し元のファイルパス
 *	@param	uiLine		呼び出し元の行数
 *
 *	@return	追加が正常に完了した場合TRUE / 失敗時FALSE
 */
//====================================================================
static bl KLHeapDebug_AddList( KLHeapDebug* pHd, void* p, u32 uiSize, char* pchFile, u32 uiLine ){
	
	KLList*				pList = NULL;
	KLibDebugHeapInfo*	pInfo = NULL;
	
	if( !pHd )
	{
		pHd = &klheapdbg;
	}
	
	pList	= (KLList*)Malloc( sizeof(KLList) );
	
	if( !pList )
	{
		
		return FALSE;
		
	}
	
	memset( (void*)pList, 0, sizeof(KLList) );
	pInfo = (KLibDebugHeapInfo*)Malloc( sizeof(KLibDebugHeapInfo) );
	
	if( !pInfo )
	{
		
		Free( pList );
		return FALSE;
		
	}
	
	memset( (void*)pInfo, 0, sizeof(KLibDebugHeapInfo) );
	
	pInfo->pAddress	= p;
	pInfo->size		= uiSize;
	pInfo->id		= (u32)pHd;
	
#if KL_DEBUG
		
	memcpy( (void*)pInfo->strFile, (void*)pchFile, sizeof(pInfo->strFile) );
	pInfo->uiLine	= uiLine;
	
#else
	
#endif

	// 使用量の更新
	pHd->uiTotalSize	+= uiSize + sizeof(KLibDebugHeapInfo) + sizeof(KLList);
	
	// ピーク時の最大使用量を更新
	if( pHd->uiMaxSize < pHd->uiTotalSize )
	{
		pHd->uiMaxSize = pHd->uiTotalSize;
	}
	
	// リストに追加
	pList->pData = (void*)pInfo;

	// リストの最後尾が存在する
	if( pList->pTail )
	{
		
		pList->pPrev		= pHd->pTail;
		pHd->pTail->pNext	= pList;
		pHd->pTail			= pList;
		
	// リストを作成
	}
	else
	{
		
		pHd->pTop	= pList;
		pHd->pTail	= pList;
		
	}
	
	return TRUE;
	
}



//====================================================================
/**
 *	@brief	ヒープデバッガから指定リストを削除
 *
 *	@param	pHd			ヒープデバッガのポインタ
 *	@param	pList		削除するリストのポインタ
 *	@param	uiSize		確保するサイズ
 *	@param	*pchFile	呼び出し元のファイルパス
 *	@param	uiLine		呼び出し元の行数
 *
 *	@return	追加が正常に完了した場合TRUE / 失敗時FALSE
 */
//====================================================================
static void KLHeapDebug_DelList( KLHeapDebug* pHd, KLList* pList, char* pchFilePath, u32 uiLine ){
	
	KLibDebugHeapInfo* pInfo = (KLibDebugHeapInfo*)pList->pData;
	
	if( !pList )
	{
		
		return;
	
	}
	
	if( !pHd )
	{
		pHd = &klheapdbg;
	}
	
#if ( KL_DEBUG && KL_DEBUG_ALLOC )

	KLLog("\n");
	KLLog("[ KLHeapDebug ] Free\n");
	KLLog("#####################################\n");
	KLLog("\tAllocated from...\n");
	KLLog("\t%s(%d)\n", pInfo->achFile, pInfo->uiLine );
	KLLog("\tAddress[%p]\n", pInfo->pAddress);
	KLLog("\tHID[%d]\n", pInfo->id);			
	KLLog("\n");
	KLLog("\tReleased from:\n");
	KLLog("\t%s(%d)\n", pchFilePath, uiLine );
	KLLog("\t@%d\n", (pHd->uiHeapNum-1) );
	KLLog("#####################################\n");
	KLLog("\n");

#endif			
	
	if( pList->pPrev != NULL )
	{
		pList->pPrev->pNext = pList->pNext;
	}
	
	if( pList->pNext != NULL )
	{
		pList->pNext->pPrev = pList->pPrev;
	}
	
	if( pList == pHd->pTop )
	{
		if( pHd->uiHeapNum<=1 )
		{
			pHd->pTop	= NULL;
		}
		else
		{
			pHd->pTop = pHd->pTop->pNext;
		}
	}
	
	if( pList == pHd->pTail )
	{
		if( pHd->uiHeapNum<=1 )
		{
			pHd->pTail = NULL;
		}
		else
		{
			pHd->pTail = pHd->pTail->pPrev;
		}
	}
	
	pHd->uiTotalSize -= pInfo->size + sizeof( KLibDebugHeapInfo ) + sizeof( KLList );
	pHd->uiHeapNum--;
	Free( pInfo->pAddress );
	pInfo->pAddress = NULL;
	
}



//====================================================================
/**
 *	@brief	指定ヒープデバッガの保持するリストを全て削除
 *
 *	@param	pHd			ヒープデバッガのポインタ
 *	@param	*pchFile	呼び出し元のファイルパス
 *	@param	uiLine		呼び出し元の行数
 *			
 */
//====================================================================
static void KLHeapDebug_DelListAll( KLHeapDebug* pHd, char* pchFilePath, u32 uiLine ){
	
	KLList*				pNow;
	KLList*				pNext;
	KLibDebugHeapInfo*	pInfo;
	
	if( !pHd )
	{
		pHd = &klheapdbg;
	}
	
	for( pNow = pHd->pTail; pNow; pNow=pNext )
	{
		pInfo = (KLibDebugHeapInfo*)(pNow->pData);
		
		pNext = pNow->pPrev;
		
		KLHeapDebug_DelList( pHd, pNow, pchFilePath, uiLine );
			
		if( pInfo )
		{
			
			Free( pInfo );
			
		}
		
		pInfo = NULL;
		
		if( pNow ){
			
			Free( pNow );
			
		}
		
		pNow=NULL;				
		
	}
	
}	


//====================================================================
/**
 *	@brief	指定ヒープデバッガから指定ヒープを検索
 *
 *	@param	pHd		ヒープデバッガのポインタ
 *	@param	p		探すヒープのポインタ
 *
 *	@return	見つけた場合はそのリストのポインタ / なければNULL
 */
//====================================================================
struct _KLList *KLHeapDebug_SearchHeap( KLHeapDebug* pHd, void* p ){

	KLList* pNow;
	KLList* pNext;
	
	if( !p )
	{
		return NULL;
	}
	
	if( !pHd )
	{
		pHd = &klheapdbg;
	}
	
	for( pNow=pHd->pTail; pNow; pNow=pNext)
	{
		pNext = pNow->pPrev;	
		
		if( pNow->pData && (((KLibDebugHeapInfo*)pNow->pData)->pAddress == p) )
		{
			return pNow;
		}
		
	}
	
	return NULL;
	
}



//====================================================================
/**
 *	@brief	システムの空きメモリサイズを返す
 *
 *	@return	システム空きメモリサイズ<br/>
 *			連続した領域とは限らず、断片の総数の可能性もあるので注意
 */
//====================================================================
size_t KLHeapDebug_GetRemain(){

	mach_port_t				hostport;
    mach_msg_type_number_t	szHost;
    vm_size_t				szPage;
	vm_statistics_data_t	stat;
	
	hostport	= mach_host_self();
    szHost		= sizeof(vm_statistics_data_t) / sizeof(integer_t);
    
	host_page_size( hostport, &szPage );
    
    if( host_statistics( hostport, HOST_VM_INFO, (host_info_t)&stat, &szHost) != KERN_SUCCESS )
	{
        KLLog("[KLHeapDebug] failed GetRemain.");
        return 0;
    }
    
    return stat.free_count * szPage;
	
}



//====================================================================
/**
 *	@brief	指定ヒープデバッガのヒープ数を取得
 *
 *	@param	pHd	ヒープデバッガ
 *
 *	@return	ヒープ数
 */
//====================================================================
size_t KLHeapDebug_Len( KLHeapDebug* pHd ){
	
	KLList*	pList	= NULL;
	KLList*	pNext	= NULL;
	size_t	result	= 0;
	
	for( pList = pHd->pTail; pList; pList=pNext )
	{
		pNext = pList->pNext;
		result++;
	}
	
	return result;
	
}



//====================================================================
/**
 *	@brief	ヒープリスト情報を出力
 *
 *	@param	pHd	ヒープデバッガ
 */
//====================================================================
void KLHeapDebug_OutputList( KLHeapDebug* pHd ){
	
#if ( KL_DEBUG && KL_DEBUG_MM )
	
	int					i		= 0;
	KLList*				pList	= NULL;
	KLibDebugHeapInfo*	pInfo	= NULL;
	
	if( !pHd )
	{
		pHd = &klheapdbg;
	}
	
	pList = pHd->pTop;
	KLLog("\n-----------------\n");

	while( pList )
	{
		
		pInfo = (KLibDebugHeapInfo*)pList->pData;
		
		KLLog("Allocated[%d] from...\n", i);
		KLLog("%s(%d)\n", pInfo->strFile, pInfo->uiLine );
		KLLog("Address[%p]\n", pInfo->pAddress);
		KLLog("ID[%d]\n\n", pInfo->id);
		
		pList = pList->pNext;
		i++;
		
	}
	
	KLLog("Total remain[%d]\n\n\n", pHd->uiHeapNum );
	KLLog("Remain: %zd\n\n", KLHeapDebug_GetRemain() );			

#endif
	
}


