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

#include "KLibDefine.h"
#include "KLObj.h"
#include <string.h>

//===================================================================
/**
 *	@brief	指定インデクスにあるオブジェクトを取得
 *
 *	@param	pContainer	コンテナのポインタ
 *	@param	index		インデクス
 */
//===================================================================
KLObj* KLContainer_GetObject( KLContainer* pContainer, u32 index ){
	
	KLObj* pObj = pContainer->pTop;
	
	for( int i=0; i<pContainer->num; ++i )
	{
		if( i==index )
		{
			return pObj;
		}
		
		if( pObj->pNext )
		{
			pObj = pObj->pNext;
		}
		else
		{
			return NULL;
		}
	}
	
	return pObj;
	
}

//===================================================================
/**
 *	@brief	オブジェクトマネージャの初期化(中で作成しないので呼び出し元管理のポインタを渡します)
 *
 *	@param	pContainer		初期化するコンテナのポインタ
 *	@param	szOneObject		詰め込むオブジェクトのサイズ
 *	@param	nReserveObject	事前にいくつ分のバッファを確保しておくか(0だと動的生成モード)
 *	@param	pParentObj		あるオブジェクトを親にしたい場合はそのポインタ / 不要ならNULL
 */
//===================================================================
bl KLContainer_Init( KLContainer* pContainer, size_t szOneObject, size_t nObject, void* pParentObj ){
	
	klmemclear(&pContainer->core, KLObj);
	pContainer->pTop		=
	pContainer->pTail		=
	pContainer->pFreeTop	=
	pContainer->pFreeTail	=
	pContainer->pFree		=
	pContainer->pParent		= NULL;
	pContainer->num			=
	pContainer->remain		= 0;
	pContainer->isStatic	= nObject!=0;
	
	// alignment
	if( szOneObject%4 )
	{
		// 例：1,2,3,→4   4,5,6,7→8
		szOneObject = (szOneObject/4+1)*4;
	}
	
	pContainer->szOneObject	= szOneObject;
	pContainer->pParent		= pParentObj;

	//KLLog("[ ObjSize ] %d", p->szOneObject);

	if( pContainer->isStatic )
	{
		
		int i				= 0;
		pContainer->remain		= nObject;
		pContainer->pFree		= (KLObj*)Calloc( szOneObject*pContainer->remain );
		if( !pContainer->pFree )
		{
			
			sfree(pContainer);
			return FALSE;
			
		}
		
		for( i=0; i<pContainer->remain; ++i )
		{
			KLObj* pFree = KLObj_GetIndex( pContainer, pContainer->pFree, i );
			
			if( !i )
			{
				pFree->pPrev	= NULL;
			}
			else
			{
				pFree->pPrev	= KLObj_GetIndex( pContainer, pContainer->pFree, i-1 );
			}
			
			if( i == pContainer->remain-1 )
			{
				pFree->pNext	= NULL;
			}
			else
			{
				pFree->pNext	= KLObj_GetIndex( pContainer, pContainer->pFree, i+1 );
			}
		
		}
		
		pContainer->pFreeTop	= &pContainer->pFree[0];
		pContainer->pFreeTail	= KLObj_GetIndex( pContainer, pContainer->pFree, pContainer->remain-1 );
		
	}
	
	return TRUE;
	
}

//===================================================================
/**
 *	@brief	コンテナのアンロード
 *
 *	@param	コンテナのポインタ
 */
//===================================================================
KLContainer* KLContainer_Unload( KLContainer* pContainer, void* pParams ){
	
	KLObj*	pNext	= NULL;
	KLObj*	pObj	= pContainer->pTail;
	
	for( ; pObj; pObj=pNext )
	{
		if( pObj->Unload )
		{
			pObj->Unload( pObj, pParams );
		}
		
		pNext = pObj->pPrev;
		
		if( pNext )
		{
			pNext->pNext = NULL;
		}
		
		KLObj_FreeNow(pObj);
		
	}
	
	sfree(pContainer->pFree);
	
	return NULL;
	
}



//=====================================================================
/**
 *	@brief	指定コンテナに所属するオブジェクトすべてにイベントを発行
 * 			ただし、リスナーを設定していないオブジェクトには無効
 */
//=====================================================================
void KLContainer_Send( KLContainer* pContainer, u32 eventid, void* pParams ){
	
	KLObj*	pObj	= NULL;
	KLObj*	pNext	= NULL;
	
	if( pContainer )
	{
		pObj = pContainer->pTop;
		
		while( pObj )
		{
			pNext = pObj->pNext;
			
			if( pObj->state == KLOBJ_RUN )
			{
					
				if( pObj->Listener )
				{
					pObj->Listener( pObj, eventid, pParams );
				}
				
			}
			
			pObj = pNext;
			
		}
		
	}
	
}



//=====================================================================
/**
 *	@brief	指定コンテナに所属するオブジェクトの順次実行
 */
//=====================================================================
void KLContainer_Execute( KLContainer* pContainer, void* pParams ){
	
	KLObj*	pObj	= NULL;
	KLObj*	pNext	= NULL;
	
	if( pContainer )
	{
		pObj = pContainer->pTop;
		
		while( pObj )
		{
			pNext = pObj->pNext;
			
			switch( pObj->state )
			{
				case KLOBJ_RUN:
				{
					
					if( pObj->Run )
						pObj->Run( pObj, pParams );
					
				}
				break;
					
				case KLOBJ_RESERVED_FREE:
				{
					
					KLContainer_ExecuteFree( pContainer, pObj, pParams );
					
				}
				break;
					
				default:
				{
				
				}
				break;
			}
			
			pObj = pNext;
			
		}
		
	}
	
}


//=====================================================================
/**
 *	@brief	指定コンテナに所属する全てのオブジェクトを削除<br>
 *			※コール時に即削除ではなく、ここでは予約を行った後に<br>
 *			次の実行でコンテナの処理が回った際に削除する
 *
 *	@param	pContainer		マネージャのポインタ
 */
//=====================================================================
void KLContainer_FreeAll( KLContainer* pContainer ){

	KLObj* pObj = NULL;
	
	if( pContainer )
	{
		for( pObj = pContainer->pTop; pObj; pObj = pObj->pNext )
		{
			KLObj_Free(pObj);
		}
	}
	
}

//===================================================================
/**
 *	@brief	オブジェクトの追加(ライブラリ内使用)
 *
 *	@param	pContainer	所属コンテナのポインタ
 *	@param	initFunc	初期化で呼び出す関数のポインタ
 *	@param	pParams		パラメータ(NULL可)
 *	@param	addToTop	先頭に追加するならTRUE
 *
 *	@return	成功時は追加されたオブジェクトのポインタ / 失敗時はNULL
 */
//===================================================================
static KLObj* KLContainer_AddCommon( KLContainer* pContainer, KLObjFunc initFunc, void* pParams, bl addToTop ){
	
	KLObj*	pObj	= NULL;
	
	
	if(  !pContainer )
	{
		return NULL;
	}
	
	pObj = KLObj_Alloc( pContainer, addToTop );
	
	if( !pObj )
	{
		return NULL;
	}
	
	pContainer->num++;
	
	// 初期化関数の実行
	if( initFunc )
	{
		initFunc( pObj, pParams );
	}
	
	pObj->state		= KLOBJ_RUN;
	pObj->pContainer= pContainer;
	
	return pObj;
	
}

//===================================================================
/**
 *	@brief	指定コンテナにオブジェクトを追加(リスト先頭)
 *
 *	@param	pContainer	所属コンテナのポインタ
 *	@param	initFunc	初期化で呼び出す関数のポインタ
 *	@param	pParams		パラメータ(NULL可)
 *
 *	@return	成功時は追加されたオブジェクトのポインタ / 失敗時はNULL
 */
//===================================================================
KLObj* KLContainer_AddTop( KLContainer* pContainer, KLObjFunc initFunc, void* pParams ){
	
	return KLContainer_AddCommon( pContainer, initFunc, pParams, TRUE );

}

//===================================================================
/**
 *	@brief	指定コンテナにオブジェクトを追加(リスト最後尾)
 *
 *	@param	pContainer	所属コンテナのポインタ
 *	@param	initFunc	初期化で呼び出す関数のポインタ
 *	@param	pParams		パラメータ(NULL可)
 *
 *	@return	成功時は追加されたオブジェクトのポインタ / 失敗時はNULL
 */
//===================================================================
KLObj* KLContainer_Add( KLContainer* pContainer, KLObjFunc initFunc, void* pParams ){
	
	return KLContainer_AddCommon( pContainer, initFunc, pParams, FALSE );

}

//=====================================================================
/**
 *	@brief	コンテナ内の解放予約をしてあるオブジェクトを実際に処理する
 *
 *	@param	pContainer	所属コンテナのポインタ
 *	@param	pObj		削除するオブジェクト
 *	@param	pParams		パラメータ
 *
 *	@return	失敗時FALSE
 *	
 */
//=====================================================================
bl KLContainer_ExecuteFree( KLContainer* pContainer, KLObj* pObj, void* pParams ){

	KLObj*	pPrev;
	KLObj*	pNext;
	
	if( pObj->pContainer != (void*)pContainer )
	{
		KLLog("[ KLContainer ] invalid address.\n");
		pContainer = (KLContainer*)pObj->pContainer;
	}
	
	if( pObj->Unload )
	{
		pObj->Unload( pObj, pParams );
	}
	
	pPrev = pObj->pPrev;
	pNext = pObj->pNext;
	
	if( pPrev )
	{
		pPrev->pNext = pNext;
		
		if( pContainer->pTail==pObj )
		{
			pContainer->pTail = pNext? pNext : pPrev;
		}
	}
	else
	{
		if( pContainer->pTail==pObj )
		{
			pContainer->pTail = pNext? pNext : pPrev;
		}
	}
	
	if( pNext )
	{
		pNext->pPrev = pPrev;
		
		if( pContainer->pTop==pObj )
		{
			pContainer->pTop = pPrev? pPrev : pNext;
		}
	}
	else
	{
		if( pContainer->pTop==pObj )
		{
			pContainer->pTop = pPrev? pPrev : pNext;
		}
	}
	
	KLObj_FreeNow(pObj);
	
	if( pContainer->num==0 )
	{
		KLLog("[ KLContainer ] Invalid count.\n");
		return FALSE;
	}
	else
	{
		pContainer->num--;
	}
	
	return TRUE;
}

