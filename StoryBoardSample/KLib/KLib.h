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
//		
//	ThirdParty library
//	===============================================
//
//		stb_image
//			stbi-1.33 - public domain JPEG/PNG reader - http://nothings.org/stb_image.c
//

/*
 KLibは、できる限りObjectiveC(＋ARC等)を使わず、
 C＋OpenGL/OpenALで2Dゲームを開発するために作られたライブラリです。
 Cocoaをほとんど知らずとも、C/C++での開発経験があれば
 それなりにiOSアプリが作成できるライブラリを目指しています。
 
 シーンごとのメモリ管理、擬似タスクシステム、ファイルアクセスラッパー、M4a/mp3/wavサウンドプレイヤ
 テクスチャローダ、OpenGLES2を1のように扱える簡易描画ラッパーなどが主な機能です。
 C言語メインなので、Objective-CやC++に慣れている方は当ライブラリを使うとかえって効率が悪くなる事が予想されます。
 ライブラリ使用にあたって、IBはもちろんXcodeのローカライズ文字列ファイルや、
 Cocoa関係のコードはできる限り使わない事を前提にしています。(タップ処理のようなObjCしかAPIがないものを除く)
 とはいえ、Cocoaと併用開発も可能です。
 文字列もCocoaのNSStringではなく、charをメインに扱います。
 16bit固定のワイド文字列であるwcharや4byte1文字で扱えるものも用意してあります。
 テキスト入力などまだ実装されていない機能や雑に組んだ部分もまだまだあるので、
 その辺は追々直したり追加していければと思います。
 
 端末機能が、iOS5.x以上、armv7(s)、Retina対応、
 コンパイラは、Apple LLVM Compiler 4.1 + GNU99 を対象としており
 非Retinaやarmv6端末など上記に当てはまらない端末は非対応です。
 
 オープンソースとして公開していますが、あくまで自作アプリの補助用に開発したものなので
 サポートや需要案件の実装等を優先して行える保証はありません。
 
 [命名規則や方針など]
 
 構造体(KLオブジェクト)は、大文字開始の他小文字＋単語の頭で大文字
 
 intを表すs32などの基本型は、全部小文字
 
 boolの代替型としてblを定義しています。実際にはsigned charで非0がTRUEと同義とする
 charとしても扱えるので、真偽判定用のほか、1/0/-1など３種類の判別にも使える緩いboolの位置づけ
 
 関数はKLDevice_など関係するオブジェクト名+アンダーバー+構造体ルール
 関連オブジェクトを持たない場合や定義関連付けに迷っている機能は、KL_で始まるか、
 連想できるもの(MEM_など)＋上記ルールとする。
 ただし、OpenGLに関して本家との命名差異を小さくするため、glsを頭にし
 例として本家glEnableのサポート関数は、glsEnableとする
 
 マクロも関数と同様の規則で命名する
 
 型関係の警告の設定を逐一変えなくていいように
 型定義はtypedefではなくdefineで行っている
 
 ObjectiveCの利用がどうしても必要なものはKlibDelegate.mなどmファイルに行っている
 ただし、それらの関数のプロトタイプはCファイルから設定変更せずインクルード可能にするため
 例外的にKLib.hや関連コードのヘッダ内に定義している
 
 ARC、およびマルチスレッドを用いた利用は見込んでいないので動作未保証
 サードパーティも含めARCは使用しない。
 自作コードでARC併用したい場合は、そのコードにだけビルド設定で「-fobjc-arc」を付けること。
  
 [既知の問題点]
 [KLLabel 1]
 KLLabelマッピングが単一フォントしかサポートしていないので、
 絵文字が使用できない。
 
 [KLGameCenter 1]
 リーダーボードのランク返却、反映同期タイミングがおかしい(サンドボックスの不具合らしい)
 その不具合に依存してダミーランクの返却で１プレイヤーが２ランクインなどおかしくなる事がある。
 */

#ifndef __KLib__
#define __KLib__

#include "c_extern.h"

#include <stdio.h>
#include "KLibDefine.h"
#include "KLDevice.h"
#include "KLGL.h"
#include "KLScene.h"
#include "KLScreen.h"
#include "KLShader.h"
#include "KLBlock.h"
#include "KLBench.h"
#include "KLTouch.h"
#include "KLHeapDebug.h"
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CGContext.h>

//==============================================================
/**
 *	@brief	Path情報<br/>
 *			ドキュメント・tmpフォルダはfopenできず<br/>
 *			objCのAPI経由じゃないとアクセスできないので注意<br/>
 *			それらのアクセスはKLFile_系の命令を使うこと
 */
//==============================================================
typedef struct{
	
	char docRoot[KLPATH_SIZE];	///< ドキュメントルート
	char tmpRoot[KLPATH_SIZE];	///< テンポラリルート
	char cacheRoot[KLPATH_SIZE];///< キャッシュルート
	char appRoot[KLPATH_SIZE];	///< アプリ自身のルート
	
}KLPath;

typedef struct{

	char**	ppTitle;
	char**	ppDescription;
	char**	ppPrice;
	char**	ppPid;
	u64		length;
	bl		connecting:1;
	
}KLPurchase;

enum {

	KLGAMECENTER_ERR_NOSCORE	= 404,
	KLGAMECENTER_ERR_NULLSCORE	= 400,
	KLGAMECENTER_ERR_NOPLAYER	= 504,
	
};

typedef struct{
	
	char	alias[KLGAMECENTER_ALIAS_MAXLEN];	///< 表示名
	char	pid[KLGAMECENTER_PID_MAXLEN];		///< PlayerID(rejuct対象なので表示しない事)
	int64_t	score;
	s64		rank;
	
}KLLeaderboardScore;

typedef struct{
	
	u64					nScore;				///< スコア数
	u64					min, max;			///< 最小ランクと最高ランク
	KLLeaderboardScore*	pScore;				///< ランキングバッファ
	KLLeaderboardScore	localPlayerScore;	///< ランキング内の自身のスコア
	
}KLLeaderboard;

typedef struct{
	
	char	alias[KLGAMECENTER_ALIAS_MAXLEN];	///< 起動中のプレイヤーの表示名
	char	pid[KLGAMECENTER_PID_MAXLEN];		///< 起動中のプレイヤーのPlayerID(rejuct対象なので表示しない事)
	int		code;								///< Error時のコード / 成功時は0
	bl		connecting:1;						///< 認証などで接続中の時はTRUE、その時は値が不安定なので参照を推奨しない
	bl		success:1;							///< 認証成功時TRUE
	bl		usable:1;							///< GameCenterが使用可能ならTRUE / 使用できない場合はFALSE
	void	(*reportCallback)(bl okng, u32 errCode, void* pContext); ///< Report callback
	void*	reportContext;						///< Report context
	
}KLGameCenter;

typedef int (*KLGameCenterCallback)();

typedef struct{
	
	void (*callbackGotoBackground)(void* pContext);
	void (*callbackEnterForeground)(void* pContext);
	void (*callbackAppTerminate)(void* pContext);
	void* pContext, *pContextFore, *pContextTerminate;
	
}KLFunc;

typedef struct{

	bl					isnotCloseOutTap;	///< キーボード表示中、キーボード外タップで閉じる場合はFALSE
	
}KLKeyboard;

//==============================================================
/**
 *	@brief	KLibオブジェクト
 */
//==============================================================
typedef struct{
	
	KLDevice		device;		///< どの端末か
	float			fps;		///< FPS
	KLScreen		screen;		///< メイン画面情報
	KLScreen		view;		///< OpenGLView情報
	KLScene			scene;		///< シーンオブジェクト
	KLPath			path;		///< パス情報
	KLBlock			heap;		///< ヒープ管理
	KLTouch			touch;		///< タッチ管理
	KLGameCenter	gamecenter;	///< ゲームセンター	[deprecated]
	KLPurchase		purchase;	///< 課金			[deprecated]
	KLKeyboard		kbd;		///< キーボード周り
	KLShaderInfo	shader;		///< Shader用変数
	KLFunc			func;		///< イベントコールバックなど
	bl				loaded;		///< 初期化済フラグ
	
}KLib;



// F U N C T I O N
//==============================================================

	void KL_Init(u32 glview_width, u32 glview_height);
	void KL_Unload();
	void KL_PrepareReuse();

	void KLPath_Init();

	u64	KL_GetNowU64();

	u32 KL_GetViewW();
	u32 KL_GetViewH();
	void KL_StartLoop();
	void KL_StopLoop();

	void KLUIGraphicsPushContext( CGContextRef context );
	void KLUIGraphicsPopContext();

	// GameCenter
	void	KLGameCenter_Auth( KLGameCenterCallback pFunc );
	bl		KLGameCenter_ReportScore(
				char* pLeaderboardId, int64_t value,
				void(*reportCallback)(bl okng, u32 errCode, void* pContext),
				void* pContext );

	void	KLGameCenter_SetReportCallback(
				void(*reportCallback)(bl okng, u32 errCode, void* pContext),
				void* pContext );

	bl		KLGameCenter_GetRanking(
				char* pLeaderboardId,
				u32 minRank, u32 maxRank,
				bl isFriendOnly, u8 typeAll0_Week1_Today2,
				KLLeaderboard* pKLLeaderboard,
				void(*getRankingCallback)(bl okng, u32 errCode, void* pContext),
				void* pContext );

	void	KLGameCenter_Unload();
	bl		KLGameCenter_InitVirtualScore( u32 nAlloc );
	void	KLGameCenter_MakeAutoVirtualScore(
				int64_t topScore,
				bl isAscending,
				u32 scoreUpRateMin,
				u32 scoreUpRateMax );
	char*	KLLeaderboard_GetChainAliasStringForLabel( KLLeaderboard* pKLLeaderboard );
	void	KLLeaderboard_Unload( KLLeaderboard* pKLLeaderboard );

	// In-App Purchase(組み込み用のみ)
	void	KLPurchase_Init();
	void	KLPurchase_Unload();
	void	KLPurchase_CancelGetList();
	bl		KLPurchase_GetList( char** ppProductIdList, u32 nList, void(*getItemCallback)(void* pContext, s32 result, u64 codeOrListCount), void* pContext );
	void	KLPurchase_BuyItem( char* pProductId, void(*successCallback)(void* pContext), void(*failedCallback)(void* pContext, u32 errcode, u32 canceledEnumInt), void* pContext );
	void	KLPurchase_RestoreItem( void(*successCallback)(void* pContext,char* pProductId), void(*failedCallback)(void* pContext, u32 errCode, u32 canceledEnumInt), void(*allEndCallback)(void* pContext), void* pContext );
	bl		KLPurchase_IsUsable(bl dontPopup);



	// Indicator
	void KLIndicator_Start();
	void KLIndicator_SetXY( s32 x, s32 y );
	void KLIndicator_SetWH( u16 w, u16 h );
	void KLIndicator_SetAlpha(u8 a);
	void KLIndicator_Stop( bl withHide );
	void KLIndicator_Init();
	void KLIndicator_Unload();
	void KL_StartIndicator();
	void KL_StopIndicator(bl withHide);



	// Keyboard
	void KLKeyboard_Init();
	void KLKeyboard_Unload();
	void KLKeyboard_SetXY( s32 x, s32 y );
	void KLKeyboard_SetWH( u16 w, u16 h );
	void KLKeyboard_Open ( const char* pFirstStrOrContinueBeforeTextByNULL, void (*returnedCallback)(char* pInputText, void* pContext), void (*closedCallback)(char* pInputText, void* pContext), void* pContext );
	void KLKeyboard_MaxStringSize( int maxlen );
	void KLKeyboard_SetColor( Rgba bgcolor, Rgba textcolor );
	void KLKeyboard_SetBorderStyle ( KLTextBorderStyle style );
	void KLKeyboard_SetType ( KLKeyboardType type );
	void KLKeyboard_SetReturnKeyType ( KLReturnKeyType type );
	void KLKeyboard_SetTextFieldClearBtn ( KLTextFieldViewMode mode );



	// Events
	void KL_DidEnterBackground();
	void KL_WillEnterForeground();
	void KL_SetBackgroundCallback( void (*callbackGotoBackground)(void* pContext), void* pContext );
	void KL_SetForegroundCallback( void (*callbackEnterForeground)(void* pContext), void* pContext );
	void KL_SetUnloadCallback( void (*callbackAppTerminate)(void* pContext), void* pContext );
	void KLView_DidEnterBackground();
	void KLView_DidEnterForeground();

#ifdef DEBUG

	#define KLLog printf

#else

	#define KLLog(...) 

#endif

	void KLAlert_Open( char* pTitle, char* pMsg, char* pBtnLabel, void(*btnPressedCallback)() );

	// Objective-C (for KLib calls)
	void KLObjC_Init();
	void KLObjC_Unload();

	void KLView_Unload ();


	// View
	void KL_GotoSegue ( char* pSegueName );

	extern KLib klib;

#include "c_extern_close.h"
#endif /* defined(__KLib__Screen__) */
