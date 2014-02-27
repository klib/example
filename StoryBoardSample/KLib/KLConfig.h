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



// S E E D S
//============================================================

// KLib内の乱数生成は4つのキーを元に使用します。
// サーバーなど別端末でも同じアルゴリズムとキーで作成する事で簡易整合に使用できます。
//
// 乱数使用のハッシュや簡易暗号化の予測防止のため、
// デフォルト値ではなくアプリごとの変更をおすすめします。

/// ランダムシードA
#define KLRANDOM_KEY_A (123456)

/// ランダムシードB
#define KLRANDOM_KEY_B (789012)

/// ランダムシードC
#define KLRANDOM_KEY_C (345678)

/// ランダムシードD
#define KLRANDOM_KEY_D (901234)



// T O U C H
//============================================================

/// マルチタッチを許可するならゼロ以外を指定
#define KLTOUCH_DEFAULT_MULTITOUCH_ENABLED (1)

/// タップを離してから何フレーム以内に再タップしたらダブルタップとみなすか
#define KLTOUCH_WTAP_ALLOW_DELAY (25)



// G R A P H I C S
//============================================================

/// タッチ位置を表示するかのフラグ
#define KLIB_VISIBLE_TOUCHPOS (1)

/// 初期背景色(0 - 255)
#define KL_BGCOLOR_R (0)
#define KL_BGCOLOR_G (0)
#define KL_BGCOLOR_B (0)
#define KL_BGCOLOR_A (255)

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

/// UV処理をshortで行う場合に定義する・未定義でfloat扱い(切替時シェーダ内計算は手動で変更する必要あり)
#define KLGL_SHORTTYPEUV (1)

/// マルチサンプルのデフォルト設定
#define KLGL_USE_MULTISAMPLE (1)


// L A B E L
//============================================================

/// CFString コンバートバッファサイズ
#define CFS_CONV_LEN (256)

/// KLLabelパースバッファ初期サイズ
#define KLLABEL_DEFAULT_PARSEBUF_SIZE (1024)

/// KLLabel文字コンバート用バッファ各種初期サイズ
#define KLLABEL_DEFAULT_GLYPH_WORK_SIZE (1024)
#define KLLABEL_DEFAULT_UTF16_WORK_SIZE (KLLABEL_DEFAULT_GLYPH_WORK_SIZE)
#define KLLABEL_DEFAULT_BBOX_WORK_SIZE (KLLABEL_DEFAULT_GLYPH_WORK_SIZE)
#define KLLABEL_DEFAULT_ADV_WORK_SIZE (KLLABEL_DEFAULT_GLYPH_WORK_SIZE)
#define KLLABEL_DEFAULT_STR_WORK_SIZE (4*1000+1)
#define KLLABEL_DEFAULT_STR32_WORK_SIZE (1000+1)
#define KLLABEL_DEFAULT_STR64_WORK_SIZE (1000+1)
#define KLLABEL_DEFAULT_EMOJISTR_WORK_SIZE (KLLABEL_DEFAULT_STR_WORK_SIZE)
#define KLLABEL_DEFAULT_EMOJISTR32_WORK_SIZE (KLLABEL_DEFAULT_STR32_WORK_SIZE)
#define KLLABEL_DEFAULT_EMOJIGLYPH_WORK_SIZE (KLLABEL_DEFAULT_STR32_WORK_SIZE)

/// 色変更タグ開始文字のデフォルト
#define KLLABEL_TAG_COL_START ('[')

/// 色変更タグ終了文字のデフォルト
#define KLLABEL_TAG_COL_END (']')


/// 色情報タグバッファ数(初期化時の辞典で確保する数、これ以上になる場合は都度 realloc される)
#define KLLABEL_DEFAULT_RESERVE_COLLIST (16)


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

/// Alias最大長(1文字最大4byteの文字があるので注意)
#define KLGAMECENTER_ALIAS_MAXLEN (32*4)

/// PlayerIDバッファ最大長(ドキュメントに不定と定義されているので十分な長さを確保する事)
#define KLGAMECENTER_PID_MAXLEN (128)



// U U I D
//===========================================================

/// UUIDをキーチェーンに保存する際のカテゴリ名
#define KLUUID_SERVICE "UniversallyUniqueIdentifier"



// O B J E C T
//===========================================================

/// シーン最大ワークサイズ(シーンサイズがこのサイズを超えるとメモリ破壊の可能性があるので要注意)
#define KLSCENE_WORKSIZE (1024*32)

/// シーン切り替えスタック数
#define KLSCENE_STACK_NUM (2)




// H E A P
//===========================================================

/// mallocセレクタ
#define Malloc(sz) malloc((size_t)(sz))

/// callocセレクタ
#define Calloc(sz) calloc(1,(size_t)(sz))

/// Freeセレクタ
#define Free free

/// デバッグ情報付きMalloc (現状realloc・calloc不可)
#define dMalloc(sz) KLHeapDebug_CoreAlloc ( NULL, (size_t)(sz), __FILE__, __LINE__ )
#define dFree (p) KLHeapDebug_CoreFree ( NULL, p, __FILE__, __LINE__ )


/// KLPath buffer size
#define KLPATH_SIZE (512)

/// UIID buffer size
#define KLUIID_SIZE (256)

#endif
