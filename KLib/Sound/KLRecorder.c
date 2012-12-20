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

#include "KLRecorder.h"
#include <string.h>

// まだしっかり検証ができていないため使用非推奨です。

#if 0

static KLRecorder KL_rec;

void KLRecorder_SystemInit()
{
    OSStatus err = AudioSessionInitialize(NULL, NULL, NULL, NULL);
    err = AudioSessionSetActive(TRUE);
	KLLog("AudioSessionInitialize:%d\n", err);
	
	memset( &KL_rec, 0, sizeof(KLRecorder) );
	
}

static void KLRecorder_InputCallback
(
	void *                               inUserData,
	AudioQueueRef                        inAQ,
	AudioQueueBufferRef                  inBuffer,
	const AudioTimeStamp *               inStartTime,
	UInt32                         	     inNumberPacketDescriptions,
	const AudioStreamPacketDescription   *inPacketDescs
){
    KLRecorder *p =(KLRecorder *)inUserData;
    //inBuffer->mAudioDataは、既に書き込むフォーマットに変換されている
    AudioFileWritePackets(p->audioFileID,
                          FALSE, 
                          inBuffer->mAudioDataByteSize,
                          inPacketDescs,
                          p->startingPacketCount,
                          (UInt32*)&inNumberPacketDescriptions,
                          inBuffer->mAudioData);
    
    p->startingPacketCount += inNumberPacketDescriptions;
    AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
}

inline Bool KLRecorder_IsRecording( KLRecorder* p )
{
	if(!p) p = &KL_rec;
	return p->isRecording;
}

void KLRecorder_ResetBuffer( KLRecorder* p )
{
    p->audioFormat.mSampleRate		= 22050.0f;//44100.0;
    p->audioFormat.mFormatID			= kAudioFormatLinearPCM;
    p->audioFormat.mFormatFlags		= kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked;
    p->audioFormat.mFramesPerPacket	= 1;
    p->audioFormat.mChannelsPerFrame	= 1;
    p->audioFormat.mBitsPerChannel		= 16;
    p->audioFormat.mBytesPerPacket		= 2;
    p->audioFormat.mBytesPerFrame		= 2;
    p->audioFormat.mReserved			= 0;

    OSStatus err = AudioFileCreateWithURL((CFURLRef)p->url, 
										  kAudioFileCAFType, 
										  &p->audioFormat, 
										  kAudioFileFlags_EraseFile, 
										  &p->audioFileID);
    err = AudioQueueNewInput(&p->audioFormat, 
							 KLRecorder_InputCallback, 
							 p, 
							 NULL, 
							 NULL, 
							 0,
							 &p->audioQueueObject);
	
	p->startingPacketCount = 0;
    AudioQueueBufferRef buffers[3];
    
    p->numPacketsToWrite = 1024;
    UInt32 bufferByteSize = p->numPacketsToWrite * p->audioFormat.mBytesPerPacket;
    
    int bufferIndex;
    for(bufferIndex = 0; bufferIndex < 3; ++bufferIndex)
	{
        AudioQueueAllocateBuffer(p->audioQueueObject,
                                 bufferByteSize,
                                 &buffers[bufferIndex]);
        AudioQueueEnqueueBuffer(p->audioQueueObject, buffers[bufferIndex], 0, NULL);
    }
}

void KLRecorder_Init( KLRecorder* p, char* pFileName )
{
	if( !p )
	{
		p = &KL_rec;
	}
	
	sprintf( temp8, "%s%s", KL_GetLib()->strTmpDirPath, pFileName );
	
	p->url = CFURLCreateWithBytes( NULL, (UInt8*)temp8, strlen(temp8), kCFStringEncodingUTF8, NULL );
	
    KLRecorder_ResetBuffer(p);
}

void KLRecorder_Start( KLRecorder* p )
{
	if( !p )
	{
		p = &KL_rec;
	}
	KLRecorder_ResetBuffer(p);
	
	UInt32 sessionCategory = kAudioSessionCategory_PlayAndRecord;
    OSStatus err = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, 
								  sizeof (sessionCategory), 
								  &sessionCategory);
	KLLog("AudioSessionSetProperty:%d\n", err);
		
	p->err = AudioQueueStart(p->audioQueueObject, NULL);
	
    if(p->err)
	{
		KLLog("AudioQueueStart = %d\n", p->err);
	}
	else
	{
		p->isRecording = TRUE;
	}
}

void KLRecorder_Stop(KLRecorder* p)
{
	if( !p )
	{
		p = &KL_rec;
	}

	if(	!p->isRecording )
	{
		return;
	}
	
	p->err = AudioQueueFlush(p->audioQueueObject);
	if(p->err)
	{
		KLLog("AudioQueueFlush = %d\n",p->err);
	}
    
	p->err = AudioQueueStop(p->audioQueueObject, TRUE);    
	if(p->err)
	{
		KLLog("AudioQueueStop = %d\n",p->err);
	}
	
    AudioFileClose(p->audioFileID);
    if(p->err)
	{
		KLLog("AudioFileClose = %d\n",p->err);
	}
	
	AudioQueueDispose(p->audioQueueObject, TRUE);
	if(p->err)
	{
		KLLog("AudioQueueDispose = %d\n",p->err);
	}
	
    UInt32 sessionCategory = kAudioSessionCategory_MediaPlayback;
    AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, 
                            sizeof (sessionCategory), 
                            &sessionCategory);
	
	p->isRecording = FALSE;
		
}

void KLRecorder_Unload( KLRecorder* p )
{
	if( !p )
	{
		p = &KL_rec;
	}
	
	if( p->isRecording )
	{
		KLRecorder_Stop(p);
	}
	UInt32 sessionCategory = kAudioSessionCategory_MediaPlayback;
    AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, 
                            sizeof (sessionCategory), 
                            &sessionCategory);
	
	AudioQueueDispose(p->audioQueueObject, TRUE);
}

#endif