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
#include "KLBench.h"
#include <sys/time.h>

static KLBench kl_bench;

void KLBench_Start(){
	
	gettimeofday(&kl_bench.tvStart, NULL);

}

u64 KLBench_Stop(bl usePrintf){
	
	gettimeofday(&kl_bench.tvEnd, NULL);
	
	kl_bench.lastResult = (kl_bench.tvEnd.tv_sec + kl_bench.tvEnd.tv_usec) - (kl_bench.tvStart.tv_sec + kl_bench.tvStart.tv_usec);
	
	if( usePrintf )
	{
		KLLog("[ KLBench ]\t%f sec\n\t\t\t%f.4 ms\n\t\t\t(%.4fframe)\n\n", kl_bench.lastResult/1000000.0, kl_bench.lastResult/1000.0, kl_bench.lastResult/1000.0/(1000.0/klib.fps) );
	}
	
	return kl_bench.lastResult;
	
}


void KLClock_Start(){
	
	kl_bench.start = clock();
	
}

void KLClock_Stop(){
	
	kl_bench.end		= clock();
	kl_bench.elapsed	= ((double) (kl_bench.end - kl_bench.start)) / CLOCKS_PER_SEC;
	//KLLog("[KLBench] %f msec\n", kl_bench.elapsed*1000 );
	
}

double KLClock_GetSec(){
	
	return kl_bench.elapsed;
	
}

double KLClock_GetMillisec(){
	
	return kl_bench.elapsed*1000;
	
}

double KLClock_GetNanosec(){
	
	return kl_bench.elapsed*1000000000;
	
}


static KLFps kl_fps;

void KLFps_Init(){
	
	kl_fps.count		= 0;
	kl_fps.time			= 0;
	kl_fps.count_before = klib.fps;
	
}

u32 KLFps_Update(){
	
	if( !kl_fps.time )
	{
		kl_fps.time = CFAbsoluteTimeGetCurrent();
	}
	
	kl_fps.count++;
	
	if( CFAbsoluteTimeGetCurrent() - kl_fps.time >= 1.000f )
	{
		
		kl_fps.count_before	= kl_fps.count-1;
		kl_fps.count		= 0;
		kl_fps.time			= 0;
		
	}
	
	return kl_fps.count_before;
	
}

