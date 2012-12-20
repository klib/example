//
//  KLMath.c
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

#include "KLMath.h"
#include "KLRandom.h"

//============================================================
/**
 *	@brief	指定値から最上位ビットだけが1の時の数値を求め返す<br/>
 *			例)32〜63→32   128〜255→128
 *
 *	@param	n	調べる数値
 *
 *	@return	1になっている最上位ビットだけ有効、他ビットは0にした時の数値
 */
//============================================================
u32 KLMath_GetTop1bitOnlyNumber( u32 n ){
	
	n = n | (n >>  1);
	n = n | (n >>  2);
	n = n | (n >>  4);
	n = n | (n >>  8);
	n = n | (n >> 16);
	
	return n ^ (n >> 1);
}

// ハッシュ計算に使用するキー
// デフォルトは適当でそのまま使うと解析されやすいので
// アプリごとに適当な数値を設定してください。
#define KLMATH_HASHKEY_X (0x12)
#define KLMATH_HASHKEY_Y (0x3456)
#define KLMATH_HASHKEY_Z (0x7890)
#define KLMATH_HASHKEY_W (0x1234)

// マスクも色々変えて値の変化を確認してください
#define KLMATH_HASHMASK_X (0xFF)
#define KLMATH_HASHMASK_Y (0xFFFFFFFF)
#define KLMATH_HASHMASK_Z (0xFFFFFFFF)
#define KLMATH_HASHMASK_W (0xFFFFFFFF)

//============================================================
/**
 *	@brief	指定サイズのバイト配列に簡易暗号処理をかける<br/>
 *			ランダムシードは復元にも全く同じものを与える事<br/>
 *			また、アプリリリース後にハッシュキー、乱数アルゴリズムを変えてしまうと<br/>
 *			当然変更前に保存したデータと整合が取れなくなるので注意する事。
 *
 *	@param	p		暗号化する任意アドレス(バイナリの先頭など)
 *	@param	sz		そのアドレスから何バイトをハッシュ計算に使用するか(バイナリサイズなど)
 *	@param	seeda	ランダムシードA
 *	@param	seeda	ランダムシードB
 *	@param	seeda	ランダムシードC
 *	@param	seeda	ランダムシードD
 *
 */
//============================================================
void KLMath_Encrypt(void* p, size_t sz, u32 seeda, u32 seedb, u32 seedc, u32 seedd ){
	
	u8* pu8 = (u8*)p;
	
	KL_srand( seeda, seedb, seedc, seedd );
	
	for( u32 i=0; i<sz; ++i ){
		
		pu8[i] ^= (u8)KL_rand();
		
	}
}


//============================================================
/**
 *	@brief	指定サイズのバイト配列にかけられた簡易暗号化を複合する<br/>
 *			ランダムシードは暗号時と全く同じものを与える事<br/>
 *			また、アプリリリース後にハッシュキー、乱数アルゴリズムを変えてしまうと<br/>
 *			当然変更前に保存したデータと整合が取れなくなるので注意する事。
 *
 *	@param	p		暗号化する任意アドレス(バイナリの先頭など)
 *	@param	sz		そのアドレスから何バイトをハッシュ計算に使用するか(バイナリサイズなど)
 *	@param	seeda	ランダムシードA
 *	@param	seeda	ランダムシードB
 *	@param	seeda	ランダムシードC
 *	@param	seeda	ランダムシードD
 *
 */
//============================================================
void KLMath_Decrypt(void* p, size_t sz, u32 seeda, u32 seedb, u32 seedc, u32 seedd ){
	
	u8* pu8 = (u8*)p;
	
	KL_srand( seeda, seedb, seedc, seedd );
	
	for( u32 i=0; i<sz; ++i ){
		
		pu8[i] ^= (u8)KL_rand();
		
	}
}

//============================================================
/**
 *	@brief	指定サイズのバイト配列からハッシュの作成<br/>
 *			関数内の各種数値は適当に加算、乗算、XORしているものなので、<br/>
 *			デフォルト値で予測されないようにアプリ毎に変更する事を推奨 <br/>
 *			また、アプリリリース後にハッシュキー、乱数アルゴリズムを変えてしまうと<br/>
 *			当然変更前に保存したハッシュと整合が取れなくなるので注意する事。
 *
 *	@param	p	ハッシュ計算をする任意アドレス(バイナリの先頭など)
 *	@param	sz	そのアドレスから何バイトをハッシュ計算に使用するか(バイナリサイズなど)
 *
 *	@return	算出した4byteハッシュ
 */
//============================================================
u32 KLMath_GetHash32(void* p,size_t sz){
	
	u32		ret = 0;
	bin8*	p8	= (bin8*)p;
	
	for(size_t i=0; i<sz; ++i)
	{
		KL_srand(
			p8[i]^KLMATH_HASHMASK_X,
			(p8[i]+KLMATH_HASHKEY_X)^KLMATH_HASHMASK_Y,
			(p8[i]+3)*KLMATH_HASHKEY_Y,
			(p8[i]+KLMATH_HASHKEY_Z)*KLMATH_HASHKEY_W);
		
		ret += (KL_rand()+KL_rand())^KLMATH_HASHMASK_Z;
	}
	
	return ret^KLMATH_HASHMASK_W;
}

//=========================================================
/**
 *	@brief	二点間の角度を取得
 *
 *	@param	x1	始点X座標
 *	@param	y1	始点Y座標
 *	@param	x2	終点X座標
 *	@param	y2	終点Y座標
 *
 *	@return	360==0x10000度のワイド角度を返す
 */
//=========================================================
wdeg KLMath_Dir( s32 x1, s32 y1, s32 x2, s32 y2 ){
	
	s32		dx		= ( x2 - x1 );
	s32		dy		= ( y2 - y1 );
	return KL_WDEG_CYCLE*0.5f*atan2f(dy, dx)/PI;
	
}

//=========================================================
/**
 *	@brief	二点間の距離を取得
 */
//=========================================================
float KLMath_Distance( s32 x1, s32 y1, s32 x2, s32 y2 ){
	
	u32 dx	= klabs( x2 - x1 );
	u32 dy	= klabs( y2 - y1 );
	return sqrtf ( dx*dx + dy*dy );
	
}

//=========================================================
/**
 *	@brief	ワイド角度を360度に丸める
 */
//=========================================================
float KLMath_Wdeg2deg( wdeg wd ){
	
	return wd * 360.0f / KL_WDEG_CYCLE;
	
}

//=========================================================
/**
 *	@brief	ワイド角度を時計の方向に変換
 */
//=========================================================
float KLMath_Wdeg2clockway( wdeg wd ){
	
	return (wd-KL_WDEG_CYCLE*0.25f) * 12.0f / KL_WDEG_CYCLE;
	
}


inline void FLG64_On( flg64* pFlg64, u64 bits ){
	
	*pFlg64 |= bits;
	
}

inline void FLG32_On( flg32* pFlg32, u32 bits ){
	
	*pFlg32 |= bits;
	
}

inline void FLG16_On( flg16* pFlg16, u16 bits ){
	
	*pFlg16 |= bits;
	
}

inline void FLG8_On( flg8* pFlg8, u8 bits ){
	
	*pFlg8 |= bits;
	
}

inline void FLG64_Off( flg64* pFlg64, u64 bits ){
	
	*pFlg64 &= (0xFFFFFFFFFFFFFFFF^bits);
	
}

inline void FLG32_Off( flg32* pFlg32, u32 bits ){
	
	*pFlg32 &= (0xFFFFFFFF^bits);
	
}

inline void FLG16_Off( flg16* pFlg16, u16 bits ){
	
	*pFlg16 &= (0xFFFF^bits);
	
}

inline void FLG8_Off( flg8* pFlg8, u8 bits ){
	
	*pFlg8 &= (0xFF^bits);
	
}

inline void FLG64_Swap( flg64* pFlg64, u64 bits ){
	
	*pFlg64 ^= bits;
	
}

inline void FLG32_Swap( flg32* pFlg32, u32 bits ){
	
	*pFlg32 ^= bits;
	
}

inline void FLG16_Swap( flg16* pFlg16, u16 bits ){
	
	*pFlg16 ^= bits;
	
}

inline void FLG8_Swap( flg8* pFlg8, u8 bits ){
	
	*pFlg8 ^= bits;
	
}

inline bl FLG64_And( flg64 _flg64, u64 bits ){
	
	return (_flg64) & (0xFFFFFFFFFFFFFFFF&bits);
	
}

inline bl FLG32_And( flg32 _flg32, u32 bits ){
	
	return (_flg32) & (0xFFFFFFFF&bits);
	
}

inline bl FLG16_And( flg16 _flg16, u16 bits ){
	
	return (_flg16) & (0xFFFF&bits);
	
}

inline bl FLG8_And( flg8 _flg8, u8 bits ){
	
	return (_flg8) & (0xFF&bits);
	
}



