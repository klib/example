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

#include "KLAL.h"
#include "KLFile.h"
#include "KLString.h"

static KLAL			klal;
static ALCdevice*	klal_pDevice	= NULL;
static ALCcontext*	klal_pContext	= NULL;
static bl			klal_is_loaded	= FALSE;

#define Swap32Endian(v) v = (((v)&0xFF)<<24)|(((v)&0xFF00)<<8)|(((v)&0xFF0000)>>8)|(((v)&0xFF000000)>>24)

void KLAL_RebuildBuffer(KLAL* p);

static void* GetOpenALAudioData( KLAL* p,
	CFURLRef fileURL, ALsizei* dataSize, ALenum* dataFormat, ALsizei *sampleRate)
{
    OSStatus    	err;
    UInt32			size;
	UInt32			bufferSize;
    void*           data = NULL;
    AudioBufferList dataBuffer;
	
    // オーディオファイルを開く
    ExtAudioFileRef audioFile;
    err = ExtAudioFileOpenURL(fileURL, &audioFile);
    
	if (err)
	{
		KLLog("[KLAL] GetOpenALAudioData(A):%d\n",(int)err);
        goto Exit;
    }
	
	//CFRelease(fileURL);
	
    // オーディオデータフォーマットを取得する
    AudioStreamBasicDescription fileFormat;
    size = sizeof(fileFormat);
    err = ExtAudioFileGetProperty(audioFile, kExtAudioFileProperty_FileDataFormat, &size, &fileFormat);
    if (err)
	{
		KLLog("[KLAL] GetOpenALAudioData(B):%d\n",(int)err);
		goto Exit;
    }
	
    // アウトプットフォーマットを設定する
    AudioStreamBasicDescription outputFormat;
    outputFormat.mSampleRate = fileFormat.mSampleRate;
    outputFormat.mChannelsPerFrame = fileFormat.mChannelsPerFrame;
    outputFormat.mFormatID = kAudioFormatLinearPCM;
    outputFormat.mBytesPerPacket = 2 * outputFormat.mChannelsPerFrame;
    outputFormat.mFramesPerPacket = 1;
    outputFormat.mBytesPerFrame = 2 * outputFormat.mChannelsPerFrame;
    outputFormat.mBitsPerChannel = 16;//fileFormat.mBitsPerChannel;
    outputFormat.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
    
	p->wav.channel	= outputFormat.mChannelsPerFrame;
	p->wav.bit		= fileFormat.mBitsPerChannel;
	p->wav.freq		= fileFormat.mSampleRate;
	
	err = ExtAudioFileSetProperty(
								  audioFile, kExtAudioFileProperty_ClientDataFormat, sizeof(outputFormat), &outputFormat);
    if (err)
	{
		KLLog("[KLAL] GetOpenALAudioData(C):%d\n",(int)err);
		goto Exit;
    }
	
    // フレーム数を取得する
    SInt64  fileLengthFrames = 0;
    size = sizeof(fileLengthFrames);
    err = ExtAudioFileGetProperty(
								  audioFile, kExtAudioFileProperty_FileLengthFrames, &size, &fileLengthFrames);
    if (err)
	{
		KLLog("[KLAL] GetOpenALAudioData(D):%d\n",(int)err);
		goto Exit;
	}
	
    // バッファを用意する
    bufferSize = (UInt32) fileLengthFrames * (UInt32) outputFormat.mBytesPerFrame;
    data = Malloc(bufferSize);
    dataBuffer.mNumberBuffers = 1;
    dataBuffer.mBuffers[0].mDataByteSize = bufferSize;
    dataBuffer.mBuffers[0].mNumberChannels = outputFormat.mChannelsPerFrame;
    dataBuffer.mBuffers[0].mData = data;
	p->wav.sz = bufferSize; 
	
    // バッファにデータを読み込む
    err = ExtAudioFileRead(audioFile, (UInt32*)&fileLengthFrames, &dataBuffer);
    if (err)
	{
		KLLog("[KLAL] GetOpenALAudioData(E):%d\n",(int)err);
		sfree(data);
		goto Exit;
    }
	
    // 出力値を設定する
    *dataSize = (ALsizei)bufferSize;
    *dataFormat = (outputFormat.mChannelsPerFrame > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
    *sampleRate = (ALsizei)outputFormat.mSampleRate;
	
Exit:
    // オーディオファイルを破棄する
    if (audioFile)
	{
		ExtAudioFileDispose(audioFile);
    }
		
    return data;
}

int CAF_ReadHeader(FILE* fp, int *channel, int* bit, int *size, int* freq, bin8* pData, u32* pBodyOffset, size_t* pTotalms, size_t filesize )
{	
	int res32;
	s64 chunkSize;
	int nonfp_offset = 0;
	u16	version;
	u32 bitPerSample;
	double samplingRate;
	int bytePerSec = 8;
	
	int flag = 0;
	
	UInt32  mFormatID;
	UInt32  mFormatFlags;
	UInt32  mFramesPerPacket;
	UInt32  mChannelsPerFrame;
	
	if( fp )
	{
		fread(&res32, 4, 1,fp);
	}
	else
	{
		if(!pData)
		{
			return 1;
		}
		memcpy( &res32, pData, 4 );
		nonfp_offset+=4;
	}
	
	// "caff"
	if(res32 != 0x66666163)
	{
		return 1;	//error 1
	}
	
	// バージョン
	if( fp )
	{
		fread(&version, 2, 1, fp);
	}
	else
	{
		memcpy(&version, pData+nonfp_offset, 2 );
		nonfp_offset += 2;
	}
	
	// フラグ（処理的に使わないので無視）
	if( fp )
	{
		fread(&flag, 2, 1, fp);
	}
	else
	{
		memcpy(&flag, pData+nonfp_offset, 2 );
		nonfp_offset += 2;	
	}
	
	while(nonfp_offset<filesize)
	{
		//チャンクの読み
		if( fp )
		{
			fread(&res32, 4, 1, fp);
			fread(&chunkSize, 8, 1, fp);
			nonfp_offset += 12;
		}
		else
		{
			u64 chunksize2=0;
			memcpy( &res32, pData+nonfp_offset, 4 );
			nonfp_offset += 4;
			
			memcpy( &chunksize2, pData+nonfp_offset, 1 );
			nonfp_offset += 1;
			chunkSize = (chunksize2&0xFF)<<56;
			memcpy( &chunksize2, pData+nonfp_offset, 1 );
			nonfp_offset += 1;
			chunkSize |= (chunksize2&0xFF)<<48;
			memcpy( &chunksize2, pData+nonfp_offset, 1 );
			nonfp_offset += 1;
			chunkSize |= (chunksize2&0xFF)<<40;
			memcpy( &chunksize2, pData+nonfp_offset, 1 );
			nonfp_offset += 1;
			chunkSize |= (chunksize2&0xFF)<<32;
			memcpy( &chunksize2, pData+nonfp_offset, 1 );
			nonfp_offset += 1;
			chunkSize |= (chunksize2&0xFF)<<24;
			memcpy( &chunksize2, pData+nonfp_offset, 1 );
			nonfp_offset += 1;
			chunkSize |= (chunksize2&0xFF)<<16;
			memcpy( &chunksize2, pData+nonfp_offset, 1 );
			nonfp_offset += 1;
			chunkSize |= (chunksize2&0xFF)<<8;
			memcpy( &chunksize2, pData+nonfp_offset, 1 );
			nonfp_offset += 1;
			chunkSize |= (chunksize2&0xFF);
		}
		
		switch(res32)
		{
			// "desc"
			case 0x63736564:
				
				if( fp )
				{
					//サンプリングレート
					fread(&samplingRate, 8, 1, fp);
					fread(&mFormatID, 4, 1, fp);
					fread(&mFormatFlags, 4, 1, fp);
					fread(&bytePerSec, 4, 1, fp);
					fread(&mFramesPerPacket, 4, 1, fp);
					fread(&mChannelsPerFrame, 4, 1, fp);
					fread(&bitPerSample, 4, 1, fp);
					nonfp_offset += 8+4+4+4+4+4+4;
				}
				else
				{
					u8 s1[4]={0,0,0,0};
					u8 s2[4]={0,0,0,0};
					memcpy( &s1, pData+nonfp_offset, 4 );
					nonfp_offset += 4;
					memcpy( &s2, pData+nonfp_offset, 4 );
					nonfp_offset += 4;
					
					samplingRate = 22050.0f;//44100.0;//(s1[0]<<8)|(s1[1]);
					
					//memcpy( &samplingRate, pData+nonfp_offset, 8 );
					//nonfp_offset += 8;
					//Float64
					memcpy( &mFormatID, pData+nonfp_offset, 4 );
					nonfp_offset += 4;
					
					memcpy( &mFormatFlags, pData+nonfp_offset, 4 );
					nonfp_offset += 4;
					Swap32Endian(mFormatFlags);
					
					memcpy( &bytePerSec, pData+nonfp_offset, 4 );
					nonfp_offset += 4;
					Swap32Endian(bytePerSec);
					
					memcpy( &mFramesPerPacket, pData+nonfp_offset, 4 );
					nonfp_offset += 4;
					Swap32Endian(mFramesPerPacket);
					
					memcpy( &mChannelsPerFrame, pData+nonfp_offset, 4 );
					nonfp_offset += 4;
					Swap32Endian(mChannelsPerFrame);
					
					memcpy( &bitPerSample, pData+nonfp_offset, 4 );
					nonfp_offset += 4;
					Swap32Endian(bitPerSample);
				}
				
				*channel = (int)mChannelsPerFrame;
				*bit = (int)bitPerSample;
				*freq = samplingRate;
				
				break;
				
			default:
				
				if( fp )
				{
					fseek( fp, (long)chunkSize, SEEK_CUR );
					nonfp_offset += chunkSize;
				}
				else
				{
					nonfp_offset += chunkSize;
				}
				break;

			// "data"
			case  0x61746164:
				*size = (int) chunkSize;
				if( fp )
				{
					ftell(fp);
				}
				else
				{
					//nonfp_offset;
				}
				
				if( !fp && pBodyOffset)
				{
					*pBodyOffset = nonfp_offset;
				}
				nonfp_offset += chunkSize;
				
				if( pTotalms )
				{
					*pTotalms = (UInt32) chunkSize / (UInt32) bytePerSec;
				}
				break;
				
		}
		
	}
	
	return 0;
	
}

int WAV_ReadHeader(FILE* fp, int *channel, int* bit, int *size, int* freq, bin8* pData, u32* pBodyOffset, size_t* pTotalms )
{	
	short	res16;
	int		res32;
	int		dataSize, chunkSize = 0;
	int		nonfp_offset = 0;
	short	channelCnt, bitPerSample, blockSize;
	int		samplingRate, bytePerSec = 8;
	
	long dataPos = 0;
	int flag = 0;
	
	if( fp )
	{
		fread(&res32, 4, 1,fp);
	}
	else
	{
		if(!pData)
		{
			return 1;
		}
		memcpy( &res32, pData, 4 );
		nonfp_offset+=4;
	}
	
	// "RIFF"
	if(res32 != 0x46464952)
	{
		return 1;	//error 1
	}
	
	//データサイズ = ファイルサイズ - 8 byte の取得
	if( fp )
	{
		fread(&dataSize, 4, 1, fp);
	}
	else
	{
		memcpy(&dataSize, pData+nonfp_offset, 4 );
		nonfp_offset += 4;
	}
	
	//WAVEヘッダーの読み
	if( fp )
	{
		fread(&res32, 4, 1, fp);
	}
	else
	{
		memcpy(&res32, pData+nonfp_offset, 4);
		nonfp_offset += 4;
	}
	
	// "WAVE"
	if(res32 != 0x45564157)
	{
		return 2;	//error 2
	}
	
	while(flag != 3)
	{
		//チャンクの読み
		if( fp )
		{
			fread(&res32, 4, 1, fp);
			fread(&chunkSize, 4, 1, fp);
		}
		else
		{
			memcpy( &res32, pData+nonfp_offset, 4 );
			nonfp_offset += 4;
			memcpy( &chunkSize, pData+nonfp_offset, 4 );
			nonfp_offset += 4;
		}
		
		switch(res32)
		{
			// "bext"
			case 0x74786562:
			default:
			
				if( fp )
				{
					fseek( fp, chunkSize, SEEK_CUR );
				}
				else
				{
					nonfp_offset += chunkSize;
				}
				break;
			
			// "fmt"
			case 0x20746d66:
				//format 読み込み
				//PCM種類の取得
				if( fp )
				{
					fread(&res16, 2, 1, fp);
				}
				else
				{
					memcpy(&res16, pData+nonfp_offset, 2);
					nonfp_offset += 2;
				}
				
				if(res16 != 1)
				{
					//非対応フォーマット
					return 4;
				}
				
				//モノラル(1)orステレオ(2)
				if( fp )
				{
					fread(&channelCnt, 2, 1, fp);
				}
				else
				{
					memcpy( &channelCnt, pData+nonfp_offset, 2 );
					nonfp_offset += 2;
				}
				
				if(res16 > 2)
				{
					//チャンネル数間違い
					return 5;
				}
				
				if( fp )
				{
					//サンプリングレート
					fread(&samplingRate, 4, 1, fp);
					
					//データ速度(byte/sec)=サンプリングレート*ブロックサイズ
					fread(&bytePerSec, 4, 1, fp);
					
					//ブロックサイズ(byte/sample)=チャンネル数*サンプルあたりのバイト数
					fread(&blockSize, 2, 1, fp);
					
					//サンプルあたりのbit数(bit/sample)：8 or 16
					fread(&bitPerSample, 2, 1, fp);
					
					if( bitPerSample!=8 && bitPerSample!=16 )
					{
						KLLog("[KLAL] !! Not supported wave format !!\nSupported bitPerSample is 8 or 16 only!\n\n");
					}
					
					if( chunkSize>16 )
					{
						fseek( fp, chunkSize-16, SEEK_CUR );
					}
					
					KLLog("[KLAL] Loaded wav header:\n\tsamplingRate:%u\n\tbytePerSec:%u\n\tblockSize:%u\n\tbitPerSample:%u\n\n", samplingRate, bytePerSec, blockSize, bitPerSample );
				}
				else
				{
					memcpy( &samplingRate, pData+nonfp_offset, 4 );
					nonfp_offset += 4;
					
					memcpy( &bytePerSec, pData+nonfp_offset, 4 );
					nonfp_offset += 4;
					
					memcpy( &blockSize, pData+nonfp_offset, 2 );
					nonfp_offset += 2;
					
					memcpy( &bitPerSample, pData+nonfp_offset, 2 );
					nonfp_offset += 2;
				}
				
				if( pTotalms )
				{
					*pTotalms = 1000 * dataSize / bytePerSec;
				}
				
				*channel = (int)channelCnt;
				*bit = (int)bitPerSample;
				*freq = samplingRate;
				
				flag += 1;
				
				break;
				
			// "data"
			case  0x61746164:
				
				*size = chunkSize;
				dataPos = fp ? ftell(fp): nonfp_offset;
				flag += 2;
				break;
		}
		
	}
	
	//頭出し("fmt "が"data"より後にあった場合のみ動く)
	if( fp )
	{
		if(dataPos != ftell(fp))
		{
			fseek(fp,dataPos,SEEK_SET);
		}
	}
	else
	{
		if(dataPos != nonfp_offset && pBodyOffset)
		{
			*pBodyOffset = nonfp_offset;
		}
	}
	
	return 0;
	
}

bl KLAL_SetCaf(KLAL* p, bin8* pData, size_t len)
{
	u32 bodyoffset = 0;
	
	if( !p )
	{
		p = &klal;
	}
	
	if( !pData )
	{
		KLLog("[ KLAL_SetCaf ] null error.\n");
		return FALSE;
	}
	
	CAF_ReadHeader(
		NULL, &p->wav.channel, &p->wav.bit, &p->wav.sz, &p->wav.freq,
		pData, &bodyoffset, &p->totalms, len );
	
	p->pData = (char*)realloc( p->pData, p->wav.sz );
	if( !p->pData )
	{
		KLLog("[ KLAL_SetCaf ] alloc error.\n");
		return FALSE;	
	}
	
	KLLog("[ CAF ] body:%d sz:%d\n", bodyoffset, p->wav.sz );
	memcpy( p->pData, pData+bodyoffset, p->wav.sz );
	return TRUE;
}

inline size_t KLAL_GetTotalMs(KLAL* p)
{
	return p? p->totalms : klal.totalms;
}

bl KLAL_SetWav(KLAL* p, bin8* pData)
{
	u32 bodyoffset = 0;
	
	if( !p )
		p = &klal;
	
	WAV_ReadHeader( NULL, &p->wav.channel, &p->wav.bit, &p->wav.sz, &p->wav.freq, pData, &bodyoffset, &p->totalms );
	
	p->pData = (char*)realloc( p->pData, p->wav.sz );
	if( !p->pData )
	{
		KLLog("[ KLAL_SetWav ] alloc error.\n");
		return FALSE;	
	}
	return TRUE;
}

bl KLAL_LoadWavCore(KLAL* p, path8 pPath)
{		
	size_t	sz;
	FILE*	fp;
	
	if( !p )
		p = &klal;
	
	fp = KLFile_Open( pPath, &sz );
	
	if( !fp )
	{
		return FALSE;
	}
	
	WAV_ReadHeader( fp, &p->wav.channel, &p->wav.bit, &p->wav.sz, &p->wav.freq, NULL, NULL, &p->totalms );
	p->pData = (char*)realloc( p->pData, p->wav.sz );
	if( !p->pData )
	{
		KLLog("[ KLAL_LoadWav ] alloc error.\n");
	}
	fread( p->pData, p->wav.sz, 1, fp );
	fclose(fp);
		
	return TRUE;
}

bl KLAL_LoadM4a( KLAL* p, path8 pPath )
{
	char	path[256];
	sprintf( path, "%s/%s", klib.path.appRoot, pPath );
	
	CFURLRef url = KLFile_CharToURLRef(path);
	
	ALsizei sz = 0, sampleRate = 0;
	ALenum	format;
	bl ret = FALSE;
	
	if( !p )
		p = &klal;
	
	KLAL_Stop(p);
	alSourcei( p->src, AL_BUFFER, 0 );
	sfree(p->pData);
	p->pData = GetOpenALAudioData( p, url, &sz, &format, &sampleRate );
	
	//CFRelease(url);
	
	ret = (p->pData != NULL);
	
	if( ret )
	{
		p->firstDelete = FALSE;
		p->formattype = KLAL_M4A;
	}
	p->isLoaded = FALSE;
	
	sCFRelease(url);
	
	
	return ret;
}

void KLAL_RebuildBuffer(KLAL* p)
{	
	if( !p )
		p = &klal;
	
	p->current = 0;//(p->current+1)%KLAL_AL_RINGNUM;
	
	if(p->bufid)
	{
		alSourcei( p->src, AL_BUFFER, 0 );
#if KLAL_ERRCHECK
		ALenum aen = alGetError();
		if( aen!=AL_NO_ERROR )
		{
			KLLog("[ KLAL ] %s %u\n", __func__, aen);
		}
#endif
		alDeleteBuffers( 1, &p->bufid );
#if KLAL_ERRCHECK
		aen = alGetError();
		if( aen!=AL_NO_ERROR )
		{
			KLLog("[ KLAL ] %s %u\n", __func__, aen);
		}
#endif
		p->bufid=0;
	}
	
	alGenBuffers(1, &p->bufid);

#if KLAL_ERRCHECK
	ALenum aen = alGetError();
	if( aen!=AL_NO_ERROR )
	{
		KLLog("[ KLAL ] %s %u\n", __func__, aen);
	}
#endif
	
}
void KLAL_RebuildAll(KLAL* p)
{
	if( !p )
		p = &klal;
	
	p->current = 0;//(p->current+1)%KLAL_AL_RINGNUM;
	
	if(p->bufid)
	{
		alSourcei( p->src, AL_BUFFER, 0 );
#if KLAL_ERRCHECK
		ALenum aen = alGetError();
		if( aen!=AL_NO_ERROR )
		{
			KLLog("[ KLAL ] %s %u\n", __func__, aen);
		}
#endif
		alDeleteBuffers( 1, &p->bufid );
#if KLAL_ERRCHECK
		aen = alGetError();
		if( aen!=AL_NO_ERROR )
		{
			KLLog("[ KLAL ] %s %u\n", __func__, aen);
		}
#endif
		p->bufid=0;
	}
#if KLAL_ERRCHECK
	ALenum aen = alGetError();
	if( aen!=AL_NO_ERROR )
	{
		KLLog("[ KLAL ] %s %u\n", __func__, aen);
	}
#endif
	
	if( p->src )
	{
		alDeleteSources( 1, &p->src );
		p->src = 0;
	}
	
	alGenSources (1, &p->src);
#if KLAL_ERRCHECK
	aen = alGetError();
	if( aen!=AL_NO_ERROR )
	{
		KLLog("[ KLAL ] %s %u\n", __func__, aen);
	}
#endif
	
	alGenBuffers(1, &p->bufid);
#if KLAL_ERRCHECK
	aen = alGetError();
	if( aen!=AL_NO_ERROR )
	{
		KLLog("[ KLAL ] %s %u\n", __func__, aen);
	}
#endif
}


void KLAL_Interruption( void* pClientData, UInt32 state )
{
	if (state == kAudioSessionBeginInterruption)
	{
		KLLog("AL interrup. START");
		alcMakeContextCurrent (NULL);
	}
	else if (state == kAudioSessionEndInterruption)
	{
		KLLog("AL interrup. END");
		alcMakeContextCurrent (klal_pContext);
	}
	
    // その他の割込処理
	
}

void KLAL_InitSystem()
{
	
	OSStatus err = AudioSessionInitialize(NULL, NULL, KLAL_Interruption, NULL);
	if(err)
	{
		KLLog("[ KLAL ] ERR: AudioSessionInitialize:%x\n", (int)err);
	}
	
	err = AudioSessionSetActive(TRUE);
	if(err)
	{
		KLLog("[ KLAL ] ERR: AudioSessionSetActive:%x\n", (int)err);
	}
	
	UInt32 sessionCategory = kAudioSessionCategory_AmbientSound;
	
	AudioSessionSetProperty( kAudioSessionProperty_AudioCategory,
							 sizeof (sessionCategory),
							 &sessionCategory );
	
	klal_pDevice	= alcOpenDevice(NULL);
	klal_pContext= alcCreateContext(klal_pDevice, NULL);
#if KLAL_ERRCHECK
	ALenum aen = alGetError();
	if( aen!=AL_NO_ERROR )
	{
		KLLog("[ KLAL ] %s %u\n", __func__, aen);
	}
#endif
	alcMakeContextCurrent(klal_pContext);
	klal_is_loaded = TRUE;
}

void KLAL_UnloadSystem()
{	
	
	KLAL* p = &klal;

	OSStatus err = AudioSessionSetActive (false);
	if(err)
	{
		KLLog("[ KLAL ] ERR: AudioSetActiveFalse for unload:%x\n", (int) err);
	}
	
	if( p->src )
	{
		alSourceStop( p->src );
		alDeleteSources (1, &p->src);
	}
	
	if( p->bufid )
		alDeleteBuffers (1, &p->bufid);
	
	alcMakeContextCurrent(NULL);
	
	if ( klal_pContext )
	{
		alcDestroyContext(klal_pContext);
		klal_pContext= NULL;
	}
	
	alcCloseDevice(klal_pDevice);
	klal_pDevice	= NULL;
	klal_is_loaded = FALSE;
	
	KLLog("[ KLAL ] Unloaded.\n");
	
}

void KLAL_Retain(KLAL* p)
{
	if( !p )
		p = &klal;
	
	p->retain = TRUE;
}

void KLAL_Reload(KLAL* p)
{
	if( !p )
		p = &klal;
	
	if( p->retain )
	{
		KLAL_RebuildAll(p);
		p->isLoaded	= FALSE;
	}
}

void KLAL_Init(KLAL* p)
{
	if( !p )
		p = &klal;
	
	p->isLoaded	= FALSE;
	p->current	= 0;
	p->retain	= FALSE;
	
	if( !klal_is_loaded ){
		
		KLAL_InitSystem();
		memset( p, 0, sizeof(KLAL) );
		p->firstDelete = TRUE;
		
		//alGenSources (1, &p->src);
		//aen = alGetError();
	}
	alGenSources (1, &p->src);
	
	p->pData = NULL;
	
	KLAL_RebuildBuffer(p);
	
}

void KLAL_Unload(KLAL* p)
{	
	if( !p )
		p = &klal;
	
	if(p->isLoaded)
	{
		KLAL_Stop(p);
		alSourcei( p->src, AL_BUFFER, 0 );
#if KLAL_ERRCHECK
		ALenum aen = alGetError();
		if( aen!=AL_NO_ERROR )
		{
			KLLog("[ KLAL ] \n\tFile:%s\n\tLine:%u\n\tErr:%x\n", __FUNCTION__, __LINE__, aen);
		}
#endif
		alDeleteBuffers( 1, &p->bufid );
#if KLAL_ERRCHECK
		aen = alGetError();
		if( aen!=AL_NO_ERROR )
		{
			KLLog("[ KLAL ] \n\tFile:%s\n\tLine:%u\n\tErr:%x\n", __FUNCTION__, __LINE__, aen);
		}
#endif
		p->bufid = 0;
		alDeleteSources( 1, &p->src );
#if KLAL_ERRCHECK
		aen = alGetError();
		if( aen!=AL_NO_ERROR )
		{
			KLLog("[ KLAL ] \n\tFile:%s\n\tLine:%u\n\tErr:%x\n", __FUNCTION__, __LINE__, aen);
		}
#endif
		p->src = 0;
		sfree( p->pData );
		p->isLoaded = 0;
	}
}

static bl KLAL_LoadCommonInit( KLAL* p, path8 pPath )
{
	if( !p )
		p = &klal;
	
	if( p->isLoaded )
	{
		if(pPath)
		{
			sfree(p->pData);
		
			alSourcei( p->src, AL_BUFFER, 0 );
			alDeleteBuffers( 1, &p->bufid );
#if KLAL_ERRCHECK
			ALenum aen = alGetError();
			if( aen!=AL_NO_ERROR )
			{
				KLLog("[ KLAL ] \n\tFile:%s\n\tLine:%u\n\tErr:%x\n", __FUNCTION__, __LINE__, aen);
			}
#endif
			p->bufid = 0;
			//alDeleteSources (1, &p->src);
			//aen = alGetError();
			//p->src = 0;
			KLAL_RebuildBuffer(p);
		}	
		
	}
	else
	{
		p->pData = NULL;
	}
	
	return TRUE;
}

static bl KLAL_LoadCore( KLAL* p )
{
	if( !p )
		p = &klal;
	
	if( !p->isLoaded && p->pData )
	{
		//alSourcei( p->src, AL_BUFFER, 0 );
		if(p->wav.channel==1)
		{
			if(p->wav.bit==8)
			{
				alBufferData( p->bufid, AL_FORMAT_MONO8, p->pData, p->wav.sz, p->wav.freq);
			}
			else
			{
				alBufferData( p->bufid, AL_FORMAT_MONO16, p->pData, p->wav.sz, p->wav.freq);
			}
		}
		else
		{
			if(p->wav.bit==8)
			{
				alBufferData( p->bufid, AL_FORMAT_STEREO8, p->pData, p->wav.sz, p->wav.freq);
			}
			else
			{
				alBufferData( p->bufid, AL_FORMAT_STEREO16, p->pData, p->wav.sz, p->wav.freq);
			}
		}
		
		if( !p->retain )
		{
			sfree(p->pData);
		}
		
#if KLAL_ERRCHECK
		ALenum aen = alGetError();
		if( aen!=AL_NO_ERROR )
		{
			KLLog("[ KLAL ] \n\tFile:%s\n\tLine:%u\n\tErr:%x\n", __FUNCTION__, __LINE__, aen);
		}
#endif
		
		alSourcei( p->src, AL_BUFFER, p->bufid );
		//alSourcei( p->src, AL_LOOPING, AL_FALSE );
#if KLAL_ERRCHECK
		aen = alGetError();
		if( aen!=AL_NO_ERROR )
		{
			KLLog("[ KLAL ] \n\tFile:%s\n\tLine:%u\n\tErr:%x\n", __FUNCTION__, __LINE__, aen);
		}
#endif
		p->isLoaded = TRUE;
		
	}
	
	/*
	ALfloat test = 0;
	alSourcef( p->src, AL_MIN_GAIN, 0.1f);
	alGetSourcef( p->src, AL_MIN_GAIN, &test);
	KLLog("Processed:%f", test);
	*/
	static bl debug = FALSE;
	if(debug)
	{
		alSourcei( p->src, AL_BUFFER, 0 );
		alSourcei( p->src, AL_BUFFER, p->bufid );
		//alSourcei( p->src, AL_LOOPING, AL_FALSE );
#if KLAL_ERRCHECK
		ALenum aen = alGetError();
		if( aen!=AL_NO_ERROR )
		{
			KLLog("[ KLAL ] \n\tFile:%s\n\tLine:%u\n\tErr:%x\n", __FUNCTION__, __LINE__, aen);
		}
#endif
	}
	
	return TRUE;
}

bl KLAL_LoadCafFromTmp( KLAL* p, char* pFileName)
{
	size_t len = 0;
	
	if( !p )
		p = &klal;	
	
	KLAL_LoadCommonInit(p,pFileName);
	
	if(pFileName)
	{
		if( !KLAL_SetCaf( p, KLFile_ReadTmp(pFileName, &len), len ) )
		{
			return FALSE;
		}
		KLAL_LoadCore(p);
	}
	
	return TRUE;
	
}

bl KLAL_LoadWavFromTmp( KLAL* p, char* pFileName)
{
	size_t len = 0;
	
	if( !p )
		p = &klal;	
	
	KLAL_LoadCommonInit(p,pFileName);
	
	if(pFileName)
	{
		if( !KLAL_SetWav( p, KLFile_ReadTmp(pFileName, &len) ) )
		{
			return FALSE;
		}
	}
	
	return TRUE;
	
}

bl KLAL_LoadWav( KLAL* p, path8 pPath ){
	
	if( !p )
		p = &klal;
	
	KLAL_LoadCommonInit(p,pPath);
	
	if(pPath)
	{
		if( !KLAL_LoadWavCore( p, pPath ) )
		{
			return FALSE;
		}
		p->formattype = KLAL_WAVE;
	}
	
#if KLAL_ERRCHECK
	ALenum aen = alGetError();
	if( aen!=AL_NO_ERROR )
	{
		KLLog("[ KLAL ] \n\tFile:%s\n\tLine:%u\n\tErr:%x\n", __FUNCTION__, __LINE__, aen);
	}
#endif
	
	KLAL_LoadCore(p);

	return TRUE;

}

/// 拡張子にwavがあればwav / それ以外はm4aとしてロード
bl KLAL_Load( KLAL* p, path8 pPath )
{
	size_t len = pPath?cstrlen(pPath):0;
	
	if( pPath && len>4 )
	{
		KLAL_Init(p);
		
		if( pPath[len-1]=='v' && pPath[len-2]=='a' && pPath[len-3]=='w' )
		{
			return KLAL_LoadWav(p, pPath);
		}
			
		return KLAL_LoadM4a(p, pPath);
		
	}
	
	return FALSE;
}

bl KLAL_Play(KLAL* p, ALfloat sec, bl isLoop ){
	
	if( !p )
		p = &klal;
	
	KLAL_LoadCore(p);
	
	if( !p->isLoaded )
		return  FALSE;
	
	alSourcef( p->src, AL_SEC_OFFSET, sec );
	alSourcei( p->src, AL_LOOPING, isLoop);
	
#if KLAL_ERRCHECK
	ALenum aen = alGetError();
	if( aen!=AL_NO_ERROR )
	{
		KLLog("[ KLAL ] \n\tFile:%s\n\tLine:%u\n\tErr:%x\n", __FUNCTION__, __LINE__, aen);
	}
#endif
	
	//alGetSourcef( p->src, AL_SEC_OFFSET, &f );
	//KLLog("[ KLAL ] offset = %f\n",f);

	alSourcePlay(p->src);

#if KLAL_ERRCHECK
	aen = alGetError();
	if( aen!=AL_NO_ERROR )
	{
		KLLog("[ KLAL ] \n\tFile:%s\n\tLine:%u\n\tErr:%x\n", __FUNCTION__, __LINE__, aen);
	}
#endif
	
	return TRUE;
}

bl KLAL_StopWithCheck( KLAL* p, ALfloat stopsec )
{
	
	if( !p )
		p = &klal;
	
	if( stopsec )
	{
		float f = 0;
		alGetSourcefv( p->src, AL_SEC_OFFSET, &f );
		if( stopsec<=f )
		{
			KLAL_Stop(p);
			return TRUE;
		}
	}
	return FALSE;
}

void KLAL_Stop(KLAL* p)
{	
	if( !p )
		p = &klal;
	
	alSourceStop(p->src);
	//ALenum aen = alGetError();
}

bl KLAL_IsPlaying(KLAL* p)
{
	ALint v;
	
	if( !p )
		p = &klal;
	
	alGetSourceiv( p->src, AL_SOURCE_STATE, &v );
	return v==AL_PLAYING;
	
}

bl KLAL_IsStopped(KLAL* p)
{
	ALint v;
	
	if( !p )
		p = &klal;
	
	alGetSourceiv( p->src, AL_SOURCE_STATE, &v );
	return v==AL_STOPPED;
	
}

// メモ：毎フレームPlayを連続でコールすると
// OpenALのSTATEがPLAYからずっと戻らない事がある？？

