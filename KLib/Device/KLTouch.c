//
//  KLTouch.c
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

#include "KLib.h"
#include "KLTouch.h"
#include "KLMath.h"
#include "KLHit.h"

#if KLTOUCH_LOGGING
	#define KLTouchLog KLLog
#else
	#define KLTouchLog(...)
#endif

static u32 kltouch_dtapwait = KLTOUCH_WTAP_ALLOW_DELAY;
static bl KLTouch_intercheck = 0;

void KLTouch_Init(){
	
	KLTouch_Reset();
	klib.touch.wtapwait = KLTOUCH_DOUBLETAP_WAIT_DEFAULT;
	
}

static char* KLTouch_GetTypeString( u32 state ){
	
	switch (state)
	{
		case KLFINGER_WAIT:
			return "Wait";
			
		case KLFINGER_TOUCHSTART:
			return "TouchStart";
			
		case KLFINGER_TOUCHING:
			return "Touching";
			
		case KLFINGER_MOVE:
			return "Move";
			
		case KLFINGER_RELEASE:
			return "Release";
			
		case KLFINGER_WAITING_WTAP:
			return "Waiting wtap";
			
		case KLFINGER_WTAP:
			return "Double tap";
			
		default:
			break;
	}
	
	return "Unknown";
	
}


//=======================================================================
/**
 *	@brief	指定IDがリストに存在する場合はそのIDを返す
 *
 *	@param	fingerid	タッチID
 *	@param	isInput		入力バッファが検索対象ならTRUE
 *
 *	@return	リストに存在すればそのインデクス / 存在しなければ KLTOUCH_NOTFOUND_FINGER
 *
 */
//=======================================================================
static s32 KLTouch_GetIndex( u32 fingerid, bl isInput ){
	
	KLFinger*	pFinger = isInput ? klib.touch.input : klib.touch.finger;
	u32			max		= KLTOUCH_MAXFINGER;//isInput ? klib.touch.nInput: klib.touch.nFinger;
	
	if( isInput && KLTouch_intercheck )
	{
		KLLog("[ KLTouch ] Interrupted in updating.\n");
	}
	
	if( max > KLTOUCH_MAXFINGER-1 )
	{
		max = KLTOUCH_MAXFINGER - 1;
	}
	
	for( int i=0; i<max; ++i )
	{
		if( pFinger[i].id==fingerid )
		{
			return i;
		}
	}
	
	return KLTOUCH_NOTFOUND_FINGER;
	
}

//=======================================================================
/**
 *	@brief	タッチリストに空きが存在する場合はそのインデクスを返す
 *
 *	@param	isInput		入力バッファが検索対象ならTRUE
 *
 *	@return	リストに存在すればそのインデクス / 存在しなければ KLTOUCH_NOTFOUND_FINGER
 *
 */
//=======================================================================
static s32 KLTouch_GetEmptyIndex( bl isInput ){
	
	KLFinger*	pFinger = isInput ? klib.touch.input : klib.touch.finger;
	u32			max		= KLTOUCH_MAXFINGER;//isInput ? klib.touch.nInput: klib.touch.nFinger;
	
	if( max > KLTOUCH_MAXFINGER-1 )
	{
		max = KLTOUCH_MAXFINGER - 1;
	}
	
	for( int i=0; i<max; ++i )
	{
		if( pFinger[i].id==0 )
		{
			return i;
		}
	}
	
	return KLTOUCH_NOTFOUND_FINGER;
	
}


//=======================================================================
/**
 *	@brief	指定IDがリストに存在する場合はその指データポインタを返す
 *
 *	@param	fingerid	タッチID
 *
 *	@return	リストに存在すればその指データポインタ / 存在しなければNULL
 *
 */
//=======================================================================
KLFinger* KLTouch_GetFinger( u32 fingerid ){
	
	KLFinger*	pFinger = klib.touch.finger;
	s32			idx		= KLTouch_GetIndex(fingerid, FALSE);
	
	if( idx == KLTOUCH_NOTFOUND_FINGER )
	{
		return NULL;
	}
	
	return &pFinger[idx];
	
}


//===========================================================
/**
 *	@brief	タッチ情報をすべて破棄して起動時の状態に戻す
 */
//===========================================================
void KLTouch_Reset(){

	klmemclear( &klib.touch, KLTouch );

}

void KLTouch_Update(){

	// このループ内で割り込まれてもいいようにコピー
	memcpy( klib.touch.finger, klib.touch.input, sizeof(klib.touch.input) );
	klib.touch.nFinger = 0;//klib.touch.nInput;
	
	KLTouch_intercheck = 1;
	
	for( int i=KLTOUCH_MAXFINGER-1; i>=0; --i )
	{
		KLFinger*	pFinger		= &klib.touch.finger[i];
		KLFinger*	pInputFinger= &klib.touch.input[i];
		bl			frameUpdate	= FALSE;
		bl			count		= 0;
		
		if( pFinger->id * pFinger->flag==0 )
		{
			continue;
		}
		
		
		if( pFinger->flag&(1<<KLFINGER_TOUCHING) )
		{
			
			count = 1;
			frameUpdate = 1;
			pFinger->flag &= 0xFFFFFFFF^(1<<KLFINGER_TOUCHSTART);
			pInputFinger->flag &= 0xFFFFFFFF^(1<<KLFINGER_TOUCHSTART);
			
		}
		
		if( pFinger->flag&(1<<KLFINGER_TOUCHSTART) )
		{
			
			count = 1;
			frameUpdate = 1;
			pFinger->flag |= (1<<KLFINGER_TOUCHING);
			pInputFinger->flag |= (1<<KLFINGER_TOUCHING);
			
		}
		
		if( pFinger->flag&(1<<KLFINGER_MOVE) )
		{
			
			count = 1;
			frameUpdate = 1;
			
		}
		
		if( pFinger->flag&(1<<KLFINGER_WTAP) )
		{
			
			count = 1;
			pFinger->flag |= (1<<KLFINGER_TOUCHING);
			pFinger->flag ^= (1<<KLFINGER_WTAP);
			
			pInputFinger->flag |= (1<<KLFINGER_TOUCHING);
			pInputFinger->flag ^= (1<<KLFINGER_WTAP);
			
			frameUpdate = 1;
			
		}
		
		if( pFinger->flag&(1<<KLFINGER_RELEASE) )
		{
			if( pFinger->wtapWait==0 )
			{
				klmemclear( pFinger, KLFinger );
				klmemclear( pInputFinger, KLFinger );
				
			}
			else
			{
				pFinger->flag = (1<<KLFINGER_WAITING_WTAP);
				pFinger->wtapWait--;
				pInputFinger->flag = (1<<KLFINGER_WAITING_WTAP);
				pInputFinger->wtapWait--;
			}
		}
		
		if( pFinger->flag&(1<<KLFINGER_WAITING_WTAP) )
		{
			if( pFinger->wtapWait==0 )
			{
				
				klmemclear( pFinger, KLFinger );
				klmemclear( pInputFinger, KLFinger );
				
			}
			else
			{
			
				pFinger->flag = (1<<KLFINGER_WAITING_WTAP);
				pFinger->wtapWait--;
				pInputFinger->flag = (1<<KLFINGER_WAITING_WTAP);
				pInputFinger->wtapWait--;
				
			}
		}
		
		if( frameUpdate )
		{
		
			pFinger->frames++;
			pInputFinger->frames++;
		
		}
		
		if( count )
		{
			klib.touch.nFinger++;
		}
		
	}
	
	KLTouch_intercheck = 0;
	
}

void KLTouch_Start( u32 fingerid, s32 tx, s32 ty ){
	
	s32 i			= KLTouch_GetIndex( fingerid, TRUE );
	bl	needreset	= 0;
	
	// リストに存在しない
	if( i==KLTOUCH_NOTFOUND_FINGER )
	{
		
		i = KLTouch_GetEmptyIndex(TRUE);//klib.touch.nInput;
		needreset = 1;
		
	}
	
	// リストがあふれる
	if( i > KLTOUCH_MAXFINGER-1 )
	{
		
		KLTouchLog("[ KLTouch ] Wrong buffer index.\n");
		
	}
	else
	{
		KLFinger* pFinger = &klib.touch.input[i];
		u8 addn = 0;
		
		if( needreset || pFinger->flag==0)
		{
			klmemclear( pFinger, KLFinger );
		}
		
		pFinger->id			= fingerid;
		pFinger->startx		= tx;
		pFinger->starty		= ty;
		
		if(pFinger->flag==0)
		{
			addn = 1;
			KLTouchLog("[KLFinger] %x start x:%d y:%d\n", pFinger->id, pFinger->startx, pFinger->starty );
		}
		else if(pFinger->flag|(1<<KLFINGER_WAITING_WTAP))
		{
			pFinger->flag |= (1<<KLFINGER_WTAP);
			pFinger->flag |= (1<<KLFINGER_TOUCHSTART);
			KLTouchLog("[KLFinger] %x wtap x:%d y:%d\n", pFinger->id, pFinger->startx, pFinger->starty );
		}

		pFinger->flag &= (0xFFFFFFFF^(1<<KLFINGER_WAITING_WTAP));
		pFinger->flag &= (0xFFFFFFFF^(1<<KLFINGER_TOUCHING));
		pFinger->flag &= (0xFFFFFFFF^(1<<KLFINGER_MOVE));
		pFinger->flag &= (0xFFFFFFFF^(1<<KLFINGER_RELEASE));
		pFinger->flag &= (0xFFFFFFFF^(1<<KLFINGER_WAIT));

		pFinger->flag |= (1<<KLFINGER_TOUCHSTART);
		
		/*
		if( pFinger->flag&((1<<KLFINGER_TOUCHSTART)|(1<<KLFINGER_MOVE)|(1<<KLFINGER_WTAP)) )
		{
			pFinger->flag |= (1<<KLFINGER_TOUCHING);
		}
		else
		{
			pFinger->flag |= (1<<KLFINGER_TOUCHSTART);
		}
		*/
		pFinger->wtapWait	= kltouch_dtapwait;
		pFinger->frames		= 0;
		
		klib.touch.nInput	+= addn;
		
		//KLTouchLog("[ KLTouch ] Start %s\n", KLTouch_GetTypeString(pFinger->state) );
		
	}
	
}

void KLTouch_Move( u32 fingerid, s32 tx, s32 ty ){
	
	s32 i			= KLTouch_GetIndex( fingerid, TRUE );
	bl	needreset	= 0;
	
	//KLTouchLog("[ KLTouch ] move %d : %d\n", tx, ty);
	
	if( i==KLTOUCH_NOTFOUND_FINGER )
	{
		
		i = KLTouch_GetEmptyIndex(TRUE);//klib.touch.nInput;
		needreset = 1;
		
	}
	
	if( i > KLTOUCH_MAXFINGER-1 )
	{
		
		KLTouchLog("[ KLTouch ] Wrong buffer index.\n");
		
	}
	else
	{
		
		KLFinger* pFinger = &klib.touch.input[i];
		
		if( needreset )
		{
			klmemclear( pFinger, KLFinger );
			klib.touch.nInput++;
		}
		
		pFinger->id			= fingerid;
		pFinger->movex		= tx;
		pFinger->movey		= ty;
		//pFinger->state		= KLFINGER_MOVE;
		pFinger->flag		|= (1<<KLFINGER_MOVE);
		pFinger->wtapWait	= kltouch_dtapwait;
		
		pFinger->power.dist = KLMath_Distance( pFinger->movex, pFinger->movey, pFinger->startx, pFinger->starty );
		pFinger->power.dir  = KLMath_Dir( pFinger->startx, pFinger->starty, pFinger->movex, pFinger->movey );
		
		//KLTouchLog("[KLFinger] %x Moving x:%d y:%d\n", pFinger->id, pFinger->movex, pFinger->movey);

	}
	
}

void KLTouch_End( u32 fingerid, s32 tx, s32 ty ){

	s32 i = KLTouch_GetIndex( fingerid, TRUE );
	bl	needreset = 0;
	
	if( i==KLTOUCH_NOTFOUND_FINGER )
	{
		
		i = KLTouch_GetEmptyIndex(TRUE);//klib.touch.nInput;
		needreset = 1;
		
	}
	
	if( i > KLTOUCH_MAXFINGER-1 || i<0 )
	{
		
		KLTouchLog("[ KLTouch ] Wrong buffer index.\n");
		
	}
	else
	{
		
		KLFinger* pFinger = &klib.touch.input[i];
		
		if( needreset )
		{
			KLTouchLog("[KLFinger] ?? release ??\n");
			klmemclear( pFinger, KLFinger );
			klib.touch.nInput++;
		}
		
		pFinger->id			= fingerid;
		pFinger->endx		= tx;
		pFinger->endy		= ty;
		//pFinger->state		= pFinger->wtapWait==0? KLFINGER_RELEASE : KLFINGER_WAITING_WTAP;
		pFinger->flag		|= (1<<KLFINGER_RELEASE);
		pFinger->wtapWait	= kltouch_dtapwait;
		
		KLTouchLog("[KLFinger] %x Release x:%d y:%d\n", pFinger->id, pFinger->endx, pFinger->endy);

		if( klib.touch.nInput )
		{
			klib.touch.nInput--;
		}
		
	}
	
}

void KLTouch_Cancel(){
	
	KLTouch_Reset();
	
}

//===========================================================
/**
 *	@brief	指定範囲にタッチ開始イベントが起こったか判定する
 *
 *	@param	x	Ｘ座標
 *	@param	y	Ｙ座標
 *	@param	w	幅
 *	@param	h	高さ
 *
 *	@return	範囲に入っている指があれば最初に発見した指のIDを返す / なければ0
 */
//===========================================================
u32 KLTouch_IsStart( s32 x, s32 y, u32 w, u32 h ){
	
	for( int i=KLTOUCH_MAXFINGER-1; i>=0; --i )
	{
		KLFinger*	pFinger		= &klib.touch.finger[i];
		
		if( pFinger->flag&((1<<KLFINGER_TOUCHSTART)|(1<<KLFINGER_WTAP)) )
		{
			if( KLHit_InRect(pFinger->startx, pFinger->starty, x, y, w, h) )
			{
				return pFinger->id;
			}
		}
		
	}
	
	return 0;
	
}

//===========================================================
/**
 *	@brief	指定範囲にタッチ中のイベントが起こったか判定する
 *
 *	@param	x	Ｘ座標
 *	@param	y	Ｙ座標
 *	@param	w	幅
 *	@param	h	高さ
 *
 *	@return	範囲に該当する指があれば最初に発見した指のIDを返す / なければ0
 */
//===========================================================
u32 KLTouch_IsTouching( s32 x, s32 y, u32 w, u32 h ){
	
	for( int i=KLTOUCH_MAXFINGER-1; i>=0; --i )
	{
		KLFinger*	pFinger		= &klib.touch.finger[i];
		
		if( pFinger->flag&((1<<KLFINGER_TOUCHING)) )
		{
			if( KLHit_InRect(pFinger->startx, pFinger->starty, x, y, w, h) )
			{
				return pFinger->id;
			}
		}
		
	}
	
	return 0;
	
}

//===========================================================
/**
 *	@brief	指定範囲にタッチ移動イベントが起こったか判定する
 *
 *	@param	x	Ｘ座標
 *	@param	y	Ｙ座標
 *	@param	w	幅
 *	@param	h	高さ
 *
 *	@return	範囲に入っている指があれば最初に発見した指のIDを返す / なければ0
 */
//===========================================================
u32 KLTouch_IsMove( s32 x, s32 y, u32 w, u32 h ){
	
	for( int i=KLTOUCH_MAXFINGER-1; i>=0; --i )
	{
		KLFinger*	pFinger		= &klib.touch.finger[i];
		
		if( pFinger->flag&(1<<KLFINGER_MOVE) )
		{
			if( KLHit_InRect(pFinger->movex, pFinger->movey, x, y, w, h) )
			{
				return pFinger->id;
			}
		}
		
	}
	
	return 0;
	
}

//===========================================================
/**
 *	@brief	指定範囲にタッチ離しイベントが起こったか判定する
 *
 *	@param	x	Ｘ座標
 *	@param	y	Ｙ座標
 *	@param	w	幅
 *	@param	h	高さ
 *
 *	@return	範囲内から離した指があれば最初に発見した指のIDを返す / なければ0
 */
//===========================================================
u32 KLTouch_IsEnd( s32 x, s32 y, u32 w, u32 h ){
	
	for( int i=KLTOUCH_MAXFINGER-1; i>=0; --i )
	{
		KLFinger*	pFinger		= &klib.touch.finger[i];
		
		if( pFinger->flag&(1<<KLFINGER_RELEASE) )
		{
			if( KLHit_InRect(pFinger->endx, pFinger->endy, x, y, w, h) )
			{
				return pFinger->id;
			}
		}
		
	}
	
	return 0;
	
}

//===========================================================
/**
 *	@brief	指定範囲にダブルタップを待っている状態の指があるか判定する
 *
 *	@param	x	Ｘ座標
 *	@param	y	Ｙ座標
 *	@param	w	幅
 *	@param	h	高さ
 *
 *	@return	ダブルタップを待っている指が範囲内にあれば最初に発見した指のIDを返す / なければ0
 */
//===========================================================
u32 KLTouch_IsWaitingDoubleTap( s32 x, s32 y, u32 w, u32 h ){
	
	for( int i=KLTOUCH_MAXFINGER-1; i>=0; --i )
	{
		KLFinger*	pFinger		= &klib.touch.finger[i];
		
		if( pFinger->state&(1<<KLFINGER_WAITING_WTAP) )
		{
			if( KLHit_InRect(pFinger->endx, pFinger->endy, x, y, w, h) )
			{
				return pFinger->id;
			}
		}
		
	}
	
	return 0;
	
}


//===========================================================
/**
 *	@brief	指定範囲にダブルタップイベントが起こったか判定する
 *
 *	@param	x	Ｘ座標
 *	@param	y	Ｙ座標
 *	@param	w	幅
 *	@param	h	高さ
 *
 *	@return	範囲内に該当する指があれば最初に発見した指のIDを返す / なければ0
 */
//===========================================================
u32 KLTouch_IsDoubleTap( s32 x, s32 y, u32 w, u32 h ){
	
	
	for( int i=KLTOUCH_MAXFINGER-1; i>=0; --i )
	{
		KLFinger*	pFinger		= &klib.touch.finger[i];
		
		if( pFinger->state&(1<<KLFINGER_WTAP) )
		{
			if( KLHit_InRect(pFinger->startx, pFinger->starty, x, y, w, h) )
			{
				return pFinger->id;
			}
		}
		
	}
	
	return 0;
	
}


static u32 retid[KLTOUCH_MAXFINGER+1];
//===========================================================
/**
 *	@brief	指定範囲にタッチ開始イベントが起こったか判定し指IDを配列で返す
 *
 *	@param	x	Ｘ座標
 *	@param	y	Ｙ座標
 *	@param	w	幅
 *	@param	h	高さ
 *
 *	@return	範囲に入っている指があれば指ID配列を返す(末尾0) / なければNULL
 */
//===========================================================
u32* KLTouch_IsStartAll( s32 x, s32 y, u32 w, u32 h ){
	
	u8	next = 0;
	klmemclear( retid, retid );
	
	for( int i=KLTOUCH_MAXFINGER-1; i>=0; --i )
	{
		KLFinger*	pFinger		= &klib.touch.finger[i];
		
		if( pFinger->flag&((1<<KLFINGER_TOUCHSTART)|(1<<KLFINGER_WTAP)) )
		{
			if( KLHit_InRect(pFinger->startx, pFinger->starty, x, y, w, h) )
			{
				retid[next++] = pFinger->id;
			}
		}
		
	}
	
	return next? retid : NULL;
	
}

//===========================================================
/**
 *	@brief	指定範囲にタッチ中の指があるか判定し指IDを配列で返す
 *
 *	@param	x	Ｘ座標
 *	@param	y	Ｙ座標
 *	@param	w	幅
 *	@param	h	高さ
 *
 *	@return	範囲に入っている指があれば指ID配列を返す(末尾0) / なければNULL
 */
//===========================================================
u32* KLTouch_IsTouchingAll( s32 x, s32 y, u32 w, u32 h ){
	
	u8	next = 0;
	klmemclear( retid, retid );
	
	for( int i=KLTOUCH_MAXFINGER-1; i>=0; --i )
	{
		KLFinger*	pFinger		= &klib.touch.finger[i];
		
		if( pFinger->flag&((1<<KLFINGER_TOUCHING)) )
		{
			if( KLHit_InRect(pFinger->startx, pFinger->starty, x, y, w, h) )
			{
				retid[next++] = pFinger->id;
			}
		}
		
	}
	
	return next ? retid : NULL;
	
}

//===========================================================
/**
 *	@brief	指定範囲にタッチ移動イベントが起こったか判定し指IDを配列で返す
 *
 *	@param	x	Ｘ座標
 *	@param	y	Ｙ座標
 *	@param	w	幅
 *	@param	h	高さ
 *
 *	@return	範囲に入っている指があれば指ID配列を返す(末尾0) / なければNULL
 */
//===========================================================
u32* KLTouch_IsMoveAll( s32 x, s32 y, u32 w, u32 h ){
	
	u8	next = 0;
	klmemclear( retid, retid );
	
	for( int i=KLTOUCH_MAXFINGER-1; i>=0; --i )
	{
		KLFinger*	pFinger		= &klib.touch.finger[i];
		
		if( pFinger->flag&(1<<KLFINGER_MOVE) )
		{
			if( KLHit_InRect(pFinger->movex, pFinger->movey, x, y, w, h) )
			{
				retid[next++] = pFinger->id;
			}
		}
		
	}
	
	return next? retid : NULL;
	
}

//===========================================================
/**
 *	@brief	指定範囲にタッチ離しイベントが起こったか判定し指IDを配列で返す
 *
 *	@param	x	Ｘ座標
 *	@param	y	Ｙ座標
 *	@param	w	幅
 *	@param	h	高さ
 *
 *	@return	範囲から離した指があれば指ID配列を返す(末尾0) / なければNULL
 */
//===========================================================
u32* KLTouch_IsEndAll( s32 x, s32 y, u32 w, u32 h ){
	
	u8	next = 0;
	klmemclear( retid, retid );
	
	for( int i=KLTOUCH_MAXFINGER-1; i>=0; --i )
	{
		KLFinger*	pFinger		= &klib.touch.finger[i];
		
		if( pFinger->flag&(1<<KLFINGER_RELEASE) )
		{
			if( KLHit_InRect(pFinger->endx, pFinger->endy, x, y, w, h) )
			{
				retid[next++] = pFinger->id;
			}
		}
		
	}
	
	return next? retid : NULL;
	
}

//===========================================================
/**
 *	@brief	指定範囲にダブルタップ待ちの指があるか判定し指IDを配列で返す
 *
 *	@param	x	Ｘ座標
 *	@param	y	Ｙ座標
 *	@param	w	幅
 *	@param	h	高さ
 *
 *	@return	範囲内に該当する指があれば指ID配列を返す(末尾0) / なければNULL
 */
//===========================================================
u32* KLTouch_IsWaitingDoubleTapAll( s32 x, s32 y, u32 w, u32 h ){
	
	u8	next = 0;
	klmemclear( retid, retid );
	
	for( int i=KLTOUCH_MAXFINGER-1; i>=0; --i )
	{
		KLFinger*	pFinger		= &klib.touch.finger[i];
		
		if( pFinger->state&(1<<KLFINGER_WAITING_WTAP) )
		{
			if( KLHit_InRect(pFinger->endx, pFinger->endy, x, y, w, h) )
			{
				retid[next++] = pFinger->id;
			}
		}
		
	}
	
	return next? retid : NULL;
	
}

//===========================================================
/**
 *	@brief	指定範囲にダブルタップした指があるか判定し指IDを配列で返す
 *
 *	@param	x	Ｘ座標
 *	@param	y	Ｙ座標
 *	@param	w	幅
 *	@param	h	高さ
 *
 *	@return	範囲内に該当する指があれば指ID配列を返す(末尾0) / なければNULL
 */
//===========================================================
u32* KLTouch_IsDoubleTapAll( s32 x, s32 y, u32 w, u32 h ){
	
	u8	next = 0;
	klmemclear( retid, retid );
	
	for( int i=KLTOUCH_MAXFINGER-1; i>=0; --i )
	{
		KLFinger*	pFinger		= &klib.touch.finger[i];
		
		if( pFinger->state&(1<<KLFINGER_WTAP) )
		{
			if( KLHit_InRect(pFinger->startx, pFinger->starty, x, y, w, h) )
			{
				retid[next++] = pFinger->id;
			}
		}
		
	}
	
	return next? retid : NULL;
	
}



//===========================================================
/**
 *	@brief	指IDを指定して範囲にタッチ離しイベントが起こったか判定する
 *
 *	@param	fingerid	指ID(自分でタッチ開始判定の戻り値を保持しておく必要あり)
 *	@param	x			Ｘ座標
 *	@param	y			Ｙ座標
 *	@param	w			幅
 *	@param	h			高さ
 *
 *	@return	範囲に入っていればTRUE / なければFALSE
 */
//===========================================================
bl KLFinger_IsEnd( u32 fingerid, s32 x, s32 y, u32 w, u32 h ){
	
	s32 idx = KLTouch_GetIndex( fingerid, FALSE );
	
	if( idx!=KLTOUCH_NOTFOUND_FINGER )
	{
		KLFinger*	pFinger		= &klib.touch.finger[idx];
		
		if( pFinger->id==fingerid )
		{
			if( pFinger->flag&(1<<KLFINGER_RELEASE) )
			{
				if( x+y+w+h==0 )
				{
					return TRUE;
				}
				else if( KLHit_InRect(pFinger->endx, pFinger->endy, x, y, w, h) )
				{
					return TRUE;
				}
			}
		}
	}
	
	return FALSE;
	
}
