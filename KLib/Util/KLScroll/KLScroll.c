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

#include "KLScroll.h"
#include "KLTouch.h"
#include "KLMath.h"
#include "KLHit.h"

void KLScroll_Init( KLScroll* p, s32 touch_areax, s32 touch_areay, u32 touch_areaw, u32 touch_areah, u32 vieww, u32 viewh )
{
	p->tid	=
	p->x	= p->y =
	p->w	= p->h =
	p->vieww= p->viewh	=
	p->axisx= p->axisy	=
	p->sx	= p->sy		=
	p->oldtx= p->oldty	=
	p->oldsx= p->oldsy	=
	p->slipx= p->slipy	=
	p->vx	= p->vy		= p->tvx = p->tvy =
	p->onFinger = 0;
	
	p->x = touch_areax;
	p->y = touch_areay;
	p->w = touch_areaw;
	p->h = touch_areah;
	p->vieww = vieww<touch_areaw ? touch_areaw :vieww;
	p->viewh = viewh<touch_areah ? touch_areah :viewh;
	p->scrollablex = touch_areaw<vieww;
	p->scrollabley = touch_areah<viewh;
	p->onFinger = FALSE;
	
}

#define KLSCROLL_RATE (0.8f)

void KLScroll_Update( KLScroll* p )
{
	u32 startid	= KLTouch_IsStart(0, 0, klib.device.w, klib.device.h);
	u32 moveid	= KLTouch_IsMove(0,0,klib.device.w,klib.device.h);
	
	p->vx *= KLSCROLL_RATE;
	p->vy *= KLSCROLL_RATE;

	if( p->tvx )
	{
		p->tvx *= 0.5f;
	}
	
	if( p->tvy )
	{
		p->tvy *= 0.5f;
	}
	
	if( KLTouch_IsEnd(0,0,klib.device.w,klib.device.h) )
	{
		p->onFinger = FALSE;
		
		if( p->tvx || p->tvy )
		{
			p->vx = p->tvx;
			p->vy = p->tvy;
			
			p->tvx = 0;
			p->tvy = 0;
		}
	}
	else if( startid+moveid )
	{
		s32 tx = 0;
		s32 ty = 0;
		
		if( startid )
		{
			KLFinger* pFinger = KLTouch_GetFinger(startid);
			tx = pFinger->startx;
			ty = pFinger->starty;
			
			if( KLHit_InRect(tx, ty, p->x, p->y, p->w, p->h) )
			{
				p->oldsx = p->sx;
				p->oldsy = p->sy;
				
				p->axisx = tx;
				p->axisy = ty;
				p->oldtx = tx;
				p->oldty = ty;
				p->vx = p->vy = 0;
				p->tvx = p->tvy = 0;
				p->onFinger = TRUE;
				
				KLLog("Touched %d / %d\n", tx,ty);
			}
			
		}
		else if( moveid && p->onFinger )
		{
			KLFinger* pFinger = KLTouch_GetFinger(moveid);
			
			tx = pFinger->movex;
			ty = pFinger->movey;
			s32 mx = ( p->w>=p->vieww ) ? 0 : tx-p->axisx;
			s32 my = ( p->h>=p->viewh ) ? 0 : ty-p->axisy;
			
			if( KLMath_Distance( tx, ty, p->oldtx, p->oldty )>2 )
			{
				p->slipx = ( p->w>=p->vieww || !p->scrollablex ) ? 0 : p->oldtx - tx;
				p->slipy = ( p->h>=p->viewh || !p->scrollabley ) ? 0 : p->oldty - ty;
				
				p->oldtx = p->scrollablex ? tx : 0;
				p->oldty = p->scrollabley ? ty : 0;
				
				p->tvx = (s32)(p->slipx*0.75f);
				p->tvy = (s32)(p->slipy*0.75f);
			}
			
			p->sx = p->scrollablex ? (mx+p->oldsx) : 0;
			p->sy = p->scrollabley ? (my+p->oldsy) : 0;
			
			KLLog("Moved %d / %d\n", mx,my);
			
		}
	}
	else
	{
		p->onFinger = FALSE;
		
		if( p->tvx || p->tvy )
		{
			p->vx = p->tvx;
			p->vy = p->tvy;
			
			p->tvx = 0;
			p->tvy = 0;
		}
	}
	
	// エリア外からの復帰
	if( !p->onFinger )
	{
		if( p->sx > 0 )
		{
			p->vx = -p->sx/3.5f;
		}
		if( p->sy > 0 )
		{
			p->vy = -p->sy/3.5f;
		}
		
		if( p->w<p->vieww && p->sx+p->vieww < p->w-1 )
		{
			p->vx = (p->w-(p->sx+p->vieww))/3.5f;
		}
		if( p->h<p->viewh && p->sy+p->viewh < p->h-1 )
		{
			p->vy = (p->h-(p->sy+p->viewh))/3.5f;
		}
	}
	
	if( p->vx>-1.0f && p->vx<1.0f )
	{
		p->vx = 0;
	}
	
	if( p->vy>-1.0f && p->vy<1.0f )
	{
		p->vy = 0;
	}
		
	p->sx += p->vx;
	p->sy += p->vy;
	
}

inline s32 KLScroll_GetX( KLScroll* p )
{
	return (s32)p->sx;
}

inline s32 KLScroll_GetY( KLScroll* p )
{
	return (s32)p->sy;
}

