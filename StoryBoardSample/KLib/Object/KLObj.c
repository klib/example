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

#include "KLObj.h"
#include "KLibDefine.h"
#include <string.h>

//===================================================================
/**
 *	@brief	オブジェクトにイベントリスナーを設定
 *
 *	@param	pObj			オブジェクトのポインタ
 */
//===================================================================
void KLObj_SetListener( void* pObj, void(*pListener)(KLObj*,u32,void*) ){
	
	((KLObj*)pObj)->Listener = pListener;

}

//===================================================================
/**
 *	@brief	オブジェクトのメンバ変数初期化(ライブラリ内部用)
 *
 *	@param	pObj			オブジェクトのポインタ
 */
//===================================================================
void KLObj_SetupForKLib( void* pTargetObj ){
	
	//KLObj* pObj = (KLObj*)pTargetObj;
#if 0
	pObj->state		= (KLObjState)0;
	pObj->pContainer= 0;
	pObj->pParent	= 0;
	pObj->pPrev		= 0;
	pObj->pNext		= 0;
	pObj->Run		= 0;
	pObj->Unload	= 0;
#else
	
#endif
}

//===================================================================
/**
 *	@brief	オブジェクトワークの0クリア
 *
 *	@param	pObj			オブジェクトのポインタ
 *	@param	szWork			オブジェクトサイズ
 */
//===================================================================
void KLObj_Clear( void* pObj, size_t szWork ){
	
	size_t szCore = sizeof(KLObj);
	
	u8* p = (u8*)pObj;
	
	if( szCore<szWork )
	{
		memset( p+szCore, 0, szWork-szCore );
	}
	
}

//===================================================================
/**
 *	@brief	オブジェクトの初期化
 *
 *	@param	pContainer		どのコンテナに入れるかのポインタ
 *	@param	pExtendedObj	KLObjを構造体先頭に保持しているオブジェクトのポインタ
 *	@param	isAddToTop		マネージャ先頭に追加ならTRUE / FALSE
 */
//===================================================================
bl KLObj_Init( KLContainer* pContainer, void* pExtendedObj, bl isAddToTop ){
	
	KLObj* pObj = (KLObj*)pExtendedObj;
	
	if(pContainer && pObj)
	{
		KLObj_SetupForKLib(pObj);
		
		pObj->pContainer	= pContainer;
		pObj->pParent	= pContainer->pParent;
		
		if( !pContainer->pTop )
		{
			pContainer->pTop	= pObj;
			pContainer->pTail	= pObj;
			pObj->pNext		= NULL;
			pObj->pPrev		= NULL;
		}
		else
		{
			if( isAddToTop )
			{
				pObj->pPrev				= NULL;
				pObj->pNext				= pContainer->pTop;
				pContainer->pTop->pPrev	= pObj;
				pContainer->pTop			= pObj;
			}
			else
			{
				pObj->pNext				= NULL;
				pObj->pPrev				= pContainer->pTail;
				pContainer->pTail->pNext	= pObj;
				pContainer->pTail			= pObj;
			}
		}
		
		//pObj->uid		= pContainer->nextUid++;
		return TRUE;
		
	}
	
	return FALSE;
	
}

//===================================================================
/**
 *	@brief	事前確保タイプのコンテナにオブジェクトを確保（事前確保スタックからオブジェクトを取得）
 *
 *	@param	p			所属コンテナのポインタ
 *	@param	addToTop	先頭ならTRUE / 最後尾に追加ならFALSE
 */
//===================================================================
KLObj* KLObj_AllocStatic( KLContainer* pContainer, bl addToTop ){
	
	KLObj* pRet = NULL;
	
	if( pContainer->remain && pContainer->pFreeTop )
	{
		KLObj* pNext = pContainer->pFreeTop->pNext;
		KLObj* pPrev = pContainer->pFreeTop->pPrev;
		
		// 次の有効化
		pRet		= pContainer->pFreeTop;
		pContainer->pFreeTop	= pContainer->pFreeTop->pNext;
		
		if( pPrev )
		{
			pPrev->pNext = pNext;
		}
		
		if( pNext )
		{
			pNext->pPrev = pPrev;
		}
		
		KLObj_Init( pContainer, pRet, addToTop );
		pContainer->remain--;
		
		if( !pContainer->remain || !pContainer->pFreeTop )
		{
			pContainer->pFreeTop	= NULL;
			pContainer->pFreeTail	= NULL;
		}
		
	}
	else
	{
#ifdef DEBUG_KLIBOBJ
		KLLog("[ KLObj ] Object stack is full.\n");
#endif
	}
	
	return pRet;
	
}

//===================================================================
/**
 *	@brief	事前確保タイプのコンテナに所属するオブジェクトを解放(事前確保スタックにオブジェクトを返却)
 *
 *	@param	p		所属するコンテナのポインタ
 *	@param	pObj	解放するオブジェクト
 */
//===================================================================
void KLObj_FreeStatic( KLContainer* pContainer, KLObj* pObj ){
	
	if( pContainer->remain++ )
	{
		pObj->pPrev	= pContainer->pFreeTail;
		if(pContainer->pFreeTail)
		{
			pContainer->pFreeTail->pNext	= pObj;
		}
		
		pObj->pNext				= NULL;
		pContainer->pFreeTail	= pObj;
		
		if(!pContainer->pFreeTop)
		{
			pContainer->pFreeTop = pObj;
		}
		
	}
	else
	{
		
		pContainer->pFreeTop	=
		pContainer->pFreeTail	= pObj;
		pObj->pNext				= NULL;
		pObj->pPrev				= NULL;
		
	}

}

//===================================================================
/**
 *	@brief	オブジェクトの確保
 *
 *	@param	pContainer	所属するコンテナのポインタ
 *	@param	addToTop	先頭に追加するならTRUE / 末尾に追加ならFALSE
 */
//===================================================================
KLObj* KLObj_Alloc( KLContainer* pContainer, bl addToTop ){
	
	KLObj* pRet = NULL;
	
	if( pContainer->isStatic )
	{
		pRet = KLObj_AllocStatic( pContainer, addToTop );
		if( pRet )
		{
			memset( ((u8*)pRet)+sizeof(KLObj), 0, pContainer->szOneObject-sizeof(KLObj) );
		}
	}
	else
	{
		pRet = (KLObj*)Calloc( pContainer->szOneObject );
		
		if( pRet )
		{
			KLObj_Init( pContainer, pRet, addToTop );
		}
		
	}
	
	return pRet;
	
}

//===================================================================
/**
 *	@brief	オブジェクトを次のコンテナ実行を待たず即時解放<br>
 *			静的扱いなら静的キューに戻し、動的扱いならFreeする
 *
 *	@param	所属するコンテナのポインタ
 *	@param	解放するオブジェクトのポインタ
 */
//===================================================================
void KLObj_FreeNow( void* pObj ){
	
	KLContainer* p = ((KLObj*)pObj)->pContainer;
	
	if( p->isStatic )
	{
		KLObj_FreeStatic( p, (KLObj*)pObj );
	}
	else
	{
		sfree( pObj );
	}
	
}

//=====================================================================
/**
 *	@brief	指定オブジェクトを解放<br>
 *			※コール時に即解放ではなく、ここでは実行キューから外すのと<br>
 *			解放予約を行うのみなので注意
 *			実際に解放処理が行われるのは次回コンテナ実行時
 */
//=====================================================================
void KLObj_Free( void* pObj ){
	
	((KLObj*)pObj)->state = KLOBJ_RESERVED_FREE;
	
}

//=====================================================================
/**
 *	@brief	オブジェクト内のKLObj以外のワークをコピー
 *
 *	@param		pDest	コピー先のKLObjectを先頭に保持する構造体
 *	@param		pSrc	コピーさせる構造体
 *	@param		sz		KLObjを含むオブジェクトのサイズ(KLObjのサイズ以下だと機能しません)
 */
//=====================================================================
void KLObj_CopyWithoutCore( KLObj* pDest, KLObj* pSrc, size_t sz ){
	
	if( sz<=sizeof(KLObj) )
	{
		return;
	}
	memmove( ((u8*)pDest)+sizeof(KLObj), ((u8*)pSrc)+sizeof(KLObj), sz-sizeof(KLObj) );
	
}

//=====================================================================
/**
 *	@brief	オブジェクト実行関数の設定
 *
 *	@param	pObj	オブジェクトのポインタ
 *	@param	func	オブジェクト関数 / NULLで関数なしオブジェクトになる
 */
//=====================================================================
void KLObj_SetFunction( void* pObj, KLObjFunc func ){
	
	((KLObj*)pObj)->Run = func;
	
}

//=====================================================================
/**
 *	@brief	オブジェクト終了関数の設定
 *
 *	@param	pObj		操作するオブジェクトのポインタ
 *	@param	unloadFunc	オブジェクト終了関数 / NULLで終了関数なしオブジェクトになる
 */
//=====================================================================
void KLObj_SetUnload( void* pObj, KLObjFunc unloadFunc ){
	
	((KLObj*)pObj)->Unload = unloadFunc;
	
}

//=====================================================================
/**
 *	@brief	オブジェクトの実行関数と終了関数を同時に設定
 *
 *	@param	pObj			操作するオブジェクトのポインタ
 *	@param	func			実行関数 / NULLで何も処理しないオブジェクトとなる
 *	@param	unloadFunc		オブジェクト終了関数 / NULLで終了関数なしオブジェクトになる
 */
//=====================================================================
void KLObj_SetFunctions( void* pObj, KLObjFunc func, KLObjFunc unloadFunc ){

	KLObj_SetFunction( pObj, func );
	KLObj_SetUnload( pObj, unloadFunc );

}


