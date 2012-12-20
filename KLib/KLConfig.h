//
//  KLConfig.h
//  KLib
//
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

#ifndef KLib_KLConfig_h
#define KLib_KLConfig_h

// D E B U G
//============================================================
#ifdef DEBUG

	/// KLibをデバッグモードで動かすか
	#define KL_DEBUG (1)

	/// KLibHeapDebugでデバッグ出力を使用するか
	#define KL_DEBUG_MM (1)

	/// KLTouchのデバッグ情報を出力するか
	#define KLTOUCH_LOGGING (0)

	/// GameCenterのランキングデータを自作仮データでデバッグするか
	#define KL_DEBUG_LEADERBOARD_RANKING (0)

	/// KLALのエラーチェック有無
	#define KLAL_ERRCHECK (1)

#else

	// !!! こちらはリリース用なので変更しない事
	#define KL_DEBUG (0)

	// !!! こちらはリリース用なので変更しない事
	#define KL_DEBUG_MM (0)

	// !!! こちらはリリース用なので変更しない事
	#define KLTOUCH_LOGGING (0)

	// !!! こちらはリリース用なので変更しない事
	#define KL_DEBUG_LEADERBOARD_RANKING (0)

	/// !!! こちらはリリース用なので変更しない事
	#define KLAL_ERRCHECK (0)

#endif

// S E E D
//============================================================

// KLib内の乱数生成は4つのキーを元に使用します。
// サーバーなど別端末でも同じアルゴリズムとキーで作成する事で簡易整合に使用できます。
//
// 乱数使用のハッシュや簡易暗号化の予測防止のため、
// デフォルト値ではなくアプリごとの変更をおすすめします。

/// ランダムシードA
#define KLRANDOM_KEY_A (1234567)

/// ランダムシードB
#define KLRANDOM_KEY_B (8901234)

/// ランダムシードC
#define KLRANDOM_KEY_C (5678901)

/// ランダムシードD
#define KLRANDOM_KEY_D (2345678)


// T O U C H
//============================================================

/// マルチタッチを許可するならゼロ以外を指定
#define KLTOUCH_DEFAULT_MULTITOUCH_ENABLED (1)

/// タップを離してから何フレーム以内に再タップしたらダブルタップとみなすか
#define KLTOUCH_WTAP_ALLOW_DELAY (25)


// G R A P H I C S
//============================================================

/// メインループをNSTimerで回すかDisplayinkで回すか(公式ドキュメント的に後者使用が良さそう)
#define KLIB_USE_NSTIMERLOOP (0)

/// 円描画時の頂点数デフォルト
#define KL_CIRCLE_QUALITY_HIGH (36)

/// [未実装]マルチサンプリングの有無(パフォーマンス優先なら無効を推奨)
#define KL_MULTISAMPLING (1)

/// インジケータの初期スタイル
#define KLINDICATOR_FIRSTSTYLE (UIActivityIndicatorViewStyleWhiteLarge)
//#define KLINDICATOR_FIRSTSTYLE (UIActivityIndicatorViewStyleWhite)
//#define KLINDICATOR_FIRSTSTYLE (UIActivityIndicatorViewStyleGray)



// T Y P E D E F
//============================================================

// WideDegreeの1周レート(数値に合った型定義に変える事)
#define KL_WDEG_CYCLE (0x10000)

// 円周率
#define PI (3.141592653589793f)


// N E T W O R K
//===========================================================
/// Wifi addressバッファサイズ
#define KLNETWORK_WIFIADRS_BUFSIZE (64)



// G A M E C E N T E R
//===========================================================
/// Alias(Name)最大長（それ以上は切り捨て）
#define KLGAMECENTER_ALIAS_MAXLENGTH (32)


// U U I D
//===========================================================

/// UUIDをキーチェーンに保存する際のカテゴリ名
#define KLUUID_SERVICE "UniversallyUniqueIdentifier"


// H E A P
//===========================================================

/// mallocセレクタ
#define Malloc malloc

/// callocセレクタ
#define Calloc(sz) calloc(1,sz)

/// Freeセレクタ
#define Free free

/// KLPath buffer size
#define KLPATH_SIZE (512)

/// UIID buffer size
#define KLUIID_SIZE (256)

#endif
