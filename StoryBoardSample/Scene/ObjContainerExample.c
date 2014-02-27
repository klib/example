//
//  ObjContainer.c
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#include "KLRandom.h"
#include "KLDrawQueue.h"
#include "ObjContainerExample.h"
#include "EmptyScene.h"
#include "Game.h"

#define SAMPLE_TEXT "白い矩形をタップで全親から子を発生\n(このサンプルでは子のMAXは8)\n親タップですべての子を消滅させる\n端末向き変えで描画方向も変わる"


// 子オブジェクトのイベントリスナー
static void Child_EventListener( KLObj* pObj, u32 eventid, void* pParam ){
	
	if( eventid==EVENTID_DESTROYALL )
	{
		// 自身を解放予約(次回Execute時に解放実行)
		KLObj_Free(pObj);
	}
	
}

// 子オブジェクトの終了関数
static void Child_Unload( KLObj* pObj, void* pParam ){
	
	//ChildObj* p = (ChildObj*)pObj;
	
}

// 子オブジェクトのメインループ関数
static void Child_Main( KLObj* pObj, void* pParam ){
	
	ChildObj*	p		= (ChildObj*)pObj;
	ParentObj*	pParent	= (ParentObj*)pObj->pParent;
	
	KLPos3d pos[] = { {p->x + pParent->x, p->y + pParent->y, 0 }, {pParent->x, pParent->y, 0 } };
	Rgba	rgba[]= { RGBA(255,0,0,255), RGBA(255,255,255,16) };
	
	KLSprite_SetCenterSimple(p->pSprite);
	KLSprite_SetXY( p->pSprite, p->x + pParent->x, p->y + pParent->y );
	KL_DrawLineMultiColor( pos, rgba, 2, 4);
	KLSprite_Draw( p->pSprite );
	
}
	
// 子オブジェクトの初期化関数
static void Child_Init( KLObj* pObj, void* pParam ){
	
	ChildObj*	p		= (ChildObj*)pObj;
	//ParentObj*	pParent	= (ParentObj*)pObj->pParent;
	
	// KLContainer_Initで親のさらなる親にシーンオブジェクトのポインタを渡したので
	// pObj->pParent->pParentをキャストすることでシーンオブジェクトにアクセスできる
	ObjContainer* pScene = (ObjContainer*)pObj->pParent->pParent;
	
	p->pSprite 	= &pScene->spChild;
	
	//　親からイベントを受け取りたいのでイベントリスナーを設定
	pObj->Listener = Child_EventListener;
	
	p->x = KL_rand()%512 - 256;
	p->y = KL_rand()%512 - 256;
	
	// 次回Executeから子オブジェクトのメインループ関数に切り替え
	KLObj_SetFunction( pObj, Child_Main );
	
	// 子終了の際にアンロードをコール
	KLObj_SetUnload( pObj, Child_Unload );
	
}

// 親オブジェクトのイベントリスナー
static void Parent_EventListener( KLObj* pObj, u32 eventid, void* pParam ){
	
	ParentObj* p = (ParentObj*)pObj;
	
	if( eventid==EVENTID_TOUCHED_RECT )
	{
		// 空きがあるなら子を追加
		KLContainer_Add( &p->children, Child_Init, NULL );
	}
	
}

// 親オブジェクトのアンロード関数
static void Parent_Unload( KLObj* pObj, void* pParam ){
	
	ParentObj* p = (ParentObj*)pObj;
	
	KLContainer_Unload( &p->children, NULL );
	
}

// 親オブジェクトのメインループ関数
// シーン同様の方法でKLContainerに積んだオブジェクトは処理できる
static void Parent_Main( KLObj* pObj, void* pParam ){
	
	ParentObj* p = (ParentObj*)pObj;

	
	// 子オブジェクトの順次処理(なければ何もしない)
	KLContainer_Execute( &p->children, NULL );
	
	
	// 回転(一周360度ではなく65536度)
	p->r += 64;
	
	// 現在のサイズの半分を中心点とする
	KLSprite_SetCenterSimple( p->pSprite );
	
	// 端末の向きで表示位置を変える
	if( KLDevice_IsPortrait() )
	{
		p->x = klib.view.w*0.5f;
		p->y = klib.view.h*0.25f*(p->id+1);
	}
	else
	{
		p->x = klib.view.w*0.25f*(p->id+1);
		p->y = klib.view.h*0.5f;
	}
	
	KLSprite_SetXY( p->pSprite, p->x, p->y );
	KLSprite_SetRot( p->pSprite, p->r );
	
	// 表示
	KLSprite_Draw( p->pSprite );
	
	// タッチしたら消滅
	if( KLTouch_IsStart( p->pSprite->x-p->pSprite->w*0.5f-16, p->pSprite->y-p->pSprite->h*0.5f-16, p->pSprite->w+32, p->pSprite->h+32 ) )
	{
		KLContainer_Send( &p->children, EVENTID_DESTROYALL, NULL);
	}
	
}

// 親オブジェクトの初期化関数
// シーン同様の方法でKLContainerに積んだオブジェクトは処理できる
static void Parent_Init( KLObj* pObj, void* pParam ){
	
	ParentObj* p = (ParentObj*)pObj;
	
	// KLContainer_Initでこのオブジェクトのさらなる親にシーンオブジェクトのポインタを渡したので
	// pObj->pParentをキャストすることでシーンオブジェクトにアクセスできる
	ObjContainer* pScene = (ObjContainer*)pObj->pParent;
	
	p->id		= *(u8*)pParam;
	p->pSprite 	= &pScene->spParent;
	
	// 子コンテナを作成( 親オブジェクトとして自身のポインタを渡す )
	KLContainer_Init( &p->children, sizeof(ChildObj), EXAMPLE_CHILD_MAX, p );
	
	//　シーンからイベントを受け取りたいのでイベントリスナーを設定
	pObj->Listener = Parent_EventListener;
	
	
	// 次回Executeから親オブジェクトのメインループ関数に切り替え
	KLObj_SetFunction( pObj, Parent_Main );
	
	// 親終了の際にアンロードをコール
	KLObj_SetUnload( pObj, Parent_Unload );
	
}


// シーン終了処理
static void ObjContainer_Unload( KLObj* pObj, void* pParam ){

	ObjContainer*	p	= (ObjContainer*)pObj;

	// シーン終了時に行いたい処理
	
	// 親をアンロード
	KLContainer_Unload( &p->parents, NULL );
	
	// ラベルアンロード
	KLLabel_Unload(&p->label);
	
	// スプライトをテクスチャ毎アンロード
	KLSprite_UnloadWithTexture( &p->spChild );
	KLSprite_UnloadWithTexture( &p->spParent );
	
}

// メインループ
static void ObjContainer_Update( KLObj* pObj, void* pParam ){
	
	ObjContainer* p	= (ObjContainer*)pObj;
	
	char* str = NULL;
	
	// 親を順次処理
	KLContainer_Execute(&p->parents, NULL);
	
	
	KL_DrawRect( klib.view.w*0.5f-72, klib.view.h-144, 144, 144, RGBA(255,255,255,255) );
				
	if( KLTouch_IsStart( klib.view.w*0.5f-72, klib.view.h-144, 144, 144 ) )
	{
		// 登録したイベントリスナーにイベントを発行
		KLContainer_Send( &p->parents, EVENTID_TOUCHED_RECT, NULL );
	}
	
	// 説明文
	str = SAMPLE_TEXT;
	p->label.fontsize = 32;
	
	// 縁取り
	u32 clen	= cstrlen(str);
	s32 x		= klib.view.w*0.5f;
	s32 y		= 0;//64;
	KLLabel_Draw(&p->label, str, clen, x+1, y, 0, RGBA(0,0,0,255), KLLABEL_ALIGN_CENTER);
	KLLabel_Draw(&p->label, str, clen, x-1, y, 0, RGBA(0,0,0,255), KLLABEL_ALIGN_CENTER);
	KLLabel_Draw(&p->label, str, clen, x,   y+1, 0, RGBA(0,0,0,255), KLLABEL_ALIGN_CENTER);
	KLLabel_Draw(&p->label, str, clen, x,   y-1, 0, RGBA(0,0,0,255), KLLABEL_ALIGN_CENTER);
	
	KLLabel_Draw(&p->label, str, clen, x, y, 0, RGBA(255,255,255,255), KLLABEL_ALIGN_CENTER);
	
}

void ObjContainer_Init( KLObj* pObj, void* pParam ){
	
	// KLibは予め確保したsizeof(KLObj)+シーンワークサイズ(KLConfig.hで調整可)分のヒープを
	// 引数のpObjとして。シーンワークサイズを超えない構造体であれば
	// そのヒープをキャストするだけで自由に扱えます。
	ObjContainer* p = (ObjContainer*)pObj;
	
	
	// シーン初期化時に行いたい処理
	//==========================================================
	
	// このシーンは黒いフェードインで開始
	KLScene_FadeIn( NULL, pObj, (Rgba){0,0,0,0}, 0);
	
	// 毎メインループで都度この関数が呼ばれて何度も初期化しても困るので
	// 次のメインループでは違う関数(初期化後の処理)をする関数に切り替えます。
	KLObj_SetFunction( pObj, ObjContainer_Update );
	
	
	// シーンが切り替わる際、このシーンのUnload処理を行う関数を設定します。
	KLObj_SetUnload( pObj, ObjContainer_Unload );

	// ラベルを作成
	KLFont font;
	KLFont_Load( &font, KLFont_name[KLFontType_HiraKakuProN_W6], FALSE);
	KLLabel_Init(&p->label);
	KLLabel_Setup( &p->label, SAMPLE_TEXT, &font );
	KLFont_Unload(&font);
	
	// サンプル用に親をPARENTS_NUM分だけヒープ作成
	// コンテナに積むオブジェクトのサイズ、最大いくつ積めるか、
	// 最後に親にしたいもののポインタ(今回は親の親をシーンオブジェクトとした) ないならNULL
	KLContainer_Init( &p->parents, sizeof(ParentObj), EXAMPLE_PARENTS_NUM, p );
	
	// コンテナに新しく親を作成
	for( int i=0; i<EXAMPLE_PARENTS_NUM; ++i )
	{
		
		// シーン同様初期化関数と渡したいパラメータがあればそのポインタを渡して作成
		// KLContainer_Initで最大数を決めた場合はそれ以上のAddは無視される
		KLContainer_Add( &p->parents, Parent_Init, &i);
		
	}
	
	// 親用スプライトとしてPNGのサンプルを読み込む
	KLSprite_LoadFile(&p->spParent, "example.png");
	
	// 親用スプライトとしてJPEGのサンプルを読み込む
	KLSprite_LoadFile(&p->spChild, "example.jpg");
	
}


