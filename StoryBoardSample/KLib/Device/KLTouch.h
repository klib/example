//
//  KLTouch.h
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

#ifndef KLib_KLTouch_h
#define KLib_KLTouch_h

#include "c_extern.h"
#include "KLibDefine.h"

typedef enum{
	
	KLFINGER_WAIT = 0,
	KLFINGER_TOUCHSTART,
	KLFINGER_TOUCHING,
	KLFINGER_MOVE,
	KLFINGER_RELEASE,
	KLFINGER_WAITING_WTAP,
	KLFINGER_WTAP,
	KLFINGER_UNKNOWN,
	
}KLFingerState;

typedef struct{

	u64				id;			///< 指ID
	
	u32				flag;		///< 状態フラグ(１フレーム回る間にSTART>ENDのような複数状態がありうる)
	KLFingerState	state;		///< 状態
	
	s32				startx;		///< タッチ開始点X
	s32				starty;		///< タッチ開始点Y
	s32				movex;		///< タッチ移動点X
	s32				movey;		///< タッチ移動点Y
	s32				movexold;	///< タッチ移動点X(1frame前)
	s32				moveyold;	///< タッチ移動点Y(1frame前)
	s32				endx;		///< タッチ終了点X
	s32				endy;		///< タッチ終了点Y
	
	KLPower			power;		///< 移動方向と平均速度
	u32				frames;		///< タッチしているフレーム数(最大0xFFFFFFFF)
	
	u32				wtapWait;	///< ダブルタップとみなす待機フレーム(デフォルトはfps/2)

}KLFinger;

/// 指定IDの指オブジェクトが存在しない
#define KLTOUCH_NOTFOUND_FINGER (-1)


/// iPadが最大11タッチ認識らしいのでそれに合わせる
#define KLTOUCH_MAXFINGER (11)


/// ダブルタップの間隔猶予フレームのデフォルト
#define KLTOUCH_DOUBLETAP_WAIT_DEFAULT (klib.fps*0.5f)


typedef struct{
	
	KLFinger	input[KLTOUCH_MAXFINGER];	///< 毎フレーム受け付ける入力バッファ イベント発行次第変化するのでフレーム中変化する可能性がある
	KLFinger	finger[KLTOUCH_MAXFINGER];	///< フレーム開始時から終了まで不変保証の参照先
	u8			nInput;						///< 入力バッファの有効数
	u8			nFinger;					///< フレーム内不変バッファの有効数
	u32			wtapwait;					///< ダブルタップ受付フレーム猶予(の初期値)
	
}KLTouch;

void		KLTouch_Init();
void		KLTouch_Reset();
void		KLTouch_Update();

void		KLTouch_Start(u64 fingerid, s32 tx, s32 ty);
void		KLTouch_Move(u64 fingerid, s32 tx, s32 ty);
void		KLTouch_End(u64 fingerid, s32 tx, s32 ty);
void		KLTouch_Cancel();

u64			KLTouch_IsStart( s32 x, s32 y, u32 w, u32 h );
u64			KLTouch_IsTouching( s32 x, s32 y, u32 w, u32 h );
u64			KLTouch_IsMove( s32 x, s32 y, u32 w, u32 h );
u64			KLTouch_IsEnd( s32 x, s32 y, u32 w, u32 h );
u64			KLTouch_IsWaitingDoubleTap( s32 x, s32 y, u32 w, u32 h );
u64			KLTouch_IsDoubleTap( s32 x, s32 y, u32 w, u32 h );

u64*		KLTouch_IsStartAll( s32 x, s32 y, u32 w, u32 h );
u64*		KLTouch_IsTouchingAll( s32 x, s32 y, u32 w, u32 h );
u64*		KLTouch_IsMoveAll( s32 x, s32 y, u32 w, u32 h );
u64*		KLTouch_IsEndAll( s32 x, s32 y, u32 w, u32 h );
u64*		KLTouch_IsWaitingDoubleTapAll( s32 x, s32 y, u32 w, u32 h );
u64*		KLTouch_IsDoubleTapAll( s32 x, s32 y, u32 w, u32 h );

bl			KLFinger_IsEnd( u64 fingerid, s32 x, s32 y, u32 w, u32 h );

KLFinger*	KLTouch_GetFinger( u64 fingerid );

#include "c_extern_close.h"

#endif
