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

#ifndef ___KLAL_H___
#define ___KLAL_H___

#include <AudioToolbox/AudioServices.h>
#include <AudioToolbox/AudioFile.h>
#include <AudioToolbox/ExtendedAudioFile.h>
#include <AudioToolbox/AudioQueue.h>
#include <AudioToolbox/AudioToolbox.h>

#include "c_extern.h"
#include "KLib.h"
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#define sndid SystemSoundID
#define sndid_err (0)

#define KLAL_WAVE (0)
#define KLAL_RIFF (1)
#define KLAL_M4A (2)

#define KLAL_BUF_NUM (1)
#define KLAL_SRC_NUM (1)

typedef struct{
	
	int	channel;
	int	bit;
	int	sz;
	int	freq;
	
}KLWav;

typedef struct{
	
	ALuint		bufid;
	ALuint		src;
	size_t		totalms;
	u32			formattype;
	u8			current;
	
	char*		pData;
	ALsizei		sz;
    ALenum		format;
    ALsizei		sampleRate;
	bl			isLoaded, firstDelete, retain;
	
	KLWav		wav;
	
}KLAL;

void KLAL_InitSystem();
void KLAL_UnloadSystem();
void KLAL_Init(KLAL*);
bl KLAL_Load( KLAL* p, path8 pPath );
bl KLAL_LoadM4a( KLAL* p, path8 pPath );
bl KLAL_LoadWav( KLAL* p, path8 pPath );
bl KLAL_LoadWavFromTmp( KLAL* p, bin8* pFileName);
bl KLAL_LoadCafFromTmp( KLAL* p, bin8* pFileName);
bl KLAL_Play(KLAL* p, ALfloat sec, bl isLoop );
void KLAL_Stop(KLAL*);
void KLAL_Retain(KLAL* p);
void KLAL_Reload(KLAL* p);
bl KLAL_StopWithCheck(KLAL*,ALfloat stopsec);
void KLAL_Unload(KLAL*);
bl KLAL_IsPlaying(KLAL*);
bl KLAL_IsStopped(KLAL*);
size_t KLAL_GetTotalMs(KLAL*);
void KLAL_SetSecureCounter(KLAL* p);

#include "c_extern_close.h"

#endif