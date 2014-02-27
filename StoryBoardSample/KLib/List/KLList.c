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

#include "KLList.h"
#include "KLHeapDebug.h"

static KLHeapDebug kllistdebug;

//========================================
/**
 *	@brief	リストデバッガを初期化
 */
//=======================================
void KLList_InitForKLib(){
	
	klmemclear( &kllistdebug, KLHeapDebug );
	
}



//========================================
/**
 *	@brief	リストを配列に変換する
 *
 *	@param	*list	変換するリストの"先頭"のポインタ
 *
 *	@return		変換後のリスト配列の先頭ポインタ/失敗時はNULL
 */
//=======================================
KLList* KLList_ToArray( KLList* pList ){
	
	int		i = 0, len = 0;
	void*	p		= NULL;
	KLList*	pRet	= NULL;
	KLList*	pTop	= pList;
	
	// リスト数をカウント
	len = KLList_Len( pList );
	
	if ( len==0 )
	{
		return NULL;
	}
	
	pRet = (KLList*) Malloc( sizeof(KLList) * len );
	
	if ( !pRet )
	{
		return NULL;
	}
	
	// 各リスト要素をコピー
	for ( i = 0;   i < len && pTop;   i++ )
	{
		p = pTop->pData;
		
		if( !p )
		{
			pRet[i].pData	=  p;
		}
		else
		{
			pRet[i].pData	=  p;
		}
		
		pRet[i].pNext	= pTop->pNext;
		pRet[i].pPrev	= pTop->pPrev;
		pTop = pTop->pNext;
	}
	
	return pRet;
	
}



//========================================
/**
 *	@brief	指定インデクスのリストを取得する
 *
 *	@param	index	取得するインデクス
 *
 *	@return		取得したリストのポインタ/なければNULL
 */
//========================================
KLList* KLList_Get( KLList* pList, unsigned int uiIndex ){
	
	u32 i = 0;
	KLList *pRet = pList;
	
	while( pList )
	{
		if( i++ == uiIndex  )
		{
			pRet = pList;
			break;
		}
		pList = pList->pNext;
	}
	
	return pRet;
}

//========================================
/**
 *	@brief	指定リストを後方に挿入
 *
 *	@param	pList	このリストの後方にセットする
 *	@param	pData	作成した新リストに当てはめるデータ
 *
 *	@warning	*pListがNULLの時は内部でリスト生成するだけになってしまうので呼出先でこの関数の戻り値をpListに入れる事を推奨
 */
//========================================
KLList* KLList_Push( KLList* pList, void* pData ){
	
	KLList* pLocal = NULL;
	
	pLocal = (KLList*) malloc( sizeof(KLList) );
	
	// 失敗時はNULLを返す
	if( !pLocal )
	{
		KLLog("[ KLLibList_Push ] allocation failed.\n");
		return NULL;
	}
	
	pLocal->pPrev	=
	pLocal->pNext	= NULL;
	
	pLocal->pData	= pData;
	
	if( pList ){
		
		pLocal->pNext	= pList->pNext;	// 引数のリスト後を挿入リスト後にする
		pLocal->pPrev	= pList;			// 挿入リストの前が引数のリスト
		pList->pNext	= pLocal;			// 引数リストの次は挿入リスト
		
		if( pLocal->pNext )
			pLocal->pNext->pPrev = pLocal;
		
	}
	
	return pLocal;

}


//========================================
/**
 *	@brief	指定リストを前方に挿入
 *
 *	@param	pList	このリストの前方にセットする
 *	@param	pData	作成した新リストに当てはめるデータ
 *
 *	@warning	*pListがNULLの時は内部でリスト生成するだけになってしまうので呼出先でこの関数の戻り値をpListに入れる事を推奨
 */
//========================================
KLList* KLList_InsertTop( KLList* pList, void* pData ){
	
	KLList* pLocal = NULL;
	
	// 引数のリストが空じゃなければ処理
	pLocal = (KLList*) malloc( sizeof(KLList) );
	
	// 失敗時はNULLを返す
	if( !pLocal )
	{
		KLLog("[ KLLibList_InsertTop ] allocation failed.\n");
		return NULL;
	}
	
	pLocal->pPrev	=
	pLocal->pNext	= NULL;
	
	pLocal->pData	= pData;
	
	if( pList )
	{
		pLocal->pPrev	= pList->pPrev;	// 引数のリスト後を挿入リスト後にする
		pLocal->pNext	= pList;		// 挿入リストの前が引数のリスト
		pList->pPrev	= pLocal;		// 引数リストの次は挿入リスト
		
		if( pLocal->pPrev )
		{
			pLocal->pPrev->pNext = pLocal;
		}
	}
	
	return pLocal;
	
}



//========================================
/**
 *	@brief	指定リストの削除
 *
 *	@param	*pList			削除したいリストポインタ
 *	@param	isReturnNext	リターンするポインタは削除するリストのnext…TRUE/prev…FALSE
 *
 *	@return	削除したリストのprevかnextに当たるリストポインタ
 *
 *	@warning	中でpDataの解放はしないので、pDataに動的確保の変数を割り当てた場合はコール前の解放を忘れずに
 */
//========================================
KLList* KLList_Delete( KLList* pList, bl isReturnNext ){
	
	KLList* pRet = NULL;
	
	if( pList )
	{
		// 戻り値の設定
		pRet = pList->pPrev;
		
		if( isReturnNext )
		{
			pRet = pList->pNext;
		}
		
		// 削除後の連結の設定
		if( pList->pPrev )
		{
			pList->pPrev->pNext = pList->pNext;
		}
		
		if( pList->pNext )
		{
			pList->pNext->pPrev = pList->pPrev;
		}
		
		sfree(pList);
		
	}
	
	return pRet;
	
}

//========================================
/**
 *	@brief	指定リストの全削除
 *
 *	@param	*pList			削除したいリストポインタ
 *
 */
//========================================
void KLList_Clear( KLList* pList ) {
	
	while ( KLList_Delete( pList, TRUE ) );
	
}

//========================================
/**
 *	@brief	指定リストを含めたそれ以降にあるリスト数を取得
 *
 *	@param	*pList		カウントを開始するリストポインタ
 *
 *	@return	指定したリスト以降のリスト数(指定したものも含む)
 */
//========================================
int KLList_Len( KLList* pList ){
	
	int result = 0;
	
	while( pList )
	{
		pList = pList->pNext;
		++result;
	}
	
	return result;
	
}
