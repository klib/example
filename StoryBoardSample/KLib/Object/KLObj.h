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

#ifndef ___KLIB_OBJECT_H___
#define ___KLIB_OBJECT_H___

// I N C L U D E
//================================================

	#include "c_extern.h"
	#include <stdio.h>
	#include "KLibDefine.h"

// D E F I N E
//================================================

	/// インデクスを指定してオブジェクト部分を取得
	#define KLObj_GetIndex(pManager, pListTop, index) \
				(KLObj*)(((u8*)pListTop)+(index)*pContainer->szOneObject)


// E N U M
//================================================

	/// ヒープの状態
	typedef enum {
		
		KLOBJ_H_FREE = 0,	///< 待機
		KLOBJ_H_USING,		///< 使用中
		
	}KLObjHeapState;

	
	/// オブジェクトの状態
	typedef enum {
		
		KLOBJ_WAIT = 0,		///< 待機
		KLOBJ_RUN,			///< 実行中
		KLOBJ_RESERVED_FREE,	///< 破棄予約済
		
	}KLObjState;



// S T R U C T U R E
//================================================

	struct KLContainer;
	struct KLObj;
	
	/// オブジェクト
	typedef struct KLObj{

		KLObjState			state;			///< 状態
		struct KLContainer*	pContainer;		///< 所属先
		
		struct KLObj*			pParent;
		struct KLObj*			pPrev;
		struct KLObj*			pNext;
		
		void (*Run)( struct KLObj*, void* );		///< 実行関数
		void (*Unload)( struct KLObj*, void* );	///< 終了関数
		void(*Listener)(struct KLObj*,u32,void*); ///< イベントリスナー
		
	}KLObj;
	
	/// オブジェクトコンテナ
	typedef struct KLContainer{

		KLObj		core;			///< コンテナ自身もオブジェクト扱いできるようにしておく
		KLObj*		pTop;			///< 最初のオブジェクト
		KLObj*		pTail;			///< 最後のオブジェクト
		KLObj*		pFreeTop;		///< 最初の静的空きエリア
		KLObj*		pFreeTail;		///< 最後の静的空きエリア
		KLObj*		pFree;			///< 空きエリア全体
		KLObj*		pParent;		///< 親オブジェクトへのポインタ(デフォルトNULL)
		size_t		szOneObject;	///< 1オブジェクトあたりのサイズ
		u32			num;			///< 確保した個数
		size_t		remain;			///< 空き個数
		bl			isStatic;		///< 固定長か動的確保か
		
	}KLContainer;

	typedef void (*KLObjFunc)( KLObj*, void* );
	

// F U N C T I O N
//================================================

	// Container
	//-------------------
	KLObj*			KLContainer_Add( KLContainer* pContainer, KLObjFunc initFunc, void* pParams );
	KLObj*			KLContainer_AddTop( KLContainer* pContainer, KLObjFunc initFunc, void* pParams );
	void			KLContainer_Execute( KLContainer* pContainer, void* pParams );
	bl				KLContainer_ExecuteFree( KLContainer* pContainer, KLObj* pObj, void* pParams );
	void			KLContainer_FreeAll( KLContainer* pContainer );
	KLObj*			KLContainer_GetObject( KLContainer* pContainer, u32 index );
	bl				KLContainer_Init( KLContainer* pContainer, size_t szOneObject, size_t nObject, void* pParentObj );
	void			KLContainer_Send( KLContainer* pContainer, u32 eventid, void* pParams );
	KLContainer*	KLContainer_Unload( KLContainer* pContainer, void* pParams );
	
	
	
	// Object
	//------------------
	void		KLObj_SetupForKLib( void* pTargetObj );
	KLObj*	KLObj_Alloc( KLContainer* pContainer, bl addToTop );
	KLObj*	KLObj_AllocStatic( KLContainer* pContainer, bl addToTop );
	void		KLObj_Clear( void* pObj, size_t szWork );
	void		KLObj_CopyWithoutCore( KLObj* pDest, KLObj* pSrc, size_t sz );
	void		KLObj_Free( void* pObj );
	void		KLObj_FreeNow( void* pObj );
	void		KLObj_FreeStatic( KLContainer* pContainer, KLObj* pObj );
	bl			KLObj_Init( KLContainer* pContainer, void* pExtendedObj, bl isAddToTop );
	void		KLObj_SetFunction( void* pObj, KLObjFunc func );
	void		KLObj_SetFunctions( void* pObj, KLObjFunc func, KLObjFunc unloadFunc );
	void		KLObj_SetListener( void* pObj, void(*pListener)(KLObj*,u32,void*) );
	void		KLObj_SetUnload( void* pObj, KLObjFunc unloadFunc );
	
#include "c_extern_close.h"
#endif