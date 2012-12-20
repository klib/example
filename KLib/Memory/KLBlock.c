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

#include "KLBlock.h"
#include "KLib.h"
//========================================================================
/*
 *		最初に指定量のバイト配列をmallocし
 *		その後はブロック毎にその配列の一部をキャストして擬似確保する
 *
 *		実験的なもので検証ができていません。
 *		使用非推奨のコードで将来はずす可能性あり
 */
//========================================================================

/// ブロックヘッダ数を増加
static inline bl KLBlock_IncBlockNum( KLBlock* pHm ){
	
	return pHm->nBlock++;
	
}

/// ブロックヘッダ数を減少
static inline bl KLBlock_DecBlockNum( KLBlock* pHm ){
	
	if(pHm->nBlock)
	{
	
		pHm->nBlock--;
		
	}
	else
	{
	
		KLLog("[ KLBlock ] ブロック数計算が誤っている可能性があります.\n");
	
	}
	
	return pHm->nBlock;
	
}

/// ブロックヘッダのサイズを返す
inline size_t KLBlock_SizeofHeader(){
	
	return sizeof(KLBlockHeader);
	
}

/// アラインメントサイズで切上げたサイズを返す
static inline size_t KLBlock_RoundUpAlign( size_t offset ){
	
	return (offset / KLIBALLOC_ALIGN_BYTES + 1) * KLIBALLOC_ALIGN_BYTES;
	
}

/// アラインメントサイズで切下げたサイズを返す
static inline size_t KLBlock_RoundDownAlign( size_t offset ){
	
	return offset / KLIBALLOC_ALIGN_BYTES * KLIBALLOC_ALIGN_BYTES;
	
}

/// ブロックヘッダを空状態で初期化
static void KLBlock_InitBlockHeader( KLBlockHeader* pBh, size_t szHeap ){
	
	pBh->state			= KLBLOCK_STATE_EMPTY;
	pBh->szHeap			= szHeap;
	pBh->szBlock		= szHeap+KLBlock_SizeofHeader();
	pBh->pBlockHead		= 0;
	pBh->pHeapHead		= 0;
	pBh->pPrev			=
	pBh->pNext			= NULL;
	
}

/// 指定オフセットに空ブロックを作成(前後のブロック連結までは行わない)
static KLBlockHeader* KLBlock_SetEmptyBlock( KLBlock* pHm, size_t offsetOnHeap, size_t szHeap ){
	
	KLBlockHeader bh;
	
	if( pHm->szTotal < offsetOnHeap+szHeap+KLBlock_SizeofHeader() ){
		
		KLLog("[ KLBlock ] 空ブロック作成するオフセット＋サイズがオーバーしていますw.\n");
		return NULL;
		
	}
	
	KLBlock_InitBlockHeader( &bh, szHeap );
	bh.pBlockHead		= &pHm->pHeap[offsetOnHeap];
	bh.pHeapHead		= &pHm->pHeap[offsetOnHeap+KLBlock_SizeofHeader()];
	bh.pPrev			= NULL;
	bh.pNext			= NULL;
	
	*((KLBlockHeader*)&pHm->pHeap[offsetOnHeap]) = bh;
	
	return (KLBlockHeader*)&pHm->pHeap[offsetOnHeap];
	
}

/// 該当サイズが入る空ブロックを返す /　なければNULL
static KLBlockHeader* KLBlock_GetEmptyBlock( KLBlock* pHm, size_t szSearchHeap ){
	
	KLBlockHeader* pBh = pHm->pTop;
		
	while( pBh )
	{
		if(pBh->state==KLBLOCK_STATE_EMPTY)
		{
			// 既にブロックヘッダ１つ分を排除したヒープ領域サイズだが
			// 分割用ブロックヘッダサイズがこの中に更に増えるため
			// もう1つ分のサイズをヒープ領域から引いた後に換算する
			// (現在のEmptyをEmptyとActiveの２つに分割しても1byte以上のEmptyが残る場合)
			if( pBh->szHeap-KLBlock_SizeofHeader()-1>=szSearchHeap+KLBlock_SizeofHeader() )
			{
				return pBh;
			
			// ブロックヘッダを除いたら足りる時もとりあえず正常とする
			// この場合、Alloc内でも判断し分割しないようにする必要がある
			// (EmptyをActiveに変更すれば入る場合)
			}
			else if( pBh->szHeap>=szSearchHeap )
			{
				return pBh;
			}
			else
			{
				KLLog("[ KLBlock ] 空きを発見しましたが、サイズが足りませんでした\n必要[%09d byte]\n発見[%09d byte]\n\n", (u32)szSearchHeap, (u32)pBh->szHeap );
				return NULL;
			}
		
		}
		
		pBh = pBh->pNext;
	
	}
	
	return NULL;
	
}

/// マージ可能なブロックかを判定
static inline bl KLBlock_CanMerge( KLBlockHeader* pBhFront, KLBlockHeader* pBhBack ){
	
	return	pBhFront &&
			pBhBack  &&
			( &pBhFront<&pBhBack ) &&
			pBhFront->state == KLBLOCK_STATE_EMPTY &&
			pBhBack->state  == KLBLOCK_STATE_EMPTY;
	
}

/// 可能な場合に限り前後のブロックを指定してマージ
static bl KLBlock_MergeBlocks( KLBlock* pHm, KLBlockHeader* pBhFront, KLBlockHeader* pBhBack ){
	
	if( KLBlock_CanMerge( pBhFront, pBhBack ) )
	{
	
		pBhFront->pNext		= pBhBack->pNext;
		pBhFront->szBlock	+= pBhBack->szBlock;
		pBhFront->szHeap	+= pBhBack->szBlock;
		if( pBhBack->pNext )
		{
		
			pBhBack->pNext->pPrev = pBhFront;
		
		}
		
		pBhBack->pPrev =
		pBhBack->pNext = NULL;
		
		if( pHm->pTail==pBhBack )
		{
			
			pHm->pTail = pBhFront;
			
		}
		
		KLBlock_DecBlockNum(pHm);
		pHm->szRemain += KLBlock_SizeofHeader();
		
		return TRUE;
		
	}
	
	return FALSE;
	
}

/// ブロックの確保
static void* KLBlock_AllocBlock( KLBlock* pHm, KLBlockHeader* pBh, size_t szHeap ){
	
	void*			pRet	= NULL;
	KLBlockHeader*	pBhNew	= NULL;
	
	szHeap = KLBlock_RoundUpAlign(szHeap);
	
	// 確保後のサイズのあまりが大きければ空ブロックを新設して分割する
	if( pBh->state==KLBLOCK_STATE_EMPTY )
	{
	
		if( pBh->szHeap-szHeap-KLBlock_SizeofHeader()-1 > 0 )
		{
		
			// トップブロックor前方ブロックが有効なら分割後は[確保][空]という順序になる
			if( pBh==pHm->pTop || !pBh->pPrev || (pBh->pPrev && pBh->pPrev->state!=KLBLOCK_STATE_EMPTY) )
			{
				
				pBhNew				= KLBlock_SetEmptyBlock( pHm, (pBh->pHeapHead-pHm->pHeap)+szHeap, pBh->szHeap-szHeap-KLBlock_SizeofHeader() );
				if( pBhNew )
				{
					pBhNew->pPrev		= pBh;
					pBhNew->pNext		= pBh->pNext;
					
					pBh->state			= KLBLOCK_STATE_ACTIVE;
					pBh->szBlock		= szHeap+KLBlock_SizeofHeader();
					pBh->szHeap			= szHeap;
					
					pHm->szRemain		-= pBh->szBlock;
					
					if( pBh->pNext )
					{
					
						pBh->pNext->pPrev	= pBhNew;
						
					}
					pBh->pNext			= pBhNew;
					
					if( pHm->pTail==pBh )
					{
						
						pHm->pTail = pBhNew;
						
					}
					
					pRet = pBh->pHeapHead;
					
					KLBlock_IncBlockNum(pHm);
					
					// 可能なら後方マージして空きエリアを出来るだけ広くする
					KLBlock_MergeBlocks( pHm, pBhNew, pBhNew->pNext );
				}
				else
				{
					KLLog("[ KLBlock ] KLBlock_SetEmptyBlockに失敗しました.\n");
					pRet = NULL;
				}
				
			// 前方マージが可能なら新設ブロックを確保ブロックの前に作成してフラグメントを極力避ける
			}
			else if( pBh->pPrev && pBh->pPrev->state==KLBLOCK_STATE_EMPTY )
			{
				// 一度前方空きブロックとマージする
				KLBlock_MergeBlocks( pHm, pBh->pPrev, pBh );
				
				if( !pBh || !pBh->pPrev )
				{
					KLLog("[ KLBlock ] マージに失敗しました\n");
					return NULL;
				}
				
				// マージしたブロック後方からブロック確保する
				pBhNew = KLBlock_SetEmptyBlock(
							pHm,
							(pBh->pPrev->pBlockHead-pHm->pHeap)+pBh->pPrev->szBlock-szHeap-KLBlock_SizeofHeader(),
							szHeap );
				if( pBhNew )
				{
					pBhNew->pPrev		= pBh->pPrev;
					pBhNew->pNext		= pBh->pPrev->pNext;
					
					// そのブロックを確保する
					pBhNew->state		= KLBLOCK_STATE_ACTIVE;
					
					if( pBhNew->pNext==NULL )
					{
						pHm->pTail = pBhNew;
					}
					
					// 確保した差分に空きエリア情報を調整する
					pBh->pPrev->szHeap	-= szHeap+KLBlock_SizeofHeader();
					pBh->pPrev->szBlock	-= szHeap+KLBlock_SizeofHeader();
					pHm->szRemain		-= szHeap+KLBlock_SizeofHeader();
					
					pRet = pBhNew->pHeapHead;
						
					KLBlock_IncBlockNum(pHm);
				
				}
				else
				{
					KLLog("[ KLBlock ] KLBlock_SetEmptyBlockに失敗しました(2).\n");
					pRet = NULL;
				}
				
			}
			else
			{
				KLLog("[ KLBlock ] 指定ブロックに確保する為の空き容量がありませんでした\n");
				pRet = NULL;
			}
			
		// ブロックヘッダを除いたら足りる時もとりあえず正常とする
		// この場合、Alloc内でも判断し分割しないようにする必要がある
		}
		else if( pBh->szHeap>=szHeap )
		{
			pBh->state		=	KLBLOCK_STATE_ACTIVE;
			pRet			=	pBh->pHeapHead;
			pHm->szRemain	-=	pBh->szBlock;
		}
		else
		{
			pRet = NULL;
		}

	}
	else
	{
		pRet = NULL;
	}		

	return pRet;
	
}

/// ブロックヒープエリアの先頭に指定アドレスを持つブロックを検索し返す
static KLBlockHeader* KLBlock_SearchActive( KLBlock* pHm, void* pHeap ){
	
	#if 0 /* 地道に検索版 */
		KLBlockHeader* pBh = pHm->pTop;
			
		while( pBh )
		{
			// 指定アドレスのヒープかチェック
			if( pBh->state==KLBLOCK_STATE_ACTIVE && pBh->pHeapHead==pHeap )
			{
				return pBh;
			}
			
			pBh = pBh->pNext;
		
		}
		
	#else	/* アドレス逆引き版 */
		
		if( pHeap )
		{
			KLBlockHeader* pBh = (KLBlockHeader*)(((u8*)pHeap)-KLBlock_SizeofHeader());
			
			if(pBh->state==KLBLOCK_STATE_ACTIVE)
			{
				return pBh;
			}
		}
		
	#endif
		return NULL;
	
}

/// ヒープマネージャの初期化( szHeap0でリセット )
bl KLBlock_Init( KLBlock* pAlloc, size_t szHeapMax, bl isClearing ){
	
	if( !pAlloc )
	{
		pAlloc = &klib.heap;
	}
	
	szHeapMax = KLBlock_RoundUpAlign( szHeapMax+KLBlock_SizeofHeader() );
	
	// サイズ指定が0の時は再初期化
	if( szHeapMax==0 ){
		
		KLLog("[ KLBlock ] すべてのヒープを解放しマネージャを再初期化します\n");
		
		// サイズ
		pAlloc->nBlock		= 0;
		pAlloc->szRemain	= pAlloc->szTotal;
		pAlloc->pTop		=
		pAlloc->pTail		= NULL;
	
	}else{
	
		// サイズ
		pAlloc->nBlock			= 0;
		pAlloc->nActiveBlock	= 0;
		pAlloc->nEmptyBlock		= 0;
		pAlloc->szTotal			= szHeapMax;
		pAlloc->szRemain		= pAlloc->szTotal;
		pAlloc->pTop			= NULL;
		pAlloc->pTail			= NULL;
		
		// 確保
		pAlloc->pHeap = (u8*)Malloc( pAlloc->szRemain );
		
		// ヒープチェック
		if( !pAlloc->pHeap ){
			
			KLLog("[ KLBlock ] マネージャ内ヒープがNULLです\n" );
			return FALSE;
			
		}
		
		// クリアするか
		if( isClearing ){
			
			memset( pAlloc->pHeap, 0, pAlloc->szRemain );
			
		}
		
	}
	
	// ブロックの初期化
	pAlloc->pTop		= (KLBlockHeader*)pAlloc->pHeap;
	pAlloc->pTail		= (KLBlockHeader*)pAlloc->pHeap;
	pAlloc->pEmptyTop	= (KLBlockHeader*)pAlloc->pHeap;
	pAlloc->pEmptyTail	= (KLBlockHeader*)pAlloc->pHeap;
	pAlloc->pActiveTop	= NULL;
	pAlloc->pActiveTail= NULL;
	KLBlock_SetEmptyBlock( pAlloc, 0, pAlloc->szTotal-KLBlock_SizeofHeader() );
	KLBlock_IncBlockNum(pAlloc);
	
	return TRUE;
	
}

/// アロケータからメモリを確保
void* KLBlock_AllocProxy( KLBlock* pHm, size_t szHeap, char* pFileName, u32 line ){
	
	void*				pRet	= NULL;
	KLBlockHeader*	pBh		= NULL;
	//size_t				offset	= 0;
	
	if( !pHm ){
		
		pHm = &klib.heap;
		
	}
	
	szHeap = KLBlock_RoundUpAlign(szHeap);
	
	pBh = KLBlock_GetEmptyBlock( pHm, szHeap );
	
	if( !pBh ){
		
		KLLog("[ KLBlock ] 確保可能な空きブロックが見つかりませんでした\n");
		return NULL;
		
	}
	
	pRet = KLBlock_AllocBlock( pHm, pBh, szHeap );
	
	#if KLBlock_DEBUG
	
		if( pRet ){
			
			pBh->filename[0] = 0;
			
			if( pFileName ){
			
				memcpy( pBh->filename, pFileName, KLBlock_FILENAME_LEN );
				
			}
			pBh->line		= line;
			
		}
			
	#endif
	
	return pRet;
	
}

/// アロケータで確保したメモリを解放
void* KLBlock_FreeProxy( KLBlock* pHm, void* p ){
	
	KLBlockHeader* pBh = NULL;
	
	if( !pHm ){
		
		pHm = &klib.heap;
		
	}
	
	pBh = KLBlock_SearchActive( pHm, p );
	
	if( !pBh ){
		
		KLLog( "[ KLBlock ] ( %p )はリスト内に存在しません\n", p );
		
	}else{
		
		pBh->state		= KLBLOCK_STATE_EMPTY;
		pHm->szRemain	+=pBh->szHeap;
		
		// 前後の空きとマージする
		if( pBh->pNext ){
			
			KLBlock_MergeBlocks( pHm, pBh, pBh->pNext );
			
		}
		
		if( pBh->pPrev ){
		
			KLBlock_MergeBlocks( pHm, pBh->pPrev, pBh );
			
		}
		
	}
	
	return NULL;
	
}

/// アロケータで確保したメモリを解放
void KLBlock_FreeProxyAll( KLBlock* pHm ){
	
	if( !pHm ){
		
		pHm = &klib.heap;
		
	}
	
	// 先頭ブロックが一つだけ設定された状態にするだけで良い
	KLBlock_Init( pHm, 0, FALSE );
	
}

/// ヒープマネージャを終了させる
void KLBlock_Quit( KLBlock* pHm ){
	
	if( !pHm ){
		
		pHm = &klib.heap;
		
	}
	
	// 内部でAllocしている箇所は大元のヒープだけなので
	// それをフリーするのみ
	if(pHm->pHeap){
		
		free(pHm->pHeap);
		pHm->pHeap = NULL;
		
	}
	
}

