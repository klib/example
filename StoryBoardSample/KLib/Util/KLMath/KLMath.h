//
//  KLMath.h
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

#ifndef KLib_KLMath_h
#define KLib_KLMath_h

#include "c_extern.h"
#include "KLibDefine.h"
#include <math.h>

/// 指定最大幅からUV値を求める
#define GetUVf( max_range, pos ) (((float)pos)/((float)max_range))

u32		KLMath_GetTop1bitOnlyNumber( u32 n );

u32		KLMath_GetHash32(void* p,size_t sz);
void	KLMath_Encrypt(void* p, size_t sz, u32 seeda, u32 seedb, u32 seedc, u32 seedd );
void	KLMath_Decrypt(void* p, size_t sz, u32 seeda, u32 seedb, u32 seedc, u32 seedd );

wdeg	KLMath_Dir( s32 x1, s32 y1, s32 x2, s32 y2 );
float	KLMath_Distance( s32 x1, s32 y1, s32 x2, s32 y2 );
KLPower KLPower_ReflectDirX( KLPower pow );
KLPower	KLPower_ReflectDirY( KLPower pow );

void FLG64_On( flg64* pFlg64, u64 bits );
void FLG32_On( flg32* pFlg32, u32 bits );
void FLG16_On( flg16* pFlg16, u16 bits );
void FLG8_On( flg8* pFlg8, u8 bits );
void FLG64_Off( flg64* pFlg64, u64 bits );
void FLG32_Off( flg32* pFlg32, u32 bits );
void FLG16_Off( flg16* pFlg16, u16 bits );
void FLG8_Off( flg8* pFlg8, u8 bits );
void FLG64_Swap( flg64* pFlg64, u64 bits );
void FLG32_Swap( flg32* pFlg32, u32 bits );
void FLG16_Swap( flg16* pFlg16, u16 bits );
void FLG8_Swap( flg8* pFlg8, u8 bits );
bl FLG64_And( flg64 _flg64, u64 bits );
bl FLG32_And( flg32 _flg32, u32 bits );
bl FLG16_And( flg16 _flg16, u16 bits );
bl FLG8_And( flg8 _flg8, u8 bits );

#include "c_extern_close.h"
#endif
