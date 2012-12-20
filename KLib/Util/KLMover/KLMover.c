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

#include "KLMover.h"

void KLMover_Clear( KLMover* pMover ){
	
	pMover->count	= 0;
	pMover->type	= 0;
	pMover->goalType= 0;
	pMover->now		=
	pMover->t		=
	pMover->t2		=
	pMover->start	=
	pMover->final	=
	pMover->rate	= 0;
	pMover->g		=
	pMover->g_inc	=
	pMover->v		= 0.0f;
	pMover->flag	= KLMOVER_IS_PAUSING;
	
}

void KLMover_Init( KLMover* p ){

	KLMover_Clear(p);

}

static void KLMover_UpdateBounceRate( KLMover* p ){
	
	p->rate	*=	2;
	FLG8_Swap( &p->flag, KLMOVER_TO_LEFT );
	
}

void KLMover_BounceReset( KLMover* pMover, s16 start, s16 end ){
	
	pMover->type		= KLMOVER_BOUNCE;
	pMover->start		= 
	pMover->now			= start;
	
	pMover->final		= end;
	pMover->t			= end + ( end - start ) / 2;
	pMover->rate		= 2;
	FLG8_Off( &pMover->flag, KLMOVER_TO_LEFT );
	pMover->t2		= end + (( end - start ) / pMover->rate) * (( pMover->flag&KLMOVER_TO_LEFT)?-1:1);
	
} 

void KLMover_SetupLiner( KLMover* pMover, float start, float vFirst, float target, float rate ){

	pMover->type		= KLMOVER_LINER;
	pMover->now			= start;
	pMover->v			= vFirst;
	pMover->t			= target;
	pMover->rate		= rate;
	
}

void KLMover_SetupGravity( KLMover* pMover, float start, float vFirst, float target, float rate ){
	
	pMover->type	= KLMOVER_GRAVITY;
	pMover->now		= start;
	pMover->v		= vFirst;
	pMover->g		= 0;
	pMover->g_inc	= rate;
	pMover->t		= target;
	
	if( rate<0.0f )
	{
		
		FLG8_On( &pMover->flag, KLMOVER_TO_LEFT );
		
	}
	else
	{
		
		FLG8_Off( &pMover->flag, KLMOVER_TO_LEFT );
		
	}
	
}

void KLMover_SetupDiv( KLMover* pMover, float start, float target, float rate ){
	
	pMover->type		= KLMOVER_DIV;
	pMover->now			= start;
	pMover->t			= target;
	pMover->rate		= rate;
	
}

void KLMover_SetupBounce( KLMover* p, s16 start, s16 end ){

	KLMover_BounceReset( p, start, end );
	
}

void KLMover_Setup( KLMover* pMover, KLMoverType type, float start, float vFirst, float target, float rate ){

	switch( type )
	{

		case KLMOVER_MANUAL:
		case KLMOVER_LINER:
		{
			KLMover_SetupLiner( pMover, start, vFirst, target, rate );
		}
		break;
		
		case KLMOVER_GRAVITY:
		{
			KLMover_SetupGravity( pMover, start, vFirst, target, rate );
		}
		break;
			
		case KLMOVER_DIV:
		{
			KLMover_SetupDiv( pMover, start, target, rate );
		}
		break;
			
		case KLMOVER_BOUNCE:
		{
			KLMover_BounceReset( pMover, start, target );
		}
		break;
	}
	
}

void KLMover_SetType( KLMover* pMover, KLMoverType type ){
	
	pMover->type = type;
	
}

void KLMover_Pause( KLMover* pMover, bl isPause ){
	
	isPause?
		FLG8_On( &pMover->flag, KLMOVER_IS_PAUSING ):
		FLG8_Off( &pMover->flag, KLMOVER_IS_PAUSING );
	
}

inline s32	KLMover_Now( KLMover* pMover ){
	
	return pMover->now;
	
}

void KLMover_Start( KLMover* pMover ){

	pMover->count = 0;
	KLMover_Pause( pMover, FALSE );
	
}

bl KLMover_IsEnd( KLMover* pMover ){
	
	return (pMover->flag&KLMOVER_IS_END)!=0;

}

void KLMover_End( KLMover* pMover ){
	
	pMover->flag |= KLMOVER_IS_END;
	
	switch( pMover->goalType )
	{
	
		case KLMOVER_GOAL_JUST:
		{
			pMover->now = pMover->t;
		}
		break;
			
		default:
		{
		
		}
		break;
			
	}
	
}

void KLMover_Update( KLMover* pMover ){
	
	// 終了していない
	if( !(pMover->flag&(KLMOVER_IS_END|KLMOVER_IS_PAUSING)) ){
		
		if( pMover->count<0xFFFFFFFF )
		{
			++pMover->count;
		}
		
		switch( pMover->type ){
				
			// 手動で終了させるまで終わらない
			case KLMOVER_MANUAL:
			{
				pMover->now += pMover->v;
			}
			break;
				
			// 固定値移動
			case KLMOVER_LINER:
			{
				
				if( pMover->now > pMover->t ){
					
					pMover->now += pMover->v;
					
					if( pMover->now <= pMover->t ){
						
						KLMover_End( pMover );
						
					}
					
				}else if( pMover->now < pMover->t ){
					
					pMover->now += pMover->v;
					
					if( pMover->now >= pMover->t ){
						
						KLMover_End( pMover );
						
					}
					
				}else{
					
					KLMover_End( pMover );
					
				}
				
			}
			break;
				
			// 重力
			case KLMOVER_GRAVITY:
			{
				pMover->g += pMover->g_inc;
				pMover->v += pMover->g;
				pMover->now += pMover->v;
				if( pMover->now >= pMover->t ){
					
					KLMover_End( pMover );
					
				}
				
			}
			break;
				
			// 除算係数
			case KLMOVER_DIV:
			{
				
				pMover->now += (pMover->t-pMover->now) / pMover->rate;
				
				if( (pMover->t-pMover->now)/pMover->rate==0 ){
					
					KLMover_End( pMover );
					
				}
				
			}
			break;
				
			// バウンド
			case KLMOVER_BOUNCE:
			{
				pMover->now = pMover->t - (pMover->t - pMover->now)/2;
				
				if( pMover->now == pMover->t ){
					
					if( pMover->final == pMover->t2 ){
						
						pMover->final = pMover->t2;
						KLMover_End( pMover );
						
					}else{
						
						pMover->t		= pMover->t2;
						pMover->t2	= pMover->final + (( pMover->final - pMover->start ) / pMover->rate) * (( pMover->flag&KLMOVER_TO_LEFT)?-1:1);
						KLMover_UpdateBounceRate( pMover );
						
					}
					
				}
				
			}
			break;	
	
		}
		
	}
	
}
